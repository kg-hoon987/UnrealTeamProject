// Fill out your copyright notice in the Description page of Project Settings.

#include "SellShop.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "../../Item/ItemComponent.h"
#include "../../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "SellItemSlot.h"

USellShop::USellShop(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CachedItemComponent = nullptr;
	SelectedItemID = NAME_None;
	TotalSellPrice = 0;
}

void USellShop::NativeConstruct()
{
	Super::NativeConstruct();

	if (InButton)
	{
		InButton->OnClicked.AddDynamic(this, &USellShop::OnInButtonClicked);
	}

	if (OutButton)
	{
		OutButton->OnClicked.AddDynamic(this, &USellShop::OnOutButtonClicked);
	}

	if (SellButton)
	{
		SellButton->OnClicked.AddDynamic(this, &USellShop::OnSellButtonClicked);
	}

	if (SellText)
	{
		SellText->SetText(FText::FromString(TEXT("총 판매가: 0")));
	}

	if (ItemDetail)
	{
		ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USellShop::OnInButtonClicked()
{
	if (SelectedItemID == NAME_None || !CachedItemComponent || !SellSlotUniformGridPanel)
	{
		return;
	}

	int32 AvailableQuantity = CachedItemComponent->GetItemQuantity(SelectedItemID);
	if (AvailableQuantity <= 0)
	{
		return;
	}

	// 이미 판매 목록에 있는 아이템인지 확인
	for (USellItemSlot* SellSlot : SellItemSlots)
	{
		if (SellSlot && SellSlot->GetItemID() == SelectedItemID)
		{
			// 현재 판매 목록에 있는 수량 확인
			int32 CurrentSellQuantity = SellSlot->GetItemQuantity();
			
			// 보유 수량을 초과하는지 확인
			if (CurrentSellQuantity >= AvailableQuantity)
			{
				return;
			}
			
			// 수량 증가
			SellSlot->UpdateQuantity(CurrentSellQuantity + 1);
			UpdateTotalSellPrice();
			return;
		}
	}

	// 새로운 아이템을 판매 목록에 추가

	// SellItemSlot 위젯 생성
	UClass* SellItemSlotClass = LoadClass<USellItemSlot>(nullptr, TEXT("/Game/Widget/W_SellItemSlot.W_SellItemSlot_C"));
	USellItemSlot* NewSellSlot = CreateWidget<USellItemSlot>(this, SellItemSlotClass);
	if (NewSellSlot)
	{
		// 그리드 위치 계산
		int32 SlotIndex = SellItemSlots.Num();
		int32 Row = SlotIndex / 5;
		int32 Column = SlotIndex % 5;

		// SellItemSlot 설정
		NewSellSlot->SetupSellItemSlot(CachedItemComponent, SelectedItemID, 1);

		// 그리드 패널에 추가
		SellSlotUniformGridPanel->AddChildToUniformGrid(NewSellSlot, Row, Column);

		// 슬롯 배열에 추가
		SellItemSlots.Add(NewSellSlot);

		// 클릭 이벤트 바인딩
		NewSellSlot->OnSellItemSlotClicked.AddDynamic(this, &USellShop::OnSellItemSlotClicked);

		// 총 판매가 업데이트
		UpdateTotalSellPrice();
	}
}

void USellShop::OnOutButtonClicked()
{
	if (SelectedItemID == NAME_None)
	{
		return;
	}

	// 선택된 아이템을 판매 목록에서 제거
	RemoveFromSellList(SelectedItemID);
}

void USellShop::OnSellButtonClicked()
{
	if (SellItemSlots.Num() == 0 || TotalSellPrice <= 0)
	{
		return;
	}

	APawn* PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn)
	{
		return;
	}

	AMyGamePlayerState* PlayerState = Cast<AMyGamePlayerState>(PlayerPawn->GetPlayerState());
	if (!PlayerState)
	{
		return;
	}

	if (!CachedItemComponent)
	{
		return;
	}

	// 모든 판매 아이템들을 처리
	bool bAllSellsSuccessful = true;
	for (USellItemSlot* SellSlot : SellItemSlots)
	{
		if (!SellSlot) continue;

		FName ItemID = SellSlot->GetItemID();
		int32 SellQuantity = SellSlot->GetItemQuantity();

		// 아이템 제거 시도
		bool bRemoved = CachedItemComponent->RemoveItemQuantity(ItemID, SellQuantity);
		if (!bRemoved)
		{
			bAllSellsSuccessful = false;
			break;
		}

	}

	// 모든 판매가 성공했으면 playerstate에 골드 지급
	if (bAllSellsSuccessful)
	{
		PlayerState->Gold += TotalSellPrice;

		// 판매 목록 초기화
		ClearSellList();

		// 인벤토리 새로고침
		RefreshSellData();

		// 판매 성공 델리게이트 호출
		OnSellSuccess.Broadcast();
	}
	else
	{
	}
}

void USellShop::SetupSellShop(UItemComponent* ItemComponent)
{
	if (!ItemComponent || !ItemSlotUniformGridPanel)
	{
		return;
	}

	CachedItemComponent = ItemComponent;

	// ItemComponent의 OnItemQuantityChanged 델리게이트 바인딩 <- 이거 안하면 구매 상점에서 수량 변화시 판매 상점에 반영 안됨
	BindToItemQuantityChanged();

	// ItemComponent의 OnItemDataChanged 델리게이트 바인딩 <- 아이템 데이터 변경시 판매 상점에 반영
	BindToItemDataChanged();

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

		// SellItemSlot 위젯 생성 (W_SellItemSlot 클래스 사용)
		UClass* SellItemSlotClass = LoadClass<USellItemSlot>(nullptr, TEXT("/Game/Widget/W_SellItemSlot.W_SellItemSlot_C"));
		USellItemSlot* ItemSlot = CreateWidget<USellItemSlot>(this, SellItemSlotClass);
		if (ItemSlot)
		{
			// 그리드 위치 계산 5x5
			int32 Row = i / 5;
			int32 Column = i % 5;

			// 그리드 패널에 추가
			ItemSlotUniformGridPanel->AddChildToUniformGrid(ItemSlot, Row, Column);

			// SellItemSlot 설정 (수량 0으로 설정하여 보유 아이템만 표시)
			ItemSlot->SetupSellItemSlot(ItemComponent, ItemData.ItemID, 0);

			// 인벤토리 모드로 설정 (보유 수량만 표시)
			ItemSlot->SetInventoryMode(true);

			// 위젯 가시성 확인
			ItemSlot->SetVisibility(ESlateVisibility::Visible);

			// 아이템 슬롯 이벤트 바인딩
			ItemSlot->OnSellItemSlotClicked.AddDynamic(this, &USellShop::OnItemSlotClicked);
			ItemSlot->OnSellItemSlotHovered.AddDynamic(this, &USellShop::OnSellItemSlotHovered);
			ItemSlot->OnSellItemSlotUnhovered.AddDynamic(this, &USellShop::OnSellItemSlotUnhovered);
		}
	}
}

