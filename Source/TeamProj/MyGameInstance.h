// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"
#define GameInst Cast<UMyGameInstance>(GetWorld()->GetGameInstance())

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	TObjectPtr<UDataTable> PlayerDataTable{};
	TSoftObjectPtr<UWorld> TargetLevel{};
	TSoftObjectPtr<UWorld> OldLevel{};
public:
	UMyGameInstance();
	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void OpenLevel(TSoftObjectPtr<UWorld> newLevel);  // 다른 레벨로 이동

	FName GetTargetLevelName()
	{
		return *TargetLevel.GetAssetName();
	}
};
