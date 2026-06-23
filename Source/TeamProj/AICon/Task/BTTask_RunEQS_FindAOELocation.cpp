// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RunEQS_FindAOELocation.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"

UBTTask_RunEQS_FindAOELocation::UBTTask_RunEQS_FindAOELocation()
{
	NodeName = TEXT("Run EQS : Find AOE Location");
	BB_DesiredCastLocation.SelectedKeyName = "DesiredCastLocation";
	BB_WantsSpatialSkill.SelectedKeyName = "WantsCastSpatialSkill";
	BB_EQSFailCount.SelectedKeyName = "EQSQueryFailCount";
}

EBTNodeResult::Type UBTTask_RunEQS_FindAOELocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!QueryTemplate) return EBTNodeResult::Failed;

	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Pawn = AI ? AI->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	// 이전 쿼리 바인딩 제거(안전)
	if (ActiveQuery)
	{
		ActiveQuery->GetOnQueryFinishedEvent().RemoveAll(this);
		ActiveQuery = nullptr;
	}

	auto* Wrapper = UEnvQueryManager::RunEQSQuery(Pawn->GetWorld(), QueryTemplate, Pawn, EEnvQueryRunMode::SingleResult, nullptr);
	if (!Wrapper) return EBTNodeResult::Failed;

	// 실행 당시의 BT 컴포넌트를 캐싱(비동기 콜백에서 사용)
	CachedBTC = &OwnerComp;

	ActiveQuery = Wrapper; // UPROPERTY로 잡아 가비지 방지
	// 시그니처에 맞춰 AddDynamic 사용 (추가 인자 X)
	ActiveQuery->GetOnQueryFinishedEvent().AddDynamic(this, &UBTTask_RunEQS_FindAOELocation::OnQueryFinished);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UBTTask_RunEQS_FindAOELocation::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ActiveQuery)
	{
		ActiveQuery->GetOnQueryFinishedEvent().RemoveAll(this);
		ActiveQuery = nullptr;
	}
	CachedBTC.Reset();
	return EBTNodeResult::Aborted;
}

void UBTTask_RunEQS_FindAOELocation::OnQueryFinished(UEnvQueryInstanceBlueprintWrapper* Wrapper, EEnvQueryStatus::Type Status)
{
	if (ActiveQuery)
	{
		ActiveQuery->GetOnQueryFinishedEvent().RemoveAll(this);
		ActiveQuery = nullptr;
	}

	if (!CachedBTC.IsValid())
	{
		return; // BT가 이미 파괴/종료됨
	}

	UBehaviorTreeComponent& OwnerComp = *CachedBTC.Get();
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Status == EEnvQueryStatus::Success)
	{
		TArray<FVector> Locs = Wrapper->GetResultsAsLocations();
		if (Locs.Num() > 0)
		{
			BB->SetValueAsVector(BB_DesiredCastLocation.SelectedKeyName, Locs[0]);
			BB->SetValueAsBool(BB_WantsSpatialSkill.SelectedKeyName, false);
			BB->SetValueAsInt(BB_EQSFailCount.SelectedKeyName, 0);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	// 실패 처리
	int32 FailCnt = BB->GetValueAsInt(BB_EQSFailCount.SelectedKeyName);
	FailCnt++;
	BB->SetValueAsInt(BB_EQSFailCount.SelectedKeyName, FailCnt);

	if (FailCnt >= MaxConsecutiveFail)
	{
		BB->SetValueAsBool(BB_WantsSpatialSkill.SelectedKeyName, false);
		BB->SetValueAsVector(BB_DesiredCastLocation.SelectedKeyName, FVector::ZeroVector);
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}