void USellShop::RefreshSellData()
{
	if (!CachedItemComponent)
	{
		return;
	}

	SetupSellShop(CachedItemComponent);
}

void USellShop::OnItemSlotClicked(FName ItemID)
{
	// 선택된 아이템 ID 저장
	SelectedItemID = ItemID;

	for (auto* a : ItemSlotUniformGridPanel->GetAllChildren())
	{
		USellItemSlot* SellSlot = Cast<USellItemSlot>(a);
		if (!SellSlot)
			UE_LOG(LogTemp, Warning, TEXT("SellItemSlotCastingfailed"));

		if (SellSlot->GetItemID() != ItemID)
			SellSlot->SetDeSelected();
		else
			UE_LOG(LogTemp, Warning, TEXT("ItemSlotDefind"));


	}
}

void USellShop::RemoveFromSellList(FName ItemID)
{
	for (int32 i = 0; i < SellItemSlots.Num(); ++i)
	{
		USellItemSlot* SellSlot = SellItemSlots[i];
		if (SellSlot && SellSlot->GetItemID() == ItemID)
		{
			// 수량이 1보다 크면 수량 감소
			int32 CurrentQuantity = SellSlot->GetItemQuantity();
			if (CurrentQuantity > 1)
			{
				SellSlot->UpdateQuantity(CurrentQuantity - 1);
				UpdateTotalSellPrice();
				return;
			}
			else
			{
				// 수량이 1이면 슬롯 제거
				SellSlotUniformGridPanel->RemoveChild(SellSlot);
				SellItemSlots.RemoveAt(i);
				UpdateTotalSellPrice();
				
				// 그리드 재배치
				RearrangeSellSlots();
				
				return;
			}
		}
	}
	
}

void USellShop::OnSellItemSlotClicked(FName ItemID)
{
	// 판매 슬롯을 클릭하면 해당 아이템 선택
	SelectedItemID = ItemID;


}

void USellShop::UpdateTotalSellPrice()
{
	TotalSellPrice = 0;
	
	if (!CachedItemComponent)
	{
		return;
	}
	
	for (USellItemSlot* SellSlot : SellItemSlots)
	{
		if (SellSlot)
		{
			FName ItemID = SellSlot->GetItemID();
			int32 SellPrice = CachedItemComponent->GetItemSellPrice(ItemID);
			int32 Quantity = SellSlot->GetItemQuantity();
			TotalSellPrice += SellPrice * Quantity;
		}
	}
	
	// SellText UI 업데이트
	if (SellText)
	{
		FString PriceText = FString::Printf(TEXT("총 판매가: %d"), TotalSellPrice);
		SellText->SetText(FText::FromString(PriceText));
	}
}

