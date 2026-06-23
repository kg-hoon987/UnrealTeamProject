// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "BTTask_RunEQS_FindAOELocation.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBTTask_RunEQS_FindAOELocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_RunEQS_FindAOELocation();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_DesiredCastLocation;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_WantsSpatialSkill;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_EQSFailCount;

	// 에셋으로 만든 EQS 쿼리 (Grid + Overlap Tests 등)
	UPROPERTY(EditAnywhere, Category = "EQS")
	class UEnvQuery* QueryTemplate;

	// 실패 폴백 처리
	UPROPERTY(EditAnywhere, Category = "EQS")
	int32 MaxConsecutiveFail = 2;

	// 실행 중 참조 유지(가비지 방지 및 RemoveAll을 위해)
	UPROPERTY() TObjectPtr<class UEnvQueryInstanceBlueprintWrapper> ActiveQuery = nullptr;

	// 실행 당시의 BT 컴포넌트를 기억(비동기 콜백에서 FinishLatentTask에 필요)
	TWeakObjectPtr<class UBehaviorTreeComponent> CachedBTC;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void OnQueryFinished(class UEnvQueryInstanceBlueprintWrapper* Wrapper, EEnvQueryStatus::Type Status);
	
};
