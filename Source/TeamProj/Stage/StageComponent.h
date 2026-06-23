// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StageStruct.h"
#include "StageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJ_API UStageComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStageComponent();

	UPROPERTY()
	int32 CurRound;

	UPROPERTY()
	int32 MaxRound;


	UPROPERTY(BlueprintReadWrite, Category = "Stage")
	FCharInfo PlayerInfo{};
	UPROPERTY(BlueprintReadWrite, Category = "Stage")
	UDataTable* StageTable{};
	UPROPERTY(BlueprintReadWrite, Category = "Stage")
	TArray<FVector> PortalPos;
	UPROPERTY(BlueprintReadWrite, Category = "Stage")
	TArray<FRound> Round;

	FStage* StageInfo{};

	//void GetRowData();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Init(EStageID StageId);

	UFUNCTION(BlueprintCallable)
	void StartStage();

	UFUNCTION(BlueprintCallable)
	void SelectWeapon();

	UFUNCTION(BlueprintCallable)
	void SetRound();

	UFUNCTION(BlueprintCallable)
	void EndStage();
};
