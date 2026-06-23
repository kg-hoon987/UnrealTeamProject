// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Status/StatusStruct.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "TeamFilterUtil.h"
#include "UObject/NoExportTypes.h"
#include "SkillStruct.generated.h"

class USkillTriggerStrategy;
class UTargetingTask;
class USkillEffect;
class ASkillProjectile;


/**
 * 
 */
USTRUCT(BlueprintType)
struct FSkillConfig
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ClassType = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName WeaponType= NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Count = 0;
};

UENUM(BlueprintType)
enum class ESkillBuffType : uint8
{
    None             UMETA(DisplayName = "None"),
    Buff             UMETA(DisplayName = "Buff"),
    DeBuff           UMETA(DisplayName = "DeBuff")
};



UENUM(BlueprintType)
enum class ESkillEffectType : uint8
{
    None             UMETA(DisplayName = "None"),
    Hurt             UMETA(DisplayName = "Hurt"),
    Poison           UMETA(DisplayName = "Posion"),
    Fire             UMETA(DisplayName = "Fire"),
    Iced             UMETA(DisplayName = "Iced"),
    TickHeal         UMETA(DisplayName = "TickHeal")
};

UENUM(BlueprintType)
enum class ESkillCCType : uint8
{
    None             UMETA(DisplayName = "None"),
    Stun             UMETA(DisplayName = "Stun"),
    Invincible       UMETA(DisplayName = "Invincible"),
    KnockBack        UMETA(DisplayName = "KnockBack"),
    KnockFront       UMETA(DisplayName = "KnockFront"),
    Provoke          UMETA(DisplayName = "Provoke"),
    SuperArmor       UMETA(DisplayName = "SuperArmor")
};

USTRUCT(BlueprintType)
struct FSkillBuffInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName            SkillBuffName = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ESkillBuffType   BuffType = ESkillBuffType::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D*      Icon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FStatModifier    BuffStat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            Duration = 0.f;

    bool operator==(const FSkillBuffInfo& Other) const
    {
        return SkillBuffName == Other.SkillBuffName &&
            Icon == Other.Icon&&
            FMath::IsNearlyEqual(Duration, Other.Duration) &&
            BuffStat == Other.BuffStat &&
            BuffType == Other.BuffType;
    }

};


USTRUCT(BlueprintType)
struct FSkillEffectInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName            SkillEffectName = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ESkillEffectType EffectType = ESkillEffectType::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D*      Icon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32            MaxStack = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            Duration = 0.f;
    
    bool operator==(const FSkillEffectInfo& Other) const
    {
        return SkillEffectName == Other.SkillEffectName &&
            Icon == Other.Icon &&
            FMath::IsNearlyEqual(Duration, Other.Duration) &&
            MaxStack == Other.MaxStack &&
            EffectType == Other.EffectType;
    }

};


USTRUCT(BlueprintType)
struct FSkillCCInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite) FName            SkillCCName = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite) ESkillCCType     EffectType = ESkillCCType::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) UTexture2D*      Icon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) int32            MaxStack = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            Duration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            KnockDistance = 0.f;
    
    
    bool operator==(const FSkillCCInfo& Other) const
    {
        return SkillCCName == Other.SkillCCName &&
            FMath::IsNearlyEqual(Duration, Other.Duration) &&
            Icon == Other.Icon &&
            FMath::IsNearlyEqual(KnockDistance, Other.KnockDistance) &&
            MaxStack == Other.MaxStack &&
            EffectType == Other.EffectType;
    }

};

USTRUCT(BlueprintType)
struct FSkillLevelData : public FTableRowBase
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            AddDamage = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            CoolRec = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            AddDuration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            CastingTimeRec = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            AddHalfHeight = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            AddRange = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            AddRadius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float            AddConeHalfAngle = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FSkillBuffInfo   BuffStat;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FSkillEffectInfo ExtraEffect;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) FSkillCCInfo     ExtraCC;
    
    bool operator==(const FSkillLevelData& Other) const
    {
        return 
            FMath::IsNearlyEqual(AddDamage, Other.AddDamage) &&
            FMath::IsNearlyEqual(CoolRec, Other.CoolRec) &&
            FMath::IsNearlyEqual(AddDuration, Other.AddDuration) &&
            FMath::IsNearlyEqual(CastingTimeRec, Other.CastingTimeRec) &&
            FMath::IsNearlyEqual(AddHalfHeight, Other.AddHalfHeight) &&
            FMath::IsNearlyEqual(AddRadius, Other.AddRadius) &&
            FMath::IsNearlyEqual(AddConeHalfAngle, Other.AddConeHalfAngle) &&
            BuffStat == Other.BuffStat &&
            ExtraEffect == Other.ExtraEffect &&
            ExtraCC == Other.ExtraCC;
    }
};


