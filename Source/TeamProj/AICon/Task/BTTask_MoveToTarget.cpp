// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MoveToTarget.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Pawn.h"

UBTTask_MoveToTarget::UBTTask_MoveToTarget()
{
	NodeName = TEXT("Move To Target");
	BB_CurrentTarget.SelectedKeyName = "CurrentTarget";

	// 기본 태그 거리 값
	TagDesiredRange.Add("Player", 400.f);
	TagDesiredRange.Add("GreatSword", 200.f);
	TagDesiredRange.Add("Sword", 200.f);
	TagDesiredRange.Add("Sage", 500.f);
	TagDesiredRange.Add("Shield", 200.f);
	TagDesiredRange.Add("Bow", 700.f);
	TagDesiredRange.Add("Catapult", 700.f);
	TagDesiredRange.Add("Staff", 700.f);
	TagDesiredRange.Add("Throwing", 700.f);
	TagDesiredRange.Add("Destroyer", 200.f);
	TagDesiredRange.Add("Bomb", 100.f);
	TagDesiredRange.Add("Dagger", 100.f);
	TagDesiredRange.Add("Building", 400.f);
}

EBTNodeResult::Type UBTTask_MoveToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Self = AI ? AI->GetPawn() : nullptr;
	if (!AI || !Self) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(BB_CurrentTarget.SelectedKeyName));
	if (!Target) return EBTNodeResult::Failed;

	const FVector SelfLoc = Self->GetActorLocation();
	const FVector TargetLoc = Target->GetActorLocation();

	// 거리 결정
	const float DesiredRange = ResolveDesiredRange(Self);

	// 방향
	FVector Dir = (SelfLoc - TargetLoc);
	if (Dir.IsNearlyZero()) Dir = Self->GetActorForwardVector();
	Dir.Z = 0.f;
	Dir.Normalize();

	// 목표 위치
	FVector MovePos = TargetLoc + Dir * DesiredRange;
	MovePos.Z = SelfLoc.Z;

	// NavMesh에 투영
	if (UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(Self->GetWorld()))
	{
		FNavLocation Projected;
		if (Nav->ProjectPointToNavigation(MovePos, Projected))
		{
			MovePos = Projected.Location;
			MovePos.Z = SelfLoc.Z;
		}
	}

	// 단순 이동
	AI->MoveToLocation(MovePos, AcceptanceRadius, /*bStopOnOverlap*/false);

	return EBTNodeResult::Succeeded; // 비동기 추적 안 함 → 호출만 하고 바로 성공 처리
}

float UBTTask_MoveToTarget::ResolveDesiredRange(AActor* Target) const
{
	if (!Target) return DefaultDesiredRange;

	for (const TPair<FName, float>& Pair : TagDesiredRange)
	{
		if (Target->ActorHasTag(Pair.Key))
		{
			return Pair.Value;
		}
	}
	return DefaultDesiredRange;
}
