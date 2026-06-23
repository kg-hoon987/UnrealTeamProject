// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "../../Skill/SkillStruct.h"
#include "BTTask_UseBestSkill.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBTTask_UseBestSkill : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_UseBestSkill();

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_CurrentTarget;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_DesiredCastLocation;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_WantsSpatialSkill;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_LastSkillTried;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BB_SkillDesiredIndex;

	UPROPERTY(EditAnywhere, Category = "Skill")
	int32 MinSkillIndex = 1;
	UPROPERTY(EditAnywhere, Category = "Skill")
	int32 MaxSkillIndex = 2;

	UPROPERTY(EditAnywhere, Category = "Skill")
	float SpatialPickThreshold = 0.25f; 

	UPROPERTY(EditAnywhere, Category = "Skill")
	bool bAvoidRepeat = true;

	UPROPERTY(EditAnywhere, Category = "Skill")
	bool bAssumeSpatialIfTargetingClass = true;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	bool IsSpatialSkill(const struct FSkillData& D) const;
	float EstimateValueSimple(class APawn* Caster, class AActor* Target, const struct FSkillData& D) const;
	
};
