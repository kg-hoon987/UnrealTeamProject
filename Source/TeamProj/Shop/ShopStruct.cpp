// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopStruct.h"
#include "../Character/GameCharacterBase.h"
#include "../Item/ItemComponent.h"
#include "../Status/StatusComponent.h"
#include "../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"

UShopItem::UShopItem()
{
    m_ItemName = TEXT("");
    m_CurrentStock = 0;
}

int32 UShopItem::GetCurrentPrice() const
{
    int32 CurrentPrice = m_ShopItemData.Price;

    //할인이 On 이면 할인된 가격 계산
    if (m_ShopItemData.DiscountInfo.bIsActive && m_ShopItemData.DiscountInfo.DiscountAmount > 0)
    {
        //고정 금액 할인(소수점 버림)
        int32 DiscountedPrice = CurrentPrice - m_ShopItemData.DiscountInfo.DiscountAmount;
        CurrentPrice = FMath::Max(1, DiscountedPrice);
    }

    return CurrentPrice;
}

void UShopItem::SetShopItemData(const FShopItemData& pData)
{
    this->m_ShopItemData = pData;
    this->m_ItemName = pData.ItemName;
    this->m_CurrentStock = 0;
}

bool UShopItem::CanPurchase(AGameCharacterBase* Purchaser, int32 Count) const
{
    if (!IsValid(Purchaser) || Count <= 0)
        return false;

    if (m_CurrentStock < Count)
        return false;

    int32 CurrentPrice = GetCurrentPrice();
    int32 TotalCost = CurrentPrice * Count;

    //MyGamePlayerState의 골드 체크
    if (const AController* Controller = Purchaser->GetController())
    {
        if (const AMyGamePlayerState* PlayerState = Cast<AMyGamePlayerState>(Controller->PlayerState))
        {
            if (PlayerState->Gold < TotalCost)
                return false;
        }
    }

    //보유 최대 수량 체크
    if (UItemComponent* ItemComp = Purchaser->FindComponentByClass<UItemComponent>())
    {
        if (m_ShopItemData.ItemData)
        {
            int32 CurrentQuantity = ItemComp->GetItemQuantity(m_ShopItemData.ItemID);
            int32 MaxQuantity = m_ShopItemData.ItemData->MaxQuantity;

            //보유 수량 + 구매할 수량이 최대 수량을 초과? 그럼 안되지
            if (CurrentQuantity + Count > MaxQuantity)
                return false;
        }
    }

    return true;
}

bool UShopItem::PurchaseItem(AGameCharacterBase* Purchaser, int32 Count)
{
    if (!CanPurchase(Purchaser, Count))
        return false;

    int32 CurrentPrice = GetCurrentPrice();
    int32 TotalCost = CurrentPrice * Count;

    //골드 차감
    if (AController* Controller = Purchaser->GetController())
    {
        if (AMyGamePlayerState* PlayerState = Cast<AMyGamePlayerState>(Controller->PlayerState))
        {
            PlayerState->Gold -= TotalCost;
        }
    }

    //아이템 지급
    if (UItemComponent* ItemComp = Purchaser->FindComponentByClass<UItemComponent>())
    {
        if (m_ShopItemData.ItemData)
        {
            ItemComp->AddItemQuantity(m_ShopItemData.ItemID, Count);
        }
    }

    m_CurrentStock -= Count;

    return true;
}




void UShopItem::UpdateStock(int32 NewStock)
{
    m_CurrentStock = NewStock;
}

void UShopItem::UpdateDiscount(const FShopDiscountInfo& DiscountInfo)
{
    m_ShopItemData.DiscountInfo = DiscountInfo;
}


void UShopItem::Tick(float DeltaTime)
{

}
