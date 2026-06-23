// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_UseBestSkill.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../../Skill/TargetingUtil.h"
#include "../../Skill/SkillTrigger.h"
#include "../../Skill/Targeting.h"
#include "../../Skill/SkillComponent.h"

static USkillComponent* GetSC(APawn* P) { return P ? P->FindComponentByClass<USkillComponent>() : nullptr; }

UBTTask_UseBestSkill::UBTTask_UseBestSkill()
{
	NodeName = TEXT("Use Best Skill");
	BB_CurrentTarget.SelectedKeyName = "CurrentTarget";
	BB_DesiredCastLocation.SelectedKeyName = "DesiredCastLocation";
	BB_WantsSpatialSkill.SelectedKeyName = "WantsCastSpatialSkill";
	BB_LastSkillTried.SelectedKeyName = "LastSkillTried";
	BB_SkillDesiredIndex.SelectedKeyName = "SkillDesiredIndex";
}

bool UBTTask_UseBestSkill::IsSpatialSkill(const FSkillData& D) const
{
	if (D.TargetingClass)
	{
		if (const UTargeting_PointRadius* CDO = Cast<UTargeting_PointRadius>(D.TargetingClass->GetDefaultObject()))
			return CDO->bSnapToGround;
		if (bAssumeSpatialIfTargetingClass) return true;
		
	}
	return (D.Radius > 0.f || D.ConeHalfAngle > 0.f);
}


float UBTTask_UseBestSkill::EstimateValueSimple(APawn* Caster, AActor* Target, const FSkillData& D) const
{
	float S = 0.f;

	if (Target)
	{
		const float Dist = FVector::Dist(Caster->GetActorLocation(), Target->GetActorLocation());
		const float R = FMath::Max(D.Range, 250.f);
		if (Dist <= R * 1.2f) S += 1.f;
	}
	if (D.Radius > 0.f || D.ConeHalfAngle > 0.f) S += 0.3f;
	if (D.Cooldown > 0.f) S += 0.2f / (1.f + D.Cooldown * 0.1f);
	if (D.Animation) S += 0.1f;
	return S;
}

EBTNodeResult::Type UBTTask_UseBestSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Pawn = AI ? AI->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BB_CurrentTarget.SelectedKeyName));
	const FVector DesiredLoc = BB->GetValueAsVector(BB_DesiredCastLocation.SelectedKeyName);
	const bool bHasDesiredLoc = !DesiredLoc.IsNearlyZero();

	if (USkillComponent* SC = GetSC(Pawn))
	{
		float BestV = -FLT_MAX;
		int32 BestIdx = INDEX_NONE;
		bool bSpatial = false;

		const FName Last = BB->GetValueAsName(BB_LastSkillTried.SelectedKeyName);

		for (int32 i = MinSkillIndex; i <= MaxSkillIndex; ++i)
		{
			USkill* S = SC->GetSkillSlot(i);
			if (!S || !S->IsReady()) continue;

			const FSkillData D = S->GetSkillData();
			if (bAvoidRepeat && Last == D.SkillID) continue;

			const bool bthisSpatial = IsSpatialSkill(D);
			float V = EstimateValueSimple(Pawn, Target, D);
			if (bthisSpatial && !bHasDesiredLoc) V += 0.25f;

			if (V > BestV) { BestV = V; BestIdx = i; bSpatial = bthisSpatial; }
		}

		if (BestIdx == INDEX_NONE) return EBTNodeResult::Failed;

		USkill* Best = SC->GetSkillSlot(BestIdx);
		if (!Best) return EBTNodeResult::Failed;

		if (bSpatial && !bHasDesiredLoc && BestV >= SpatialPickThreshold)
		{
			BB->SetValueAsBool(BB_WantsSpatialSkill.SelectedKeyName, true);
			BB->SetValueAsName(BB_LastSkillTried.SelectedKeyName, Best->GetSkillData().SkillID);
			BB->SetValueAsInt(BB_SkillDesiredIndex.SelectedKeyName, BestIdx);
			return EBTNodeResult::Succeeded;
		}
		const FVector UseLoc = (bSpatial ? DesiredLoc : FVector::ZeroVector);
		const bool OK = SC->TryUseSKill(BestIdx, Target, UseLoc);

		if (OK)
		{
			BB->SetValueAsName(BB_LastSkillTried.SelectedKeyName, Best->GetSkillData().SkillID);
			BB->SetValueAsInt(BB_SkillDesiredIndex.SelectedKeyName, BestIdx);
			if (bSpatial)
			{
				BB->SetValueAsBool(BB_WantsSpatialSkill.SelectedKeyName, false);
				BB->SetValueAsVector(BB_DesiredCastLocation.SelectedKeyName, FVector::ZeroVector);
			}
			return EBTNodeResult::Succeeded;
		}
	}
	return EBTNodeResult::Failed;
}

