// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../AICon/GameUnitBase.h"
#include "../Status/StatusStruct.h"
#include "Components/SplineComponent.h"
#include "BuildingStruct.generated.h"

/**
 * 
 */
UENUM()
enum class EBuildingEffectType : uint8
{
	None,
	Main_AddStat,     //FStatModifier 
	Inn_Gold, //골드수급량 int32
	Statue_Exp, //경험치 보정 비율 int32
	DragonEggPerLevel, //드래곤알레벨 int32
	Laboratory_HPRegenPerSec, //틱 회복 flaot
	Barrack_SpawnAlly, //병사 수 증가, 리젠 감소 float
	Tower_Stat, //타워 공격력 ,공격속도, 공격범위 float
	Main_AddReRoll,     //리롤권 int32
	Bomber_Explosion
};
USTRUCT(BlueprintType)
struct FSplinePointData
{
	GENERATED_BODY()

	UPROPERTY() FVector Location;
	UPROPERTY() FRotator Rotation;
	UPROPERTY() FVector Scale;
	UPROPERTY() FVector ArriveTangent;
	UPROPERTY() FVector LeaveTangent;
	UPROPERTY() TEnumAsByte<ESplinePointType::Type> PointType;
};

USTRUCT(BlueprintType)
struct FWallDoorManager
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 DoorID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FSplinePointData> points;
};

USTRUCT(BlueprintType)
struct FBuildingStateInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<AActor> BuildingClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Level;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector Location;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isManager = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FWallDoorManager WallDoorManager;

};

USTRUCT(BlueprintType)
struct FBuildingEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) EBuildingEffectType Type = EBuildingEffectType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32   intValue;				// 아웃풋 밸류들
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float   floatValue_1;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float   floatValue_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float   floatValue_3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool isGlobal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<ETeam> targetTeams;			// 효과 적용 대상 팀들
};

USTRUCT(BlueprintType)
struct FBuildingLevelData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString meshRef;					// 메쉬 레퍼런스
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ovelapOffset;					// 오버랩 가능 오프셋 크기
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FBaseStat BuildingStat;		

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 UpgradeCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FBuildingEffect> buildingEffects;
};
