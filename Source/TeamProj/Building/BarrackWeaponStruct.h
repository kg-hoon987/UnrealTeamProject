// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BarrackWeaponStruct.generated.h"

UENUM()
enum class EWeaponClass : uint8
{
	Worrior,
	Archer,
	Priest,
	Wizard
};

USTRUCT(BlueprintType)
struct FBarrackWeapon : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeaponClass WeaponClassId;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<FName> WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FString AssetDataPath;
};