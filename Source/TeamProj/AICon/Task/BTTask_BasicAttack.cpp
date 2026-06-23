// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_BasicAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../../Skill/SkillComponent.h"

UBTTask_BasicAttack::UBTTask_BasicAttack()
{
	NodeName = TEXT("BasicAttack");
	BB_CurrentTarget.SelectedKeyName = "CurrentTarget";
}

EBTNodeResult::Type UBTTask_BasicAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Pawn = AI ? AI->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(BB_CurrentTarget.SelectedKeyName)) : nullptr;

	if (USkillComponent* SC = Pawn->FindComponentByClass<USkillComponent>())
	{
		return SC->TryUseSKill(0, Target, FVector::ZeroVector) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}
	return EBTNodeResult::Failed;
}
