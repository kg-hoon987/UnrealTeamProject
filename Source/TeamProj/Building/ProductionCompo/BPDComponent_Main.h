// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BuildingProductionComponent.h"
#include "BPDComponent_Main.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBPDComponent_Main : public UBuildingProductionComponent
{
	GENERATED_BODY()
private:
	UBPDComponent_Main();

	int32 AddReroll;

protected:
	virtual void InitChild() override ;

	virtual void ActivateEffect(const FBuildingEffect& Effect) override;

	virtual void RegisterEffect(const FBuildingEffect& Effect) override;

	virtual void UnRegisterEffect(const FBuildingEffect& Effect) override;
};
