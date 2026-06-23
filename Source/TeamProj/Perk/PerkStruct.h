// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/NoExportTypes.h"
#include "../Status/StatusComponent.h"
#include "../Skill/SkillStruct.h"
#include "../Item/ItemStruct.h"
#include "PerkStruct.generated.h"

UENUM(BlueprintType)
enum class EPerkType : uint8
{
    None            UMETA(DisplayName = "None"),
    Player          UMETA(DisplayName = "Player"),
    Item            UMETA(DisplayName = "Item"),
    Skill           UMETA(DisplayName = "Skill"),
    Stat            UMETA(DisplayName = "Stat"),
    RoundBonus      UMETA(DisplayName = "Round Bonus"),
};

UENUM(BlueprintType)
enum class EPlayerEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    GoldIncrease    UMETA(DisplayName = "Gold Increase"),  //Value
    ExpIncrease     UMETA(DisplayName = "Exp Increase"),  //Value
};

UENUM(BlueprintType)
enum class EItemEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    ItemQuantity    UMETA(DisplayName = "Item Quantity"),  //ItemID, Value
    ItemBoost       UMETA(DisplayName = "Item Boost"),  //ItemID, ItemDataOverride, RoundDuration (ItemID, 현재 수량, 최대 수량 보존)
    AllItemsQuantity UMETA(DisplayName = "All Items Quantity"),  //Value
};

UENUM(BlueprintType)
enum class EPerkSkillEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    SkillBoost      UMETA(DisplayName = "Skill Boost"),  //SkillID, SkillDataOverride, RoundDuration (SkillID 보존)
};

UENUM(BlueprintType)
enum class EStatEffectType : uint8
{
    None            UMETA(DisplayName = "None"),
    PlayerStat      UMETA(DisplayName = "Player Stat"),  //StatModifier, RoundDuration
    AllyStat        UMETA(DisplayName = "Ally Stat"),  //StatModifier, RoundDuration
    EnemyStat       UMETA(DisplayName = "Enemy Stat"),  //StatModifier, RoundDuration
    AllyUnitStat    UMETA(DisplayName = "Ally Unit Stat"),  //StatModifier, RoundDuration, TargetWeaponType
    EnemyUnitStat   UMETA(DisplayName = "Enemy Unit Stat"),  //StatModifier, RoundDuration, TargetWeaponType
};

UENUM(BlueprintType)
enum class ERoundBonusEffectType : uint8
{
    None                    UMETA(DisplayName = "None"),
    RoundGoldIncrease       UMETA(DisplayName = "Round Gold Increase"),  //Value, RoundDuration
    RoundExpIncrease        UMETA(DisplayName = "Round Exp Increase"),  //Value, RoundDuration
    RoundItemQuantity       UMETA(DisplayName = "Round Item Quantity"),  //ItemID, Value, RoundDuration
    RoundAllItemsQuantity   UMETA(DisplayName = "Round All Items Quantity"),  //Value, RoundDuration
};

UENUM(BlueprintType)
enum class EPerkRarity : uint8
{
    Common          UMETA(DisplayName = "Common"),
    Rare            UMETA(DisplayName = "Rare"),
    Epic            UMETA(DisplayName = "Epic"),
    Legendary       UMETA(DisplayName = "Legendary")
};

USTRUCT(BlueprintType)
struct FPerkData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PerkID = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PerkName = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPerkRarity Rarity = EPerkRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPerkType PerkType = EPerkType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Value = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName SkillID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemEffectType ItemEffectType = EItemEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPerkSkillEffectType SkillEffectType = EPerkSkillEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStatEffectType StatEffectType = EStatEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPlayerEffectType PlayerEffectType = EPlayerEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ERoundBonusEffectType RoundEffectType = ERoundBonusEffectType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FItemData ItemDataOverride;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FSkillData SkillDataOverride;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RoundDuration = 0; // 0이면 영구 지속

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FStatModifier StatModifier;

    // 특정 무기 타입 타겟팅 (UnitStat용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EWeaponType TargetWeaponType = EWeaponType::None;

    bool operator==(const FPerkData& Other) const
    {
        return PerkID == Other.PerkID &&
               PerkName == Other.PerkName &&
               Description == Other.Description &&
               Icon == Other.Icon &&
               Rarity == Other.Rarity &&
               PerkType == Other.PerkType &&
               Value == Other.Value &&
               ItemID == Other.ItemID &&
               ItemEffectType == Other.ItemEffectType &&
               SkillEffectType == Other.SkillEffectType &&
               StatEffectType == Other.StatEffectType &&
               PlayerEffectType == Other.PlayerEffectType &&
               RoundEffectType == Other.RoundEffectType &&
               ItemDataOverride.ItemID == Other.ItemDataOverride.ItemID &&
               SkillDataOverride.SkillID == Other.SkillDataOverride.SkillID &&
               RoundDuration == Other.RoundDuration &&
               StatModifier == Other.StatModifier &&
               TargetWeaponType == Other.TargetWeaponType;
    }
};


