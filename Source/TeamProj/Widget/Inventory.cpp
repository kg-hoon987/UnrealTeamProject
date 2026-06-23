#include "Inventory.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "../Item/ItemComponent.h"
#include "ItemSlot.h"

UInventory::UInventory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedItemComponent = nullptr;
}

void UInventory::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventory::SetupInventory(UItemComponent* ItemComponent)
{
	if (!ItemComponent || !ItemSlotUniformGridPanel)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetupInventory: ItemComponent or ItemSlotUniformGridPanel is null"));
		return;
	}

	CachedItemComponent = ItemComponent;

	// ItemComponent의 OnItemDataChanged 델리게이트 바인딩
	BindToItemDataChanged();

	// ItemComponent의 OnItemQuantityChanged 델리게이트 바인딩
	BindToItemQuantityChanged();

	// 기존 슬롯들 제거
	ItemSlotUniformGridPanel->ClearChildren();

	// 모든 아이템 데이터 가져오기
	TArray<UItem*> AllItems = ItemComponent->GetAllItemData();

	// 아이템 슬롯 생성 및 배치
	for (int32 i = 0; i < AllItems.Num(); ++i)
	{
		UItem* Item = AllItems[i];
		if (!Item)
		{
			continue;
		}

		FItemData ItemData = Item->GetItemData();
		int32 ItemQuantity = ItemComponent->GetItemQuantity(ItemData.ItemID);

		// ItemSlot 위젯 생성 (W_ItemSlot 클래스 사용)
		UClass* ItemSlotClass = LoadClass<UItemSlot>(nullptr, TEXT("/Game/Widget/W_ItemSlot.W_ItemSlot_C"));
		UItemSlot* ItemSlot = CreateWidget<UItemSlot>(this, ItemSlotClass);
		if (ItemSlot)
		{
			// 위젯 크기 설정 (디버깅용)
			//ItemSlot->SetDesiredSizeInViewport(FVector2D(70.0f, 70.0f));

			// 그리드 위치 계산 (5열 기준)
			int32 Row = i / 5;
			int32 Column = i % 5;


			// 그리드 패널에 추가
			ItemSlotUniformGridPanel->AddChildToUniformGrid(ItemSlot, Row, Column);

			// ItemSlot의 ItemID와 ItemQuantity 설정
			ItemSlot->SetItemSlotData(ItemData.ItemID, ItemQuantity);

			// ItemSlot 설정
			ItemSlot->SetupItemSlot(ItemComponent);

			// 위젯 가시성 확인
			ItemSlot->SetVisibility(ESlateVisibility::Visible);

			// 아이템 슬롯 클릭 델리게이트 바인딩
			ItemSlot->OnItemSlotClicked.AddDynamic(this, &UInventory::ItemSlotClicked);

		}
	}
}

void UInventory::ItemSlotClicked(FName ItemID)
{
	// 플레이어의 ItemComponent에서 아이템 변경
	if (CachedItemComponent)
	{
		CachedItemComponent->ChangeItem(ItemID);
	}

	// 인벤토리 닫기 이벤트 호출
	OnInventoryClose.Broadcast();
}

void UInventory::BindToItemDataChanged()
{
	if (CachedItemComponent)
	{
		// 기존 바인딩 해제
		CachedItemComponent->OnItemDataChanged.RemoveDynamic(this, &UInventory::OnItemDataChanged);

		// 새로운 바인딩
		CachedItemComponent->OnItemDataChanged.AddDynamic(this, &UInventory::OnItemDataChanged);
	}
}

void UInventory::BindToItemQuantityChanged()
{
	if (CachedItemComponent)
	{
		// 기존 바인딩 해제
		CachedItemComponent->OnItemQuantityChanged.RemoveDynamic(this, &UInventory::OnItemQuantityChanged);

		// 새로운 바인딩
		CachedItemComponent->OnItemQuantityChanged.AddDynamic(this, &UInventory::OnItemQuantityChanged);
	}
}

void UInventory::OnItemDataChanged(FName ItemID)
{
	if (!CachedItemComponent)
	{
		return;
	}

	// 아이템 데이터가 변경되었으므로 전체 인벤토리 새로고침
	SetupInventory(CachedItemComponent);
}

void UInventory::OnItemQuantityChanged(FName ItemID)
{
	if (!CachedItemComponent || !ItemSlotUniformGridPanel)
	{
		return;
	}

	// 해당 아이템 슬롯의 수량만 업데이트
	for (int32 i = 0; i < ItemSlotUniformGridPanel->GetChildrenCount(); ++i)
	{
		UItemSlot* ExistingSlot = Cast<UItemSlot>(ItemSlotUniformGridPanel->GetChildAt(i));
		if (ExistingSlot && ExistingSlot->GetItemID() == ItemID)
		{
			int32 NewQuantity = CachedItemComponent->GetItemQuantity(ItemID);
			ExistingSlot->SetItemSlotData(ItemID, NewQuantity);
			break;
		}
	}
}