// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BuildingProductionComponent.h"
#include "../../Stage/StageStruct.h"
#include "BPDComponent_Barrack.generated.h"

enum class EIdentity : uint8;
class AAllyCharacter;
class URoundComponent;

USTRUCT()
struct FUnitSlot
{
	GENERATED_BODY()

	UPROPERTY()
	AAllyCharacter* Unit = nullptr;   // 현재 유닛

	UPROPERTY()
	float RespawnTime = 0.f;   // 리스폰 남은 시간

	bool isDead = false;
};

UCLASS()
class TEAMPROJ_API UBPDComponent_Barrack : public UBuildingProductionComponent
{
	GENERATED_BODY()
	
private:
	UBPDComponent_Barrack();

	int AllyCount;

	float Regentime;

protected:
	virtual void InitChild() override;

	virtual void BeginPlay() override;

	virtual void ActivateEffect(const FBuildingEffect& Effect) override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	int32 ClassId;

private:

	
	TArray<FCharInfo*> IdenList;
	
	FCharInfo CurSpawnData;

	int SpawnCount;

	TSubclassOf<class AAllyCharacter> AllyBP;

	TArray<FVector> SpawnPos;

	TArray<FUnitSlot> SpawnedList;

	URoundComponent* Roundcom;


	void SpawnAlly(int index = 0);

	UFUNCTION()
	void OnAllyDied(AActor* Ally);
public:
	UDataTable* CharInfoTable = nullptr; //데이터테이블
	
	void SetClassId(int32 id);

	void InitLRCharacter(EIdentity EIden);
	
	void SetAllyIdentity(EIdentity EIden);

	void End();
};