// 라운드 기반 지속시간을 위한 구조체
USTRUCT(BlueprintType)
struct FRoundBasedEffect
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FPerkData PerkData;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RemainingRounds = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ActivationRound = 0;

    FRoundBasedEffect()
    {
        RemainingRounds = 0;
        ActivationRound = 0;
    }

    FRoundBasedEffect(const FPerkData& InPerkData, int32 InActivationRound)
    {
        PerkData = InPerkData;
        // RoundDuration이 0이면 영구 지속, 아니면 +1을 적용하여 실제 의도한 라운드 수만큼 효과가 지속되도록 함
        if (InPerkData.RoundDuration == 0)
        {
            RemainingRounds = 0; // 영구 지속
        }
        else
        {
            RemainingRounds = InPerkData.RoundDuration + 1;
        }
        ActivationRound = InActivationRound;
    }

    bool operator==(const FRoundBasedEffect& Other) const
    {
        return PerkData == Other.PerkData && 
               RemainingRounds == Other.RemainingRounds && 
               ActivationRound == Other.ActivationRound;
    }
};

// TMap에서 사용하기 위한 스탯 모디파이어 배열 구조체
USTRUCT(BlueprintType)
struct FStatModifierArray
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FStatModifier> Modifiers;

    FStatModifierArray() = default;
    
    FStatModifierArray(const TArray<FStatModifier>& InModifiers)
        : Modifiers(InModifiers)
    {
    }
};

// UI 표시용 퍼크 정보 구조체
USTRUCT(BlueprintType)
struct FPerkDisplayInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString PerkName = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RemainingRounds = -1; // -1이면 영구 지속

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPerkRarity Rarity = EPerkRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EPerkType PerkType = EPerkType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName PerkID = NAME_None;

    FPerkDisplayInfo() = default;
    
    FPerkDisplayInfo(const FPerkData& PerkData, int32 InRemainingRounds = -1)
        : Icon(PerkData.Icon)
        , PerkName(PerkData.PerkName)
        , Description(PerkData.Description)
        , RemainingRounds(InRemainingRounds)
        , Rarity(PerkData.Rarity)
        , PerkType(PerkData.PerkType)
        , PerkID(PerkData.PerkID)
    {
    }
};

///////////////////////////Object//////////////////////////////

// Item 효과 전략 인터페이스
UCLASS(Blueprintable, Abstract)
class TEAMPROJ_API UItemEffectStrategy : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ApplyEffect(const FPerkData& PerkData, UWorld* World);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool RemoveEffect(const FPerkData& PerkData, UWorld* World);
};

// 아이템 수량 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UItemQuantityEffect : public UItemEffectStrategy
{
    GENERATED_BODY()

public:
    virtual bool ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 아이템 부스트 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UItemBoostEffect : public UItemEffectStrategy
{
    GENERATED_BODY()

private:
    // 원본 아이템 데이터 저장 (복원용)
    UPROPERTY()
    FItemData m_OriginalItemData;

    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    UItemBoostEffect();
    
    virtual bool ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
    
    // PerkData 설정 함수 (지연 활성화용)
    UFUNCTION(BlueprintCallable)
    void SetPerkData(const FPerkData& PerkData);
};

// 전체 아이템 수량 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UAllItemsQuantityEffect : public UItemEffectStrategy
{
    GENERATED_BODY()

private:
    // 적용된 아이템들과 수량 저장 (제거용)
    UPROPERTY()
    TArray<FName> m_AppliedItemIDs;

public:
    virtual bool ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 스킬 효과 전략 인터페이스
UCLASS(Blueprintable, Abstract)
class TEAMPROJ_API USkillEffectStrategy : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ApplyEffect(const FPerkData& PerkData, UWorld* World);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool RemoveEffect(const FPerkData& PerkData, UWorld* World);
};

// 스킬 부스트 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API USkillBoostEffect : public USkillEffectStrategy
{
    GENERATED_BODY()

private:
    // 원본 스킬 데이터 저장 (복원용)
    UPROPERTY()
    FSkillData m_OriginalSkillData;

    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    USkillBoostEffect();
    
    virtual bool ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
};

// 플레이어 효과 전략 인터페이스
UCLASS(Blueprintable, Abstract)
class TEAMPROJ_API UPlayerEffectStrategy : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ApplyPlayerEffect(const FPerkData& PerkData, UWorld* World);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool RemovePlayerEffect(const FPerkData& PerkData, UWorld* World);
};

