// Fill out your copyright notice in the Description page of Project Settings.

#include "ShopComponent.h"
#include "../Character/GameCharacterBase.h"
#include "../Item/ItemComponent.h"
#include "../Status/StatusComponent.h"

UShopComponent::UShopComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    m_AllShopItems.Empty();
    m_PlayerItemComponent = nullptr;
    m_ShopSlotCount = 1;
    m_RefreshCount = 1;
    m_Salepercent = 10.f;
}

void UShopComponent::BeginPlay()
{
    Super::BeginPlay();

    // SetupShopComp이 호출되지 않았다면 기본값으로 초기화
    if (m_ShopSlotCount <= 1 && m_RefreshCount <= 1)
    {
        InitAllShopItems();
    }
}

void UShopComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UShopComponent::InitAllShopItems()
{
    TArray<FItemData*> ShopItems;

    // 플레이어의 ItemComponent가 있으면 최신 아이템 데이터 사용
    if (m_PlayerItemComponent)
    {
        TArray<UItem*> AllItems = m_PlayerItemComponent->GetAllItemData();
        for (UItem* Item : AllItems)
        {
            if (Item)
            {
                // UItem의 FItemData를 포인터로 변환하여 사용
                FItemData* ItemDataPtr = new FItemData(Item->GetItemData());
                ShopItems.Add(ItemDataPtr);
            }
        }
    }
    else
    {
        // 기존 방식: 데이터테이블에서 직접 가져오기
        UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Item/DT_Item.DT_Item"));
        if (!ItemDataTable)
            return;

        TArray<FItemData*> AllRows;
        ItemDataTable->GetAllRows(TEXT("ItemInit"), AllRows);

        for (FItemData* Row : AllRows)
        {
            if (Row)
            {
                ShopItems.Add(Row);
            }
        }
    }

    //섞어
    for (int32 i = ShopItems.Num() - 1; i > 0; i--)
    {
        int32 j = FMath::RandRange(0, i);
        ShopItems.Swap(i, j);
    }

    //상점 칸 갯수만큼 아이템 선택
    int32 ShopItemCount = m_ShopSlotCount;
    for (int32 i = 0; i < ShopItemCount; i++)
    {
        FItemData* ItemData = ShopItems[i];
        if (!ItemData) continue;

        //ItemData -> ShopItemData 넣기
        FShopItemData ShopItemData;
        ShopItemData.ItemID = ItemData->ItemID;
        ShopItemData.ItemName = ItemData->ItemName;
        ShopItemData.Description = ItemData->Description;
        ShopItemData.Icon = ItemData->Icon;
        ShopItemData.Price = ItemData->Cost;
        ShopItemData.ItemData = ItemData;

        //랜덤 할인 설정 (30% 확률로 할인, 1원 아이템은 할인 제외)
        if (FMath::RandRange(0, 100) < m_Salepercent && ItemData->Cost > 1)
        {
            FShopDiscountInfo DiscountInfo;
            DiscountInfo.bIsActive = true;

            //고정 금액 할인 (가격의 10% ~ 50% 범위, 소수점 버림)
            int32 MinDiscount = FMath::FloorToInt(ItemData->Cost * 0.1f);
            int32 MaxDiscount = FMath::FloorToInt(ItemData->Cost * 0.5f);

            // 최소 1원 이상 할인되도록 보장
            if (MinDiscount < 1) MinDiscount = 1;
            if (MaxDiscount < MinDiscount) MaxDiscount = MinDiscount;

            DiscountInfo.DiscountAmount = FMath::RandRange(MinDiscount, MaxDiscount);

            ShopItemData.DiscountInfo = DiscountInfo;
        }

        UShopItem* NewShopItem = NewObject<UShopItem>(this);
        if (!NewShopItem) continue;

        NewShopItem->SetShopItemData(ShopItemData);

        //랜덤 재고 (5~10개)
        int32 RandomStock = FMath::RandRange(5, 10);
        NewShopItem->UpdateStock(RandomStock);

        m_AllShopItems.Add(NewShopItem);
    }
}

void UShopComponent::RefreshShopItems()
{
    m_AllShopItems.Empty();

    InitAllShopItems();
}

void UShopComponent::SetupShopComp(int32 SlotCount,float Sale, int32 RefreshCount)
{
    m_ShopSlotCount = SlotCount;
    m_Salepercent = Sale;
    m_RefreshCount = RefreshCount;

    //상점 새로고침
    RefreshShopItems();
}

bool UShopComponent::PurchaseItem(FName ItemID, int32 Count)
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    if (!ShopItem) return false;

    AGameCharacterBase* Character = Cast<AGameCharacterBase>(GetOwner());
    if (!Character) return false;

    if (!ShopItem->CanPurchase(Character, Count)) return false;

    int32 CurrentPrice = ShopItem->GetCurrentPrice();
    int32 TotalCost = CurrentPrice * Count;

    if (ShopItem->PurchaseItem(Character, Count))
    {
        OnShopItemPurchased.Broadcast(ItemID);
        OnShopStockChanged.Broadcast(ItemID);

        return true;
    }

    return false;
}


bool UShopComponent::CanPurchaseItem(FName ItemID, int32 Count) const
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    if (!ShopItem) return false;

    AGameCharacterBase* Character = Cast<AGameCharacterBase>(GetOwner());
    if (!Character) return false;

    return ShopItem->CanPurchase(Character, Count);
}



void UShopComponent::UpdateShopItemStock(FName ItemID, int32 NewStock)
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    if (ShopItem)
    {
        ShopItem->UpdateStock(NewStock);
        OnShopStockChanged.Broadcast(ItemID);
    }
}

void UShopComponent::UpdateShopItemDiscount(FName ItemID, const FShopDiscountInfo& DiscountInfo)
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    if (ShopItem)
    {
        ShopItem->UpdateDiscount(DiscountInfo);
    }
}

UShopItem* UShopComponent::GetShopItem(FName ItemID) const
{
    for (UShopItem* ShopItem : m_AllShopItems)
    {
        if (ShopItem && ShopItem->GetShopItemData().ItemID == ItemID)
        {
            return ShopItem;
        }
    }
    return nullptr;
}

TArray<UShopItem*> UShopComponent::GetAllShopItems() const
{
    return m_AllShopItems;
}

int32 UShopComponent::GetShopItemPrice(FName ItemID) const
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    return ShopItem ? ShopItem->GetCurrentPrice() : 0;
}


int32 UShopComponent::GetShopItemStock(FName ItemID) const
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    return ShopItem ? ShopItem->GetCurrentStock() : 0;
}

bool UShopComponent::CanRefreshShop() const
{
    return m_RefreshCount > 0;
}

bool UShopComponent::UseRefreshCount()
{
    if (m_RefreshCount > 0)
    {
        m_RefreshCount--;
        return true;
    }
    return false;
}

void UShopComponent::SyncShopItemData(FName ItemID, const FItemData& NewItemData)
{
    UShopItem* ShopItem = GetShopItem(ItemID);
    if (ShopItem)
    {
        FShopItemData ShopData = ShopItem->GetShopItemData();

        // 아이템 데이터 동기화
        ShopData.ItemName = NewItemData.ItemName;
        ShopData.Description = NewItemData.Description;
        ShopData.Icon = NewItemData.Icon;
        ShopData.Price = NewItemData.Cost; // 상점 가격을 아이템 비용으로 동기화

        // ShopItem의 데이터 업데이트
        ShopItem->SetShopItemData(ShopData);
    }
}

void UShopComponent::SetPlayerItemComponent(UItemComponent* ItemComponent)
{
    m_PlayerItemComponent = ItemComponent;
}