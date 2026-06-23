// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Skill/SkillStruct.h"
#include "RoundStruct.h"
#include "StageStruct.generated.h"

class AGameCharacterBase;
class UStatusComponent;

UENUM(BlueprintType)
enum class EStageID : uint8
{
	None		 UMETA(DisplayName = "None"),
	Stage1		 UMETA(DisplayName = "Stage1"),
	Stage2		 UMETA(DisplayName = "Stage2"),
	Stage3		 UMETA(DisplayName = "Stage3")
};


/**
 * 
 */
USTRUCT(BlueprintType)
struct FCharInfo : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EClassType ClassType =EClassType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FClassConfig StatID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FSkillConfig> SkillID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 SkillLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UAnimInstance> AnimClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector CharScale;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) USkeletalMesh* Chest = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<USkeletalMesh*> Parts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* WeaponLeft= nullptr; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) UStaticMesh* WeaponRight = nullptr; 
	
	FCharInfo()
	{
		Parts.SetNum(8);
		CharScale = FVector(1.f, 1.f, 1.f);
	}
	
};



USTRUCT(BlueprintType)
struct FStage : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	EStageID EStageID;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector MaxMove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FVector MinMove;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	FName StageID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	TArray<FVector> PortalPos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	TArray<FRotator> PortalRot;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	TArray<FRound> Round;
};



