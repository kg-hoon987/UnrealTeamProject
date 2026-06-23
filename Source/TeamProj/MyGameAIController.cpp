// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Character/GameCharacterBase.h"
#include "Status/StatusComponent.h"
#include "Kismet/KismetSystemLibrary.h"



AMyGameAIController::AMyGameAIController()
{
	//퍼셉션 컴포넌트
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception"));
	SetPerceptionComponent(*PerceptionComp);

	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMyGameAIController::OnPerceptionUpdated);
	
	
	
}



void AMyGameAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Blackboard) return;
	if (Stimulus.WasSuccessfullySensed())
	{
		if (GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
			Blackboard->SetValueAsObject(KeyTargetActor, Actor);
	}
	else
	{
		// 잃었을 때, 현재 타깃이면 해제
		if (Blackboard->GetValueAsObject(KeyTargetActor) == Actor)
			Blackboard->ClearValue(KeyTargetActor);
	}
}

void AMyGameAIController::InitBlackboardAndBT()
{
	if (!ensure(BBData && BTData)) return;

	UBlackboardComponent* OutBB = nullptr;
	UseBlackboard(BBData, OutBB);
	RunBehaviorTree(BTData);
}

void AMyGameAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (const IGenericTeamAgentInterface* T = Cast<IGenericTeamAgentInterface>(InPawn))
	{
		SetGenericTeamId(T->GetGenericTeamId());
	}
	else if (IGenericTeamAgentInterface* W = Cast<IGenericTeamAgentInterface>(InPawn))
		W->SetGenericTeamId(GetGenericTeamId());

	InitBlackboardAndBT();
	if (Blackboard)
	{
		Blackboard->SetValueAsBool(KeyHasLOS, false);
		Blackboard->SetValueAsBool(KeyInAttackRange, false);
	}

}

static bool IsPAB(uint8 id) //플레이어 아군 빌딩 묶음
{
	return	(id == static_cast<uint8>(ETeam::Player)) ||
			(id == static_cast<uint8>(ETeam::Ally)) ||
			(id == static_cast<uint8>(ETeam::Building));
}


ETeamAttitude::Type AMyGameAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* SA = Cast<IGenericTeamAgentInterface>(this);
	const IGenericTeamAgentInterface* SB = Cast<IGenericTeamAgentInterface>(&Other);

	const FGenericTeamId A = SA ? SA->GetGenericTeamId() : FGenericTeamId::NoTeam;
	const FGenericTeamId B = SB ? SB->GetGenericTeamId() : FGenericTeamId::NoTeam;

	if (A == B) return ETeamAttitude::Friendly;

	if (IsPAB(A.GetId()) && IsPAB(B.GetId())) return ETeamAttitude::Friendly; 
	return ETeamAttitude::Hostile;
}

void AMyGameAIController::SetMainCastle(AActor* InCastle) //스테이지쪽관리에서 이 함수를 한번만 불러주면 블랙보드에 들어감
{
	if (Blackboard && InCastle)
	{
		Blackboard->SetValueAsObject(KeyMainCastle, InCastle);
	}
}

float AMyGameAIController::GetAttackRange() 
{
	if (AGameCharacterBase* CB = Cast<AGameCharacterBase>(GetPawn()))
	{
		if (UStatusComponent* StatusComp = CB->FindComponentByClass<UStatusComponent>())
		{
			float Radius = StatusComp->m_FinalStat.AttackRadius;
			return Radius;
		}
	}
	return 100.f;
}

AActor* AMyGameAIController::SelectBestTarget() const
{
	if (!PerceptionComp) return nullptr;

	APawn* SelfPawn = GetPawn();
	if (!SelfPawn) return nullptr;

	TArray<AActor*> SensedActors;
	PerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), SensedActors);
	float BestScore = -FLT_MAX;
	AActor* Best = nullptr;

	const FVector SelfLoc = SelfPawn->GetActorLocation();

	for (AActor* A : SensedActors)
	{
		if (!IsValid(A)) continue;
		if (GetTeamAttitudeTowards(*A) != ETeamAttitude::Hostile) continue;

		const float Dist = FVector::Dist(SelfLoc, A->GetActorLocation());
		float Score = 1.f / FMath::Max(Dist, 1.f);

		if (const APawn* P = Cast<APawn>(A))
		{
			if (P->IsPlayerControlled()) Score += 1.0f;
		}

		if (Score > BestScore) { BestScore = Score; Best = A; }
	}
	return Best;
}

void AMyGameAIController::BeginPlay()
{
	Super::BeginPlay();

}

