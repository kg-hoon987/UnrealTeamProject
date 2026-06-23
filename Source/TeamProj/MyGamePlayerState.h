// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Building/BuildingStateComponent.h"
#include "MyGamePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AMyGamePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMyGamePlayerState();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStageComponent* StageCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URoundComponent* RoundCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBuildingStateComponent* BuildingCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPerkComponent* PerkComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 Gold;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Exp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Reroll;

	UPROPERTY(BlueprintReadOnly)
	FVector RespawnPos{};

protected:
	virtual void BeginPlay() override;
	
public:
	FVector RespawnLocation() { return BuildingCom->MainCastle->GetActorLocation(); }
};
