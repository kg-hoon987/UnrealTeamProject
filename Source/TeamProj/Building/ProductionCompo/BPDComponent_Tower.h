// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BuildingProductionComponent.h"
#include "BPDComponent_Tower.generated.h"

class ATower;

UCLASS()
class TEAMPROJ_API UBPDComponent_Tower : public UBuildingProductionComponent
{
	GENERATED_BODY()
	
private:

	UBPDComponent_Tower();

	int AttackDmg;

	float AttackSpd;
	
	float ShotDelay;

	float AttackRadius;

	bool bisRemoveDynamic;

	ATower* owner;

	TSubclassOf<class ASkillProjectile> Projectile;

protected:
	virtual void InitChild() override;

	virtual void ActivateEffect(const FBuildingEffect& Effect) override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
	void Shot();

public:
	AActor* target;
	
	UPROPERTY()
	TArray<AActor*> InRangeActors;

	UFUNCTION()
	void RemoveActorCuaseDied(AActor* act);

};
