// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Skill/SkillStruct.h"
#include "RoundStruct.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpawnEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	FName MobID = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	int32 MobCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	float GenTick = 0.f;
	};

USTRUCT(BlueprintType)
struct FSpawnPortal
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	FName PortalID = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	TArray<FSpawnEntry> MobInfo;
	};



USTRUCT(BlueprintType)
struct FRound
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)	FName RoundID = NAME_None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)	TArray<FSpawnPortal> PortalInfo;

};