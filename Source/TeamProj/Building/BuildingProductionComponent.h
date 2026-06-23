// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildingStruct.h"
#include "BuildingStateComponent.h"
#include "BuildingProductionComponent.generated.h"

class ABuildingBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJ_API UBuildingProductionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuildingProductionComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentLevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBuildingEffect> Effects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsBomb = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int ExplosionDmg;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ExplosionRange;

	UFUNCTION(BlueprintCallable)
	void InitBuildingEffect(const FBuildingLevelData& Row);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<EBuildingEffectType> OwnEffect;

	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void InitChild() {};
	virtual void OnBinding() {};

	bool CheckEffect(FBuildingEffect ef);

	virtual void HandleOnActivate() {};

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void OnActivate(const FBuildingEffect& effect);
	void OnDeActivate(const FBuildingEffect& effect);

	void OnExplosion();
	
	virtual void RegisterEffect(const FBuildingEffect& effect) PURE_VIRTUAL(UBuildingProductionComponent::RegisterEffect,);
	virtual void UnRegisterEffect(const FBuildingEffect& effect)PURE_VIRTUAL(UBuildingProductionComponent::UNRegisterEffect, );

	virtual void ActivateEffect(const FBuildingEffect& effect)PURE_VIRTUAL(UBuildingProductionComponent::ActivateEffect, );
	virtual void DeActivateEffect(const FBuildingEffect& effect)PURE_VIRTUAL(UBuildingProductionComponent::DeActivateEffect, );
		
};
