// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MoveToTarget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBTTask_MoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_MoveToTarget();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_CurrentTarget;

	/** 태그→거리 매핑 */
	UPROPERTY(EditAnywhere, Category = "Gap")
	TMap<FName, float> TagDesiredRange;

	/** 기본 거리 (태그 매칭 실패 시) */
	UPROPERTY(EditAnywhere, Category = "Gap", meta = (ClampMin = "0"))
	float DefaultDesiredRange = 200.f;

	/** 목표 도착 허용 반경 */
	UPROPERTY(EditAnywhere, Category = "Move")
	float AcceptanceRadius = 50.f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	float ResolveDesiredRange(AActor* Target) const;

};
