// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateSensedTargets.generated.h"


USTRUCT(BlueprintType)
struct FTagPriorityMap
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, float> Priority; // Boss=2.0, Healer=1.5 ...
};



UCLASS()
class TEAMPROJ_API UBTService_UpdateSensedTargets : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_UpdateSensedTargets();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard") FBlackboardKeySelector BB_CurrentTarget;
	UPROPERTY(EditAnywhere, Category = "Blackboard") FBlackboardKeySelector BB_InAttackRange;
	UPROPERTY(EditAnywhere, Category = "Blackboard") FBlackboardKeySelector BB_HasLoS;
	UPROPERTY(EditAnywhere, Category = "Blackboard") FBlackboardKeySelector BB_ThreatScore;
	UPROPERTY(EditAnywhere, Category = "Blackboard") FBlackboardKeySelector BB_DistanceToTarget;
	
	// 기본 사거리(태그 매칭 실패 시)
	UPROPERTY(EditAnywhere, Category = "Scoring") float DefaultAttackRange = 350.f;

	// 태그별 사거리(예: Melee:200, Ranger:800)
	UPROPERTY(EditAnywhere, Category = "Scoring")TMap<FName, float> TagAttackRange;

	// 태그/클래스 우선순위(예: Boss:2.0, Healer:1.5 ...)
	UPROPERTY(EditAnywhere, Category = "Scoring") FTagPriorityMap ClassPriority;

	UPROPERTY(EditAnywhere, Category = "Scoring") bool bUsePathCost = false;
	UPROPERTY(EditAnywhere, Category = "Scoring") float W_Class = 1.2f;
	UPROPERTY(EditAnywhere, Category = "Scoring") float W_Distance = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Scoring") float W_LoS = 1.0f;
	
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

public:
	float GetAttackRangeForTarget(AActor* Target) const;
	float ClassScore(const AActor* Target) const;
	bool  HasLoS(class AAIController* AI, AActor* Target) const;
	float NormInvDistance(class APawn* Self, AActor* Target) const;

	AActor* PickBest(APawn* Self, const TArray<AActor*>& Candidates, class AAIController* AI, float& OutScore) const;
	
};