void USellShop::RearrangeSellSlots()
{
	if (!SellSlotUniformGridPanel)
	{
		return;
	}
	
	// 기존 슬롯들 제거
	SellSlotUniformGridPanel->ClearChildren();
	
	// 슬롯들을 다시 배치
	for (int32 i = 0; i < SellItemSlots.Num(); ++i)
	{
		USellItemSlot* SellSlot = SellItemSlots[i];
		if (SellSlot)
		{
			int32 Row = i / 5;
			int32 Column = i % 5;
			SellSlotUniformGridPanel->AddChildToUniformGrid(SellSlot, Row, Column);
		}
	}
}

void USellShop::ClearSellList()
{
	// 모든 판매 슬롯 제거
	if (SellSlotUniformGridPanel)
	{
		SellSlotUniformGridPanel->ClearChildren();
	}

	// 슬롯 배열 초기화
	SellItemSlots.Empty();

	// 총 가격 초기화
	TotalSellPrice = 0;

	// UI 업데이트
	if (SellText)
	{
		SellText->SetText(FText::FromString(TEXT("총 판매가: 0")));
	}

	// 선택된 아이템 초기화
	SelectedItemID = NAME_None;
}

void USellShop::BindToItemQuantityChanged()
{
	if (CachedItemComponent)
	{
		// 계속 AddDynamic만 하니까 누적되면서 여러번 함수가 불리는 현상 발생;; 왠만하면 해제하고 다시 바인딩 ㄱㄱ
		// 기존 바인딩 해제
		CachedItemComponent->OnItemQuantityChanged.RemoveDynamic(this, &USellShop::OnItemQuantityChanged);
		
		// 새로운 바인딩
		CachedItemComponent->OnItemQuantityChanged.AddDynamic(this, &USellShop::OnItemQuantityChanged);
	}
}

void USellShop::BindToItemDataChanged()
{
	if (CachedItemComponent)
	{
		// 기존 바인딩 해제
		CachedItemComponent->OnItemDataChanged.RemoveDynamic(this, &USellShop::OnItemDataChanged);

		// 새로운 바인딩
		CachedItemComponent->OnItemDataChanged.AddDynamic(this, &USellShop::OnItemDataChanged);
	}
}

void USellShop::OnItemQuantityChanged(FName ItemID)
{
	if (!CachedItemComponent || !ItemSlotUniformGridPanel)
	{
		return;
	}

	// 기존 슬롯들의 수량 업데이트
	for (int32 i = 0; i < ItemSlotUniformGridPanel->GetChildrenCount(); ++i)
	{
		USellItemSlot* ExistingSlot = Cast<USellItemSlot>(ItemSlotUniformGridPanel->GetChildAt(i));
		if (ExistingSlot && ExistingSlot->GetItemID() == ItemID)
		{
			ExistingSlot->ForceUpdateQuantity();
		}
	}
}

void USellShop::OnItemDataChanged(FName ItemID)
{
	if (!CachedItemComponent || !ItemSlotUniformGridPanel)
	{
		return;
	}

	// 아이템 데이터가 변경되었으므로 전체 인벤토리 새로고침
	RefreshSellData();
}

void USellShop::OnSellItemSlotHovered(FName ItemID)
{
	UpdateItemDetail(ItemID);
}

void USellShop::OnSellItemSlotUnhovered(FName ItemID)
{
	HideItemDetail();
}

void USellShop::UpdateItemDetail(FName ItemID)
{
	if (!CachedItemComponent || !ItemDetail)
	{
		return;
	}

	UItem* Item = CachedItemComponent->GetAllItem(ItemID);
	if (!Item)
	{
		return;
	}

	FItemData ItemData = Item->GetItemData();
	int32 ItemQuantity = CachedItemComponent->GetItemQuantity(ItemID);

	// ItemDetail 보이기
	ItemDetail->SetVisibility(ESlateVisibility::Visible);

	// 아이템 이미지 설정
	if (ItemDetailImage && ItemData.Icon)
	{
		ItemDetailImage->SetBrushFromTexture(ItemData.Icon);
	}

	// 아이템 수량 설정
	if (ItemDetailQuantity)
	{
		FString QuantityText = FString::FromInt(ItemQuantity);
		ItemDetailQuantity->SetText(FText::FromString(QuantityText));
	}

	// 아이템 이름 설정
	if (ItemDetailItemName)
	{
		ItemDetailItemName->SetText(FText::FromString(ItemData.ItemName));
	}

	// 아이템 설명 설정
	if (ItemDetailItemDetail)
	{
		ItemDetailItemDetail->SetText(FText::FromString(ItemData.Description));
	}

	// 추가 정보 텍스트들 (현재는 비워둠)
	if (ItemDetailEx1)
	{
		ItemDetailEx1->SetText(FText::FromString(TEXT("")));
	}

	if (ItemDetailEx2)
	{
		ItemDetailEx2->SetText(FText::FromString(TEXT("")));
	}
}

void USellShop::HideItemDetail()
{
	if (ItemDetail)
	{
		ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	}
}
