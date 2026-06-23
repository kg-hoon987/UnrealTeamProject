// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "../Item/ItemStruct.h"
#include "UObject/NoExportTypes.h"
#include "ShopStruct.generated.h"

class UShopComponent;




USTRUCT(BlueprintType)
struct FShopDiscountInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 DiscountAmount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsActive = false;

    bool operator==(const FShopDiscountInfo& Other) const
    {
        return DiscountAmount == Other.DiscountAmount &&
            bIsActive == Other.bIsActive;
    }
};

USTRUCT(BlueprintType)
struct FShopItemData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ItemID = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString ItemName = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description = TEXT("");

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FShopDiscountInfo DiscountInfo;

    //실제 아이템 데이터 참조(블프에선 안씀)
    FItemData* ItemData = nullptr;

    bool operator==(const FShopItemData& Other) const
    {
        return ItemID == Other.ItemID &&
            ItemName == Other.ItemName &&
            Description == Other.Description &&
            Icon == Other.Icon &&
            Price == Other.Price &&
            DiscountInfo == Other.DiscountInfo;
    }
};





///////////////////////////Object//////////////////////////////

UCLASS(Blueprintable)
class TEAMPROJ_API UShopItem : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString m_ItemName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FShopItemData m_ShopItemData;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 m_CurrentStock;

public:
    UShopItem();

    UFUNCTION(BlueprintCallable)
    FString GetItemName() const { return m_ItemName; }

    UFUNCTION(BlueprintCallable)
    FShopItemData GetShopItemData() const { return m_ShopItemData; }

    UFUNCTION(BlueprintCallable)
    int32 GetCurrentStock() const { return m_CurrentStock; }


    UFUNCTION(BlueprintCallable)
    int32 GetCurrentPrice() const;

    UFUNCTION(BlueprintCallable)
    void SetShopItemData(const FShopItemData& pData);

    UFUNCTION(BlueprintCallable)
    bool CanPurchase(AGameCharacterBase* Purchaser, int32 Count = 1) const;

    UFUNCTION(BlueprintCallable)
    bool PurchaseItem(AGameCharacterBase* Purchaser, int32 Count = 1);




    UFUNCTION(BlueprintCallable)
    void UpdateStock(int32 NewStock);

    UFUNCTION(BlueprintCallable)
    void UpdateDiscount(const FShopDiscountInfo& DiscountInfo);


    UFUNCTION(BlueprintCallable)
    void Tick(float DeltaTime);
};
