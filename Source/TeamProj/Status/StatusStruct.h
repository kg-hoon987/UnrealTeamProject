// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StatusStruct.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EClassType : uint8
{
    None             UMETA(DisplayName = "None"),
    Player           UMETA(DisplayName = "Player"),
    Ally             UMETA(DisplayName = "ally"), //플레이어, 아군, 건물
    Enemy            UMETA(DisplayName = "Enemy") //적군, 보스
};


USTRUCT(BlueprintType)
struct FClassConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ClassType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
    int32 Level = 1;
};



UENUM(BlueprintType)
enum class EWeaponType : uint8
{   //Common Weapon
    None             UMETA(DistplayName = "None"),
    GreatSword       UMETA(DistplayName = "GreatSword"),
    Shield           UMETA(DistplayName = "Shield"),
    Staff            UMETA(DistplayName = "Staff"),
    Throwing         UMETA(DistplayName = "Throwing"),
    Bow              UMETA(DistplayName = "Bow"),
    catapult         UMETA(DistplayName = "catapult"),
    //OnlyTeam Weapon
    Sword            UMETA(DistplayName = "Sword"),
    Sage            UMETA(DistplayName = "Sage"),
    //OnlyEnemy Weapon
    Fly              UMETA(DistplayName = "Fly"),
    Dagger           UMETA(DistplayName = "Dagger"),
    Gantlet          UMETA(DistplayName = "Gantlet"),
    Bomb             UMETA(DistplayName = "Bomb"),
    Destroyer        UMETA(DistplayName = "Destroyer")
};



USTRUCT(BlueprintType)
struct FStatModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddHP = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddAtk = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddDef = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddAtkSpd = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddCritR = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddCritD = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddAtkRadius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddCoolRec = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float AddSpd = 0.f;

    bool operator==(const FStatModifier& Other) const
    {
        return
            FMath::IsNearlyEqual(AddHP, Other.AddHP) && FMath::IsNearlyEqual(AddAtk, Other.AddAtk) && FMath::IsNearlyEqual(AddDef, Other.AddDef) && 
            FMath::IsNearlyEqual(AddAtkSpd, Other.AddAtkSpd) && FMath::IsNearlyEqual(AddCritR, Other.AddCritR) && FMath::IsNearlyEqual(AddCritD, Other.AddCritD) &&
            FMath::IsNearlyEqual(AddAtkRadius, Other.AddAtkRadius) && FMath::IsNearlyEqual(AddCoolRec, Other.AddCoolRec) && FMath::IsNearlyEqual(AddSpd, Other.AddSpd);
    }
};

USTRUCT(BlueprintType)
struct FBaseStat : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName         ClassID = NAME_None; //SLime_Hand 그만큼 기본스탯을 올려주자 대신 순수스탯좀 높아질수도있다
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName         WeaponID = NAME_None; //SLime_Hand 그만큼 기본스탯을 올려주자 대신 순수스탯좀 높아질수도있다
    UPROPERTY(EditAnywhere, BlueprintReadWrite) EWeaponType   WeaponType = EWeaponType::None; //SLime_Hand 그만큼 기본스탯을 올려주자 대신 순수스탯좀 높아질수도있다
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32         Level = 1; 
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         MaxHP = 0.f; 
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         Attack = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         Defense = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         AttackSpeed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         CritRate = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         CritDamage = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         AttackRadius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         CooldownReduction = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         Speed = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32         ReqEXPToNextLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool          IsDead = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FStatModifier WeaponStat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32         GetEXP = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32         GetMoney = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         DropBoxRate = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float         MaxGroggy = 0.f;
};



USTRUCT(BlueprintType)
struct FCurrentStat
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32   CntLevel = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float   CntHP = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32   CntEXP = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float   Groggy = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool    bDash = true;
};

