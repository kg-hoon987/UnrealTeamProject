// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Status/StatusStruct.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "UObject/NoExportTypes.h"
#include "../Skill/SkillStruct.h"
#include "ItemStruct.generated.h"

class USkillTriggerStrategy;
class UTargetingTask;
class USkillEffect;
class ASkillProjectile;

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FName                               ItemID = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FString                             ItemName = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FString                             Description = TEXT("");
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     UTexture2D* Icon = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     UAnimMontage* Animation = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     USoundBase* Sound = nullptr;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               HalfHeight = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Range = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Radius = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               ConeHalfAngle = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Cooldown = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               CastingTime = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     float                               Duration = 0.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FProjectileInfo                     ProjectileInfo;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)     FSkillLevelData                     ItemLevel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)     ETargetAffiliation                  TargetAffiliation = ETargetAffiliation::HostileOnly;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TSubclassOf<USkillTriggerStrategy>  TriggerClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TSubclassOf<UTargetingTask>         TargetingClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     TArray<TSubclassOf<USkillEffect>>   Effects;


    UPROPERTY(EditAnywhere, BlueprintReadWrite)     int32                               MaxQuantity = 0;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)     int32                               Cost = 0;
};

///////////////////////////Object//////////////////////////////

UCLASS(Blueprintable)
class TEAMPROJ_API UItem : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString m_ItemName;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FItemData m_ItemData;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float m_CoolRemaining;
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 m_CurrentQuantity;

public:
    UItem();
    UFUNCTION(BlueprintCallable)
    FItemData GetItemData() const { return m_ItemData; }
    UFUNCTION(BlueprintCallable)
    const FSkillLevelData& GetItemLevelData() const;
    UFUNCTION(BlueprintCallable)
    int32 GetItemQuantity() { return m_CurrentQuantity; }

    UFUNCTION(BlueprintCallable)
    void SetItemData(const FItemData& pData, int32 nQuantity);
    UFUNCTION(BlueprintCallable)
    void SetItemQuantity(int32 nQuantity);

    UFUNCTION(BlueprintCallable)
    int32 AddToQuantity(int32 Amount);
    UFUNCTION(BlueprintCallable)
    int32 RemoveFromQuantity(int32 Amount);

    UFUNCTION(BlueprintCallable)
    bool UseItem(AActor* User, AActor* OptionalTarget = nullptr, const FVector& OptionalLocation = FVector::ZeroVector);

    UFUNCTION(BlueprintCallable)
    bool CoolStart();
    UFUNCTION(BlueprintCallable)
    void CoolDown(float DeltaTime);
    UFUNCTION(BlueprintCallable)
    bool IsReady() const { return m_CoolRemaining <= 0.f; }
    UFUNCTION(BlueprintCallable)
    bool HasQuantity() const { return m_CurrentQuantity > 0; }
};
