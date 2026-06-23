// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateSensedTargets.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "../../Building/BuildingBase.h"
#include "NavigationSystem.h"

static bool IsDeadOrNeutral(const AActor* A)
{
	if (!IsValid(A)) return true;

	// 1) 팀 Neutral(NoTeam=255) 체크
	if (const IGenericTeamAgentInterface* T = Cast<const IGenericTeamAgentInterface>(A))
	{
		if (T->GetGenericTeamId().GetId() == 255) return true;
	}

	return false;
}


static void GetSightCandidates(AAIController* AI, TArray<AActor*>& Out)
{
	Out.Reset();
	if (!AI) return;

	if (auto* Per = AI->FindComponentByClass<UAIPerceptionComponent>())
	{
		TArray<AActor*> Tmp;
		Per->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), Tmp);

		for (AActor* A : Tmp)
		{
			if (!IsValid(A)) continue;
			if (IsDeadOrNeutral(A))
			{
				// 인식 캐시에서도 비워주면 더 깔끔
				Per->ForgetActor(A);
				continue;
			}
			if (AI->GetTeamAttitudeTowards(*A) == ETeamAttitude::Hostile)
			{
				Out.Add(A);
			}
		}
	}
}

UBTService_UpdateSensedTargets::UBTService_UpdateSensedTargets()
{
	Interval = 0.4f; 
	RandomDeviation = 0.1f;
	BB_CurrentTarget.SelectedKeyName = "CurrentTarget";
	BB_InAttackRange.SelectedKeyName = "InAttackRange";
	BB_HasLoS.SelectedKeyName = "HasLoSToTarget";
	BB_ThreatScore.SelectedKeyName = "ThreatScore";
	BB_DistanceToTarget.SelectedKeyName = "DistanceToTarget";

	//태그를 여기다 설정을 해놓으면 태그에 따라서 사거리가 달라진다눙~
	TagAttackRange.Add("Player", 400.f);
	TagAttackRange.Add("GreatSword", 300.f);
	TagAttackRange.Add("Sword", 300.f);
	TagAttackRange.Add("Sage", 600.f);
	TagAttackRange.Add("Shield", 300.f);
	TagAttackRange.Add("Bow", 800.f);
	TagAttackRange.Add("Catapult", 800.f);
	TagAttackRange.Add("Staff", 800.f);
	TagAttackRange.Add("Throwing", 800.f);
	TagAttackRange.Add("Destroyer", 300.f);
	TagAttackRange.Add("Bomb", 150.f);
	TagAttackRange.Add("Dagger", 150.f);
	TagAttackRange.Add("Building", 200.f);
}



void UBTService_UpdateSensedTargets::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsBool(BB_InAttackRange.SelectedKeyName, false);
		BB->SetValueAsBool(BB_HasLoS.SelectedKeyName, false);
		BB->SetValueAsFloat(BB_ThreatScore.SelectedKeyName, 0.f);
	}
}
void UBTService_UpdateSensedTargets::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Self = AI ? AI->GetPawn() : nullptr;
	if (!AI || !Self) return;

	TArray<AActor*> Cands; GetSightCandidates(AI, Cands);

	Cands.RemoveAll([AI](AActor* A)
		{
			if (!IsValid(A)) return true;
			if (IsDeadOrNeutral(A)) return true;
			return AI->GetTeamAttitudeTowards(*A) != ETeamAttitude::Hostile;
		});
	float BestScore = 0.f;
	AActor* Best = PickBest(Self, Cands, AI, BestScore);

	if (auto* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsObject(BB_CurrentTarget.SelectedKeyName, Best);

		bool bLoS = false, bInRange = false;
		float Dist = 0.f;

		if (Best)
		{
			bLoS = HasLoS(AI, Best);

			if (ABuildingBase* BBB = Cast<ABuildingBase>(Best))
			{
				Dist = FVector::Dist(Self->GetActorLocation(), BBB->GetActorLocation());

				float BounderyDist;
				float Offset = 20;
				FVector B2UVector = Self->GetActorLocation() - BBB->GetActorLocation();
				FVector Origin;
				FVector Extent;
				BBB->GetActorBounds(true, Origin, Extent);
				float X_axis = B2UVector.ProjectOnTo(BBB->GetActorForwardVector() * Dist).Length();
				float Y_axis = B2UVector.ProjectOnTo(BBB->GetActorRightVector() * Dist).Length();

				if (X_axis > Y_axis)
				{
					BounderyDist = Dist * (Extent.X / X_axis) + Offset;
				}
				else if (X_axis < Y_axis)
				{
					BounderyDist = Dist * (Extent.Y / Y_axis) + Offset;
				}
				else
				{
					BounderyDist = FVector2d(Extent.X, Extent.Y).Length() + Offset;
				}
				Dist -= BounderyDist;
			}
			else
				Dist = FVector::Dist(Self->GetActorLocation(), Best->GetActorLocation());
			const float R = GetAttackRangeForTarget(Self);
			bInRange = (Dist <= R);
		}
		BB->SetValueAsBool(BB_HasLoS.SelectedKeyName, bLoS);
		BB->SetValueAsBool(BB_InAttackRange.SelectedKeyName, bInRange);
		BB->SetValueAsFloat(BB_ThreatScore.SelectedKeyName, Best ? BestScore : 0.f);
		BB->SetValueAsFloat(BB_DistanceToTarget.SelectedKeyName, Dist);

		if (!Best)
		{
			 BB->ClearValue(BB_CurrentTarget.SelectedKeyName);
			 BB->ClearValue(BB_ThreatScore.SelectedKeyName);
			 BB->ClearValue(BB_DistanceToTarget.SelectedKeyName);
		}
	}
}

float UBTService_UpdateSensedTargets::GetAttackRangeForTarget(AActor* Target) const
{
	if (!Target) return DefaultAttackRange;
	for (const TPair<FName, float>& Pair : TagAttackRange)
	{
		if (Target->ActorHasTag(Pair.Key))
		{
			return Pair.Value;
		}
	}
	return DefaultAttackRange;
}

float UBTService_UpdateSensedTargets::ClassScore(const AActor* Target) const
{
	float Out = 1.f;
	for (const auto& P : ClassPriority.Priority)
		if (Target && Target->ActorHasTag(P.Key))
			Out = FMath::Max(Out, P.Value);
	return Out;
}

bool UBTService_UpdateSensedTargets::HasLoS(AAIController* AI, AActor* Target) const
{
	return (AI && Target) ? AI->LineOfSightTo(Target) : false;
}

float UBTService_UpdateSensedTargets::NormInvDistance(APawn* Self, AActor* Target) const
{
	const float D = (Self && Target) ? FVector::Dist(Self->GetActorLocation(), Target->GetActorLocation()) : 1e6f;
	return 1.f / FMath::Max(D, 1.f);
}



AActor* UBTService_UpdateSensedTargets::PickBest(APawn* Self, const TArray<AActor*>& Candidates, AAIController* AI, float& OutScore) const
{
	float BestS = -FLT_MAX; AActor* Best = nullptr;
	for (AActor* T : Candidates)
	{
		float S = 0.f;
		S += W_Class * ClassScore(T);
		S += W_LoS * (HasLoS(AI, T) ? 1.f : 0.f);
		S += W_Distance * NormInvDistance(Self, T);
		if (S > BestS) { BestS = S; Best = T; }
	}
	OutScore = (Best ? BestS : 0.f);
	return Best;
}
