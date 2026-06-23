// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoundStruct.h"
#include "StageStruct.h"
#include "Portal.generated.h"

class AEnemyCharacter;

USTRUCT(BlueprintType)
struct FPortalSpawnInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	FSpawnEntry Entry;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FCharInfo CharInfo;
};

UCLASS()
class TEAMPROJ_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	APortal();


protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
	UDataTable* CharInfoTable = nullptr; //데이터테이블

	UPROPERTY(BlueprintReadOnly, Category = "Spawn")
	TArray<FPortalSpawnInfo> SpawnPlan; //라운드에서 받아온 몹인포 적용
	UPROPERTY(BlueprintReadOnly, Category = "Spawn")
	TArray<float> GenTickPlan;
	UPROPERTY(BlueprintReadOnly, Category = "Spawn")
	TArray<int32> GenMocCntPlan;

	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<class AEnemyCharacter> EnemyBP;
	bool isReady;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void BuildSpawnPlanFromIDs(TArray<FSpawnEntry> pptData);

	void SpawnMob(int32 MobIndex,float DeltaTime);

	UFUNCTION()
	void ReadySpawn();


};