// 라운드 효과 전략 인터페이스
UCLASS(Blueprintable, Abstract)
class TEAMPROJ_API URoundEffectStrategy : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ApplyRoundEffect(const FPerkData& PerkData, UWorld* World);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool RemoveRoundEffect(const FPerkData& PerkData, UWorld* World);
};

// 골드 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UGoldIncreaseEffect : public UPlayerEffectStrategy
{
    GENERATED_BODY()

public:
    virtual bool ApplyPlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemovePlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 경험치 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UExpIncreaseEffect : public UPlayerEffectStrategy
{
    GENERATED_BODY()

public:
    virtual bool ApplyPlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemovePlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 라운드 골드 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API URoundGoldIncreaseEffect : public URoundEffectStrategy
{
    GENERATED_BODY()

public:
    virtual bool ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 라운드 경험치 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API URoundExpIncreaseEffect : public URoundEffectStrategy
{
    GENERATED_BODY()

public:
    virtual bool ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 라운드 아이템 수량 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API URoundItemQuantityEffect : public URoundEffectStrategy
{
    GENERATED_BODY()

public:
    virtual bool ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 라운드 전체 아이템 수량 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API URoundAllItemsQuantityEffect : public URoundEffectStrategy
{
    GENERATED_BODY()

private:
    // 적용된 아이템들과 수량 저장 (제거용)
    UPROPERTY()
    TArray<FName> m_AppliedItemIDs;

public:
    virtual bool ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
};

// 스탯 효과 전략 인터페이스
UCLASS(Blueprintable, Abstract)
class TEAMPROJ_API UStatEffectStrategy : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool ApplyStatEffect(const FPerkData& PerkData, UWorld* World);
    
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    bool RemoveStatEffect(const FPerkData& PerkData, UWorld* World);
};

// 플레이어 스탯 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UPlayerStatEffect : public UStatEffectStrategy
{
    GENERATED_BODY()

private:
    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    virtual bool ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
    
    // 헬퍼 함수들
    bool ApplyStatModifier(const FPerkData& PerkData, UWorld* World);
    bool RemoveStatModifier(const FPerkData& PerkData, UWorld* World);
};

// 아군 스탯 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UAllyStatEffect : public UStatEffectStrategy
{
    GENERATED_BODY()

private:
    // 이미 적용된 아군 캐릭터들을 캐시 (성능 최적화)
    UPROPERTY()
    TArray<class UStatusComponent*> m_AppliedAllies;
    
    // 적용된 스탯 모디파이어 저장
    FStatModifier m_AppliedModifier;

    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    virtual bool ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 새 아군이 소환될 때만 개별 적용 (PerkComponent에서 호출)
    UFUNCTION(BlueprintCallable)
    void ApplyToNewAlly(class UStatusComponent* NewAllyStatusComp);
    
    // 특정 아군에게 적용된 효과 제거
    UFUNCTION(BlueprintCallable)
    void RemoveFromAlly(class UStatusComponent* AllyStatusComp);
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
    
    // 헬퍼 함수들
    bool ApplyStatModifierToAllAllies(const FPerkData& PerkData, UWorld* World);
    bool RemoveStatModifierFromAllAllies(const FPerkData& PerkData, UWorld* World);
};

// 적군 스탯 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UEnemyStatEffect : public UStatEffectStrategy
{
    GENERATED_BODY()

private:
    // 이미 적용된 적군 캐릭터들을 캐시 (성능 최적화)
    UPROPERTY()
    TArray<class UStatusComponent*> m_AppliedEnemies;
    
    // 적용된 스탯 모디파이어 저장
    FStatModifier m_AppliedModifier;

    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    virtual bool ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 새 적군이 소환될 때만 개별 적용 (PerkComponent에서 호출)
    UFUNCTION(BlueprintCallable)
    void ApplyToNewEnemy(class UStatusComponent* NewEnemyStatusComp);
    
    // 특정 적군에게 적용된 효과 제거
    UFUNCTION(BlueprintCallable)
    void RemoveFromEnemy(class UStatusComponent* EnemyStatusComp);
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
    
    // 헬퍼 함수들
    bool ApplyStatModifierToAllEnemies(const FPerkData& PerkData, UWorld* World);
    bool RemoveStatModifierFromAllEnemies(const FPerkData& PerkData, UWorld* World);
};

// 특정 무기 타입 아군 유닛 스탯 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UAllyUnitStatEffect : public UStatEffectStrategy
{
    GENERATED_BODY()

private:
    // 이미 적용된 유닛 캐릭터들을 캐시 (성능 최적화)
    UPROPERTY()
    TArray<class UStatusComponent*> m_AppliedUnits;
    
    // 적용된 스탯 모디파이어 저장
    FStatModifier m_AppliedModifier;

    // 타겟 무기 타입 저장
    EWeaponType m_TargetWeaponType = EWeaponType::None;

    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    virtual bool ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 특정 유닛에게 적용된 효과 제거
    UFUNCTION(BlueprintCallable)
    void RemoveFromUnit(class UStatusComponent* UnitStatusComp);
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
    
    // 헬퍼 함수들
    bool ApplyStatModifierToTargetUnits(const FPerkData& PerkData, UWorld* World);
    bool RemoveStatModifierFromTargetUnits(const FPerkData& PerkData, UWorld* World);
};

// 특정 무기 타입 적군 유닛 스탯 증가 효과 전략
UCLASS(Blueprintable)
class TEAMPROJ_API UEnemyUnitStatEffect : public UStatEffectStrategy
{
    GENERATED_BODY()

private:
    // 이미 적용된 유닛 캐릭터들을 캐시 (성능 최적화)
    UPROPERTY()
    TArray<class UStatusComponent*> m_AppliedUnits;
    
    // 적용된 스탯 모디파이어 저장
    FStatModifier m_AppliedModifier;

    // 타겟 무기 타입 저장
    EWeaponType m_TargetWeaponType = EWeaponType::None;

    // 라운드 기반 지속시간 추적
    UPROPERTY()
    FRoundBasedEffect m_RoundBasedEffect;

    // 현재 라운드 수
    UPROPERTY()
    int32 m_CurrentRound = 0;

    // 지연 활성화 관련 변수들
    UPROPERTY()
    bool m_bIsActivated = false;          // 실제 효과가 적용되었는지 여부

    UPROPERTY()
    bool m_bWaitingForNextRound = false;  // 다음 라운드를 기다리는 중인지 여부

public:
    virtual bool ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    virtual bool RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World) override;
    
    // 특정 유닛에게 적용된 효과 제거
    UFUNCTION(BlueprintCallable)
    void RemoveFromUnit(class UStatusComponent* UnitStatusComp);
    
    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();
    
    // 라운드 수 업데이트
    UFUNCTION(BlueprintCallable)
    void UpdateCurrentRound(int32 NewRound);
    
    // 헬퍼 함수들
    bool ApplyStatModifierToTargetUnits(const FPerkData& PerkData, UWorld* World);
    bool RemoveStatModifierFromTargetUnits(const FPerkData& PerkData, UWorld* World);
};

// 기본 퍼크 클래스 (추상 클래스)
UCLASS(Blueprintable, Abstract)
class TEAMPROJ_API UPerk : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString m_PerkName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FPerkData m_PerkData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool m_bIsActive = false;

public:
    UPerk();

    UFUNCTION(BlueprintCallable)
    FPerkData GetPerkData() const { return m_PerkData; }

    UFUNCTION(BlueprintCallable)
    FString GetPerkName() const { return m_PerkName; }

    UFUNCTION(BlueprintCallable)
    bool IsActive() const { return m_bIsActive; }

    UFUNCTION(BlueprintCallable)
    void SetPerkData(const FPerkData& pData);

    UFUNCTION(BlueprintCallable)
    void ActivatePerk();

    UFUNCTION(BlueprintCallable)
    void DeactivatePerk();

    UFUNCTION(BlueprintCallable)
    void TogglePerk();

    // 타입별 특별한 활성화/비활성화 로직을 위한 가상 함수
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnPerkActivated();

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void OnPerkDeactivated();
};


// 스텟 퍼크 클래스
UCLASS(Blueprintable)
class TEAMPROJ_API UStatPerk : public UPerk
{
    GENERATED_BODY()

public:
    UStatPerk();

    virtual void OnPerkActivated_Implementation() override;
    virtual void OnPerkDeactivated_Implementation() override;

    // 라운드 시작 시 호출되는 함수 (라운드 기반 지속시간용)
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();

    // 스탯 효과 전략 팩토리 함수
    UFUNCTION()
    UStatEffectStrategy* CreateStatEffectStrategy(EStatEffectType EffectType);

    // 아군 스탯 효과 인스턴스 저장 (캐시 시스템 활용)
    UPROPERTY()
    UAllyStatEffect* m_AllyStatEffect = nullptr;
    
    // 적군 스탯 효과 인스턴스 저장 (캐시 시스템 활용)
    UPROPERTY()
    UEnemyStatEffect* m_EnemyStatEffect = nullptr;
    
    // 플레이어 스탯 효과 인스턴스 저장 (캐시 시스템 활용)
    UPROPERTY()
    UPlayerStatEffect* m_PlayerStatEffect = nullptr;
    
    // 아군 유닛 스탯 효과 인스턴스 저장 (캐시 시스템 활용)
    UPROPERTY()
    UAllyUnitStatEffect* m_AllyUnitStatEffect = nullptr;
    
    // 적군 유닛 스탯 효과 인스턴스 저장 (캐시 시스템 활용)
    UPROPERTY()
    UEnemyUnitStatEffect* m_EnemyUnitStatEffect = nullptr;
};


// 플레이어 퍼크 클래스
UCLASS(Blueprintable)
class TEAMPROJ_API UPlayerPerk : public UPerk
{
    GENERATED_BODY()

public:
    UPlayerPerk();

    virtual void OnPerkActivated_Implementation() override;
    virtual void OnPerkDeactivated_Implementation() override;

private:
    // 플레이어 효과 전략 팩토리 함수
    UFUNCTION()
    UPlayerEffectStrategy* CreatePlayerEffectStrategy(EPlayerEffectType EffectType);
};

// 아이템 퍼크 클래스
UCLASS(Blueprintable)
class TEAMPROJ_API UItemPerk : public UPerk
{
    GENERATED_BODY()

public:
    UItemPerk();

    virtual void OnPerkActivated_Implementation() override;
    virtual void OnPerkDeactivated_Implementation() override;

    // 라운드 시작 시 호출되는 함수 (ItemBoost 지연 활성화용)
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();

private:
    // 효과 전략 팩토리 함수
    UFUNCTION()
    UItemEffectStrategy* CreateEffectStrategy(EItemEffectType EffectType);
    
    // 현재 사용 중인 효과 전략 저장
    UPROPERTY()
    UItemEffectStrategy* m_CurrentEffectStrategy = nullptr;
};

// 스킬 퍼크 클래스
UCLASS(Blueprintable)
class TEAMPROJ_API USkillPerk : public UPerk
{
    GENERATED_BODY()

public:
    USkillPerk();

    virtual void OnPerkActivated_Implementation() override;
    virtual void OnPerkDeactivated_Implementation() override;

    // 라운드 시작 시 호출되는 함수 (SkillBoost 지연 활성화용)
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();

private:
    // 효과 전략 팩토리 함수
    UFUNCTION()
    USkillEffectStrategy* CreateEffectStrategy(EPerkSkillEffectType EffectType);
    
    // 현재 사용 중인 효과 전략 저장
    UPROPERTY()
    USkillEffectStrategy* m_CurrentEffectStrategy = nullptr;
};

// 라운드 보너스 퍼크 클래스
UCLASS(Blueprintable)
class TEAMPROJ_API URoundBonusPerk : public UPerk
{
    GENERATED_BODY()

public:
    URoundBonusPerk();

    virtual void OnPerkActivated_Implementation() override;
    virtual void OnPerkDeactivated_Implementation() override;

    // 라운드 시작 시 호출되는 함수
    UFUNCTION(BlueprintCallable)
    void OnRoundStart();

private:
    // 라운드 효과 전략 팩토리 함수
    UFUNCTION()
    URoundEffectStrategy* CreateRoundEffectStrategy(ERoundBonusEffectType EffectType);

    // 라운드 기반 지속 시간 추적
    UPROPERTY()
    int32 m_RemainingRounds = 0;
    
    // 지연 적용을 위한 플래그들
    UPROPERTY()
    bool m_bWaitingForNextRound = false;
    
    UPROPERTY()
    bool m_bIsActivated = false;
};
