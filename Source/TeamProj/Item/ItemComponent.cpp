// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemComponent.h"
#include "../Damage/DamageComponent.h"

UItemComponent::UItemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_AllItems.Empty();
	m_EquippedItem = nullptr;
}

void UItemComponent::BeginPlay()
{
	Super::BeginPlay();

	//InitAllItems(); //여기서 init 안하고 플레이어에서 init함
}

void UItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UItem* Item = m_EquippedItem)
	{
		const bool IsReadyItem = Item->IsReady();
		Item->CoolDown(DeltaTime);
		if (Item->IsReady() && !IsReadyItem)
			OnItemReady.Broadcast();
	}
}

void UItemComponent::InitAllItems()
{
	UDataTable* ItemDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Item/DT_Item.DT_Item"));
	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemDataTable] Load to Failed"));
		return;
	}
	TArray<FItemData*> AllRows;
	ItemDataTable->GetAllRows(TEXT("ItemInit"), AllRows);

	for (FItemData* Row : AllRows)
	{
		if (!Row) continue;

		UItem* NewItem = NewObject<UItem>(this);
		if (!NewItem) continue;

		NewItem->SetItemData(*Row, 0);
		m_AllItems.Add(NewItem);
	}
}

bool UItemComponent::EquipItem(FName ItemID)
{
	UItem* TargetItem = GetAllItem(ItemID);
	if (!TargetItem) return false;

	m_EquippedItem = TargetItem;

	return true;
}

bool UItemComponent::UnequipItem()
{
	m_EquippedItem = nullptr;

	return true;
}

bool UItemComponent::ChangeItem(FName ItemID)
{
	UnequipItem();
	if (EquipItem(ItemID))
		return true;
	return false;
}

bool UItemComponent::TryUseItem(AActor* OptionalTarget, FVector OptionalLocation)
{
	if (!m_EquippedItem || !m_EquippedItem->IsReady() || !m_EquippedItem->HasQuantity())
		return false;

	if (m_EquippedItem->RemoveFromQuantity(1) >= 0)
	{
		FName ItemID = m_EquippedItem->GetItemData().ItemID;
		OnItemQuantityChanged.Broadcast(ItemID);
	}

	return m_EquippedItem->UseItem(GetOwner(), OptionalTarget, OptionalLocation);
}

bool UItemComponent::AddItemQuantity(FName ItemID, int32 Amount)
{
	UItem* Item = GetAllItem(ItemID);
	if (!Item)
		return false;

	int32 OldQuantity = Item->GetItemQuantity();
	int32 NewQuantity = Item->AddToQuantity(Amount);

	if (NewQuantity != OldQuantity)
		OnItemQuantityChanged.Broadcast(ItemID);

	return true;
}

bool UItemComponent::RemoveItemQuantity(FName ItemID, int32 Amount)
{
	UItem* Item = GetAllItem(ItemID);
	if (!Item)
		return false;

	int32 OldQuantity = Item->GetItemQuantity();
	int32 NewQuantity = Item->RemoveFromQuantity(Amount);

	if (NewQuantity != OldQuantity)
		OnItemQuantityChanged.Broadcast(ItemID);

	return true;
}

UItem* UItemComponent::GetEquippedItem() const
{
	return m_EquippedItem;
}

FName UItemComponent::GetEquippedItemID() const
{
	if (m_EquippedItem == nullptr)
		return NAME_None;
	return m_EquippedItem->GetItemData().ItemID;
}

UItem* UItemComponent::GetAllItem(FName ItemID) const
{
	for (UItem* Item : m_AllItems)
	{
		if (Item && Item->GetItemData().ItemID == ItemID)
			return Item;
	}
	return nullptr;
}

int32 UItemComponent::GetItemQuantity(FName ItemID) const
{
	UItem* Item = GetAllItem(ItemID);
	return Item ? Item->GetItemQuantity() : 0;
}

int32 UItemComponent::GetItemPrice(FName ItemID) const
{
	UItem* Item = GetAllItem(ItemID);
	return Item ? Item->GetItemData().Cost : 0;
}

int32 UItemComponent::GetItemSellPrice(FName ItemID) const
{
	UItem* Item = GetAllItem(ItemID);
	if (!Item)
	{
		return 0;
	}

	int32 OriginalPrice = Item->GetItemData().Cost;
	int32 SellPrice = OriginalPrice / 2; // 원래 가격의 50%

	// 최소 1원 보장
	return FMath::Max(1, SellPrice);
}

bool UItemComponent::HasEquippedItem() const
{
	return m_EquippedItem != nullptr;
}

bool UItemComponent::HasQuantity(FName ItemID) const
{
	UItem* Item = GetAllItem(ItemID);
	return Item->HasQuantity();
}

void UItemComponent::UpdateItemData(FName ItemID, const FItemData& NewItemData)
{
	UItem* Item = GetAllItem(ItemID);
	if (Item)
	{
		int32 CurrentQuantity = Item->GetItemQuantity();
		Item->SetItemData(NewItemData, CurrentQuantity);

		// 아이템 데이터 변경 알림
		OnItemDataChanged.Broadcast(ItemID);
	}
	else
	{
	}
}