// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIInfoBase.h"
#include "Components/TextBlock.h"
#include "../Building/BuildingBase.h"
#include "UIBuildingBase.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UUIBuildingBase : public UUIInfoBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	ABuildingBase* Building;
	/*
	UPROPERTY(EditAnywhere,meta= (BindWidget))
	UTextBlock* BuildingSimpleDesp{};
	UPROPERTY(EditAnywhere,meta= (BindWidget))
	UTextBlock* CurrentLevelInfo{};
	UPROPERTY(EditAnywhere,meta= (BindWidget))
	UTextBlock* NextLevelInfo{};
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* NextLevelCost{};

	*/


public:
	void SetBuilding(ABuildingBase* pBuilding);

	UFUNCTION(BlueprintCallable)
	ABuildingBase* GetBuilding() { return Building; }
	
};
