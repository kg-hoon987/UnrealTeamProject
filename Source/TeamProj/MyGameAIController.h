// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GenericTeamAgentInterface.h"
#include "AICon/GameUnitBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Sight.h"
#include "MyGameAIController.generated.h"

/**
 * 
 */

//일단 타입분류만들어놓으
UENUM()
enum class EAIType : uint8
{
	None,
	Melee,
	Ranged
};

UCLASS()
class TEAMPROJ_API AMyGameAIController : public AAIController
{
	GENERATED_BODY()
	
private:
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	void InitBlackboardAndBT();
	
protected:
	TObjectPtr<class UBehaviorTree> BTData;
	TObjectPtr<class UBlackboardData> BBData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY() TObjectPtr<UAISenseConfig_Sight> SightConfig = nullptr;
	UPROPERTY() TObjectPtr<UAISenseConfig_Damage> DamageConfig = nullptr;

	EAIType AIType = EAIType::None;

	virtual void OnPossess(APawn* InPawn) override;
	
public:
	AMyGameAIController();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category = "Team")
	ETeam Team = ETeam::None;

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		return FGenericTeamId(static_cast<uint8>(Team));
	}

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override
	{
		Team = static_cast<ETeam>(InTeamID.GetId());
	}

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	UFUNCTION(BlueprintCallable)
	void SetMainCastle(AActor* InCastle);

	UFUNCTION(BlueprintCallable)
	float GetAttackRange();

	UFUNCTION(BlueprintCallable)
	AActor* SelectBestTarget() const;
	
	FORCEINLINE UAIPerceptionComponent* GetPerceptionComp() const { return PerceptionComp;  }

protected:
	virtual void BeginPlay();

	UPROPERTY(EditDefaultsOnly, Category = "BBKeys")
	FName KeyTargetActor = TEXT("TargetActor");
	UPROPERTY(EditDefaultsOnly, Category = "BBKeys")
	FName KeyMainCastle = TEXT("MainCastle");
	UPROPERTY(EditDefaultsOnly, Category = "BBKeys")
	FName KeyHasLOS = TEXT("HasLOS");
	UPROPERTY(EditDefaultsOnly, Category = "BBKeys")
	FName KeyInAttackRange = TEXT("InAttackRange");



};