USTRUCT(BlueprintType)
struct FProjectileInfo
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite) TSubclassOf<ASkillProjectile> ProjectileClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ProjectileSpeed = 1500.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ProjectileRadius = 10.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bProjectileHoming = false;
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float HomingAccel = 20000.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSpawnFromSky = false;     //메테오 방식?
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float SkyHeight = 1200.f;       //떨어질 높이
    UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bExplodeOnGround = true;   //바닥에서 폭발?
    UPROPERTY(EditAnywhere, BlueprintReadWrite) float ExplosionRadius = 300.f;   //폭발 범위
};


USTRUCT(BlueprintType)
struct FSkillData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FName                               SkillID = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FName                               WeaponType = TEXT("");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FString                             SkillName = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FString                             Description = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     UTexture2D*                         Icon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     UAnimMontage*                       Animation = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     USoundBase*                         Sound = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               HalfHeight = 0.f;   //높이
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Range = 0.f;        //거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Radius = 0.f;       //구체 타겟 반지름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               ConeHalfAngle = 0.f;//전방 각도
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Cooldown = 0.f;     
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               CastingTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Duration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FProjectileInfo                     ProjectileInfo;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TArray<FSkillLevelData>             SkillLevels;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     ETargetAffiliation                  TargetAffiliation = ETargetAffiliation::HostileOnly;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TSubclassOf<USkillTriggerStrategy>  TriggerClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TSubclassOf<UTargetingTask >        TargetingClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TArray<TSubclassOf<USkillEffect>>   Effects;
};


USTRUCT(BlueprintType)
struct FSkillContext //스킬 트리거 
{
    GENERATED_BODY()

    AActor* Caster = nullptr; //스킬 시전자
    AActor* OptionalTarget = nullptr; //단일타겟 + 조준점..???????
    FVector OptionalLocation = FVector::ZeroVector; //타겟 지정 위칭
    int32 SkillLevel = 0;
    FSkillData* Data = nullptr;
};



/////////////////////////////CC////////////////////////////

USTRUCT(BlueprintType)
struct FActiveBuff
{
    GENERATED_BODY()

    UPROPERTY()
    FSkillBuffInfo BuffInfo;
    
    UPROPERTY()
    UTexture2D* Icon = nullptr;

    UPROPERTY()
    float RemainingDuration = 0.f;


    FActiveBuff() = default;

    FActiveBuff(const FSkillBuffInfo& InInfo, float InDuration)
        : BuffInfo(InInfo), RemainingDuration(InDuration) {}
};



USTRUCT(BlueprintType)
struct FActiveStatusEffect
{
    GENERATED_BODY()

    UPROPERTY()
    FSkillEffectInfo EffectData;

    UPROPERTY()
    UTexture2D* Icon;
    
    UPROPERTY()
    float RemainingDuration;

    UPROPERTY()
    int32 CurrentStack;

    UPROPERTY()
    float TickDmgTime;

    FActiveStatusEffect()
        : Icon(nullptr), RemainingDuration(0.f), CurrentStack(1), TickDmgTime(0.f)
    {}

    FActiveStatusEffect(const FSkillEffectInfo& InEffectData, float InDuration)
        : EffectData(InEffectData), RemainingDuration(InDuration), CurrentStack(1), TickDmgTime(0.f)
    {}
};




///////////////////////////Object//////////////////////////////



UCLASS(Blueprintable)
class TEAMPROJ_API USkill : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere,BlueprintReadOnly)
    FString m_SkillName;
    UPROPERTY(EditAnywhere,BlueprintReadOnly)
    int32 m_CurrentLevel;
    UPROPERTY(EditAnywhere,BlueprintReadOnly)
    FSkillData m_SkillData;
    UPROPERTY(EditAnywhere,BlueprintReadOnly)
    float m_CoolRemaining;



public:
    USkill();
    void FireOnce_Internal(const FSkillContext& TriggeredCtx);
    UFUNCTION(BlueprintCallable)
    int32 GetSkillLevel() { return m_CurrentLevel; }
    UFUNCTION(BlueprintCallable)
    FSkillData GetSkillData() const { return m_SkillData; }
    FSkillData* GetSkillDataPtr() { return &m_SkillData; }

    UFUNCTION(BlueprintCallable)
    const FSkillLevelData& GetSkillLevelData() const;


    UFUNCTION(BlueprintCallable)
    void SetSkillData(const FSkillData& pData);
    UFUNCTION(BlueprintCallable)
    void SetSkillLevel(int32 nLevel);



    UFUNCTION(BlueprintCallable)
    bool SkillLevelUp();
 
    UFUNCTION(BlueprintCallable)
    bool ExecuteSkill(AActor* Caster, AActor* OptionalTarget = nullptr, const FVector& OptionalLocation = FVector::ZeroVector);

    
    UFUNCTION(BlueprintCallable)
    bool CoolStart();
    UFUNCTION(BlueprintCallable)
    void CoolDown(float DeltaTime);
    UFUNCTION(BlueprintCallable)
    bool IsReady() const { return m_CoolRemaining<=0.f; }

    UFUNCTION()
    float GetCurrentSkillRadius();
};
