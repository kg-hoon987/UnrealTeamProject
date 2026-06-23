// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BuildingProductionComponent.h"
#include "BPDComponent_Statue.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBPDComponent_Statue : public UBuildingProductionComponent
{
	GENERATED_BODY()
private:
	UBPDComponent_Statue();
	int32 makeExp;
protected:
	virtual void OnBinding() override;

	UFUNCTION()
	virtual void HandleOnActivate() override;

	virtual void ActivateEffect(const FBuildingEffect& Effect) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
