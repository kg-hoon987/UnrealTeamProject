// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Building/BuildingBase.h"
#include "BuildingHealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBuildingHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	ABuildingBase* Building;

public:
	void SetBuilding(ABuildingBase* pBuilding);

	UFUNCTION(BlueprintCallable)
	ABuildingBase* GetBuilding() { return Building; }
	
};
