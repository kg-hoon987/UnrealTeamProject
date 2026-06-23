// Fill out your copyright notice in the Description page of Project Settings.

#include "PurchaseShop.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"
#include "../../Shop/ShopComponent.h"
#include "../../Shop/ShopStruct.h"
#include "../../Item/ItemComponent.h"
#include "../../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"

UPurchaseShop::UPurchaseShop(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SlotCounts.Init(0, 5);
	SlotPrices.Init(0, 5);
	SlotOriginalPrices.Init(0, 5);
	SlotStacks.Init(0, 5);
	SlotItemIDs.Init(NAME_None, 5);
	SlotHasDiscount.Init(false, 5);
	DefaultItemSpriteColors.Init(FLinearColor::White, 5);
	
	CachedShopComponent = nullptr;
	CachedItemComponent = nullptr;
}

void UPurchaseShop::NativeConstruct()
{
	Super::NativeConstruct();

	if (PurchaseButton)
	{
		PurchaseButton->OnClicked.AddDynamic(this, &UPurchaseShop::OnPurchaseButtonClicked);
	}

	if (RefreshButton)
	{
		RefreshButton->OnClicked.AddDynamic(this, &UPurchaseShop::OnRefreshButtonClicked);
	}

	if (Slot1Down)
	{
		Slot1Down->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot1DownClicked);
	}
	if (Slot1Up)
	{
		Slot1Up->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot1UpClicked);
	}

	if (Slot2Down)
	{
		Slot2Down->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot2DownClicked);
	}
	if (Slot2Up)
	{
		Slot2Up->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot2UpClicked);
	}

	if (Slot3Down)
	{
		Slot3Down->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot3DownClicked);
	}
	if (Slot3Up)
	{
		Slot3Up->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot3UpClicked);
	}

	if (Slot4Down)
	{
		Slot4Down->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot4DownClicked);
	}
	if (Slot4Up)
	{
		Slot4Up->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot4UpClicked);
	}

	if (Slot5Down)
	{
		Slot5Down->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot5DownClicked);
	}
	if (Slot5Up)
	{
		Slot5Up->OnClicked.AddDynamic(this, &UPurchaseShop::OnSlot5UpClicked);
	}

	if (ItemSlot1Button)
	{
		ItemSlot1Button->OnHovered.AddDynamic(this, &UPurchaseShop::OnItemSlot1ButtonHovered);
		ItemSlot1Button->OnUnhovered.AddDynamic(this, &UPurchaseShop::OnItemSlot1ButtonUnhovered);
	}
	if (ItemSlot2Button)
	{
		ItemSlot2Button->OnHovered.AddDynamic(this, &UPurchaseShop::OnItemSlot2ButtonHovered);
		ItemSlot2Button->OnUnhovered.AddDynamic(this, &UPurchaseShop::OnItemSlot2ButtonUnhovered);
	}
	if (ItemSlot3Button)
	{
		ItemSlot3Button->OnHovered.AddDynamic(this, &UPurchaseShop::OnItemSlot3ButtonHovered);
		ItemSlot3Button->OnUnhovered.AddDynamic(this, &UPurchaseShop::OnItemSlot3ButtonUnhovered);
	}
	if (ItemSlot4Button)
	{
		ItemSlot4Button->OnHovered.AddDynamic(this, &UPurchaseShop::OnItemSlot4ButtonHovered);
		ItemSlot4Button->OnUnhovered.AddDynamic(this, &UPurchaseShop::OnItemSlot4ButtonUnhovered);
	}
	if (ItemSlot5Button)
	{
		ItemSlot5Button->OnHovered.AddDynamic(this, &UPurchaseShop::OnItemSlot5ButtonHovered);
		ItemSlot5Button->OnUnhovered.AddDynamic(this, &UPurchaseShop::OnItemSlot5ButtonUnhovered);
	}

	// ItemSprite 기본 색상 저장
	UImage* ItemSprites[] = { ItemSprite1, ItemSprite2, ItemSprite3, ItemSprite4, ItemSprite5 };
	for (int32 i = 0; i < 5; i++)
	{
		if (ItemSprites[i])
		{
			DefaultItemSpriteColors[i] = ItemSprites[i]->GetColorAndOpacity();
		}
	}
	
	// 처음엔 ItemDetail SizeBoxes 숨김으로
	if (ItemDetail1) ItemDetail1->SetVisibility(ESlateVisibility::Hidden);
	if (ItemDetail2) ItemDetail2->SetVisibility(ESlateVisibility::Hidden);
	if (ItemDetail3) ItemDetail3->SetVisibility(ESlateVisibility::Hidden);
	if (ItemDetail4) ItemDetail4->SetVisibility(ESlateVisibility::Hidden);
	if (ItemDetail5) ItemDetail5->SetVisibility(ESlateVisibility::Hidden);

	// 처음엔 모든 Border들을 숨김으로  //추가3
	if (Border1) Border1->SetVisibility(ESlateVisibility::Hidden);
	if (Border2) Border2->SetVisibility(ESlateVisibility::Hidden);
	if (Border3) Border3->SetVisibility(ESlateVisibility::Hidden);
	if (Border4) Border4->SetVisibility(ESlateVisibility::Hidden);
	if (Border5) Border5->SetVisibility(ESlateVisibility::Hidden);
	
	// 슬롯 값들 초기화
	for (int32 i = 0; i < 5; i++)
	{
		UpdateSlotCount(i + 1, 0);
		UpdateSlotPrice(i + 1, 0);
		UpdateSlotStack(i + 1, 0);
	}
	
	// 총 금액 초기화
	UpdateTotalPrice();
}

void UPurchaseShop::OnPurchaseButtonClicked()
{
	if (!CachedItemComponent || !CachedShopComponent)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}
	
	AMyGamePlayerState* PlayerState = Cast<AMyGamePlayerState>(PC->PlayerState);
	if (!PlayerState)
	{
		return;
	}

	// 총 구매 금액 계산
	int32 TotalCost = 0;
	for (int32 i = 0; i < 5; i++)
	{
		if (SlotCounts[i] > 0 && SlotItemIDs[i] != NAME_None)
		{
			TotalCost += SlotPrices[i] * SlotCounts[i];
		}
	}

	// 골드가 충분한가?
	if (PlayerState->Gold < TotalCost)
	{
		return; // 골드 부족
	}

	bool bAnyPurchase = false;
	
	// 각 슬롯의 수량만큼 구매 시도
	for (int32 i = 0; i < 5; i++)
	{
		if (SlotCounts[i] > 0 && SlotItemIDs[i] != NAME_None)
		{
			FName ItemID = SlotItemIDs[i];
			int32 Count = SlotCounts[i];
			
			// 플레이어의 ItemComponent에 아이템 추가
			if (CachedItemComponent->AddItemQuantity(ItemID, Count))
			{
				bAnyPurchase = true;
				
				// 상점 재고 감소
				CachedShopComponent->UpdateShopItemStock(ItemID, SlotStacks[i] - Count);
				
				// 구매 후 수량 초기화
				SlotCounts[i] = 0;
				UpdateSlotCount(i + 1, 0);
			}
		}
	}
	
	if (bAnyPurchase)
	{
		// 골드 차감
		PlayerState->Gold -= TotalCost;
		
		// 상점 데이터 새로고침 (재고 업데이트)
		RefreshShopData();
		
		// 총 가격 업데이트
		UpdateTotalPrice();

		// 구매 성공 델리게이트 호출
		OnPurchaseSuccess.Broadcast();
	}
}

void UPurchaseShop::OnRefreshButtonClicked()
{
	if (!CachedShopComponent)
	{
		return;
	}

	// 새로고침 가능한가?
	if (!CachedShopComponent->CanRefreshShop())
	{
		return;
	}

	// 새로고침 횟수 사용
	if (CachedShopComponent->UseRefreshCount())
	{
		// 상점 아이템 목록 새로고침
		CachedShopComponent->RefreshShopItems();
		
		// 상점 설정 다시 호출하여 새로운 아이템들을 슬롯에 배치
		SetupPurchaseShop(CachedShopComponent, CachedItemComponent);
		
		// 새로고침 횟수 텍스트 업데이트
		UpdateRefreshText();
	}
}

void UPurchaseShop::OnSlot1DownClicked()
{
	if (SlotCounts[0] > 0)
	{
		SlotCounts[0]--;
		UpdateSlotCount(1, SlotCounts[0]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot1UpClicked()
{
	if (SlotCounts[0] < SlotStacks[0])
	{
		// 플레이어의 아이템 컴포넌트에서 MaxQuantity 체크
		if (CachedItemComponent && SlotItemIDs[0] != NAME_None)
		{
			UItem* PlayerItem = CachedItemComponent->GetAllItem(SlotItemIDs[0]);
			if (PlayerItem)
			{
				FItemData ItemData = PlayerItem->GetItemData();
				int32 CurrentQuantity = CachedItemComponent->GetItemQuantity(SlotItemIDs[0]);
				
				// 현재 보유 수량 + 구매할 수량이 최대 수량을 초과하지 않는지 체크
				if (CurrentQuantity + SlotCounts[0] + 1 > ItemData.MaxQuantity)
				{
					return;
				}
			}
		}
		
		SlotCounts[0]++;
		UpdateSlotCount(1, SlotCounts[0]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot2DownClicked()
{
	if (SlotCounts[1] > 0)
	{
		SlotCounts[1]--;
		UpdateSlotCount(2, SlotCounts[1]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot2UpClicked()
{
	if (SlotCounts[1] < SlotStacks[1])
	{
		// 플레이어의 아이템 컴포넌트에서 MaxQuantity 체크
		if (CachedItemComponent && SlotItemIDs[1] != NAME_None)
		{
			UItem* PlayerItem = CachedItemComponent->GetAllItem(SlotItemIDs[1]);
			if (PlayerItem)
			{
				FItemData ItemData = PlayerItem->GetItemData();
				int32 CurrentQuantity = CachedItemComponent->GetItemQuantity(SlotItemIDs[1]);
				
				// 현재 보유 수량 + 구매할 수량이 최대 수량을 초과하지 않는지 체크
				if (CurrentQuantity + SlotCounts[1] + 1 > ItemData.MaxQuantity)
				{
					return;
				}
			}
		}
		
		SlotCounts[1]++;
		UpdateSlotCount(2, SlotCounts[1]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot3DownClicked()
{
	if (SlotCounts[2] > 0)
	{
		SlotCounts[2]--;
		UpdateSlotCount(3, SlotCounts[2]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot3UpClicked()
{
	if (SlotCounts[2] < SlotStacks[2])
	{
		// 플레이어의 아이템 컴포넌트에서 MaxQuantity 체크
		if (CachedItemComponent && SlotItemIDs[2] != NAME_None)
		{
			UItem* PlayerItem = CachedItemComponent->GetAllItem(SlotItemIDs[2]);
			if (PlayerItem)
			{
				FItemData ItemData = PlayerItem->GetItemData();
				int32 CurrentQuantity = CachedItemComponent->GetItemQuantity(SlotItemIDs[2]);
				
				// 현재 보유 수량 + 구매할 수량이 최대 수량을 초과하지 않는지 체크
				if (CurrentQuantity + SlotCounts[2] + 1 > ItemData.MaxQuantity)
				{
					return;
				}
			}
		}
		
		SlotCounts[2]++;
		UpdateSlotCount(3, SlotCounts[2]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot4DownClicked()
{
	if (SlotCounts[3] > 0)
	{
		SlotCounts[3]--;
		UpdateSlotCount(4, SlotCounts[3]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot4UpClicked()
{
	if (SlotCounts[3] < SlotStacks[3])
	{
		// 플레이어의 아이템 컴포넌트에서 MaxQuantity 체크
		if (CachedItemComponent && SlotItemIDs[3] != NAME_None)
		{
			UItem* PlayerItem = CachedItemComponent->GetAllItem(SlotItemIDs[3]);
			if (PlayerItem)
			{
				FItemData ItemData = PlayerItem->GetItemData();
				int32 CurrentQuantity = CachedItemComponent->GetItemQuantity(SlotItemIDs[3]);
				
				// 현재 보유 수량 + 구매할 수량이 최대 수량을 초과하지 않는지 체크
				if (CurrentQuantity + SlotCounts[3] + 1 > ItemData.MaxQuantity)
				{
					return;
				}
			}
		}
		
		SlotCounts[3]++;
		UpdateSlotCount(4, SlotCounts[3]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot5DownClicked()
{
	if (SlotCounts[4] > 0)
	{
		SlotCounts[4]--;
		UpdateSlotCount(5, SlotCounts[4]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnSlot5UpClicked()
{
	if (SlotCounts[4] < SlotStacks[4])
	{
		// 플레이어의 아이템 컴포넌트에서 MaxQuantity 체크
		if (CachedItemComponent && SlotItemIDs[4] != NAME_None)
		{
			UItem* PlayerItem = CachedItemComponent->GetAllItem(SlotItemIDs[4]);
			if (PlayerItem)
			{
				FItemData ItemData = PlayerItem->GetItemData();
				int32 CurrentQuantity = CachedItemComponent->GetItemQuantity(SlotItemIDs[4]);
				
				// 현재 보유 수량 + 구매할 수량이 최대 수량을 초과하지 않는지 체크
				if (CurrentQuantity + SlotCounts[4] + 1 > ItemData.MaxQuantity)
				{
					return;
				}
			}
		}
		
		SlotCounts[4]++;
		UpdateSlotCount(5, SlotCounts[4]);
		UpdateTotalPrice();
	}
}

void UPurchaseShop::OnItemSlot1ButtonHovered()
{
	SetItemSpriteColor(1, FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 호버시 주황색

	if (ItemDetail1)
	{
		ItemDetail1->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UPurchaseShop::OnItemSlot1ButtonUnhovered()
{
	ResetItemSpriteColor(1);  // 언호버 시 기본 색상(흰색)

	if (ItemDetail1)
	{
		ItemDetail1->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPurchaseShop::OnItemSlot2ButtonHovered()
{
	SetItemSpriteColor(2, FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 주황색

	if (ItemDetail2)
	{
		ItemDetail2->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UPurchaseShop::OnItemSlot2ButtonUnhovered()
{
	ResetItemSpriteColor(2);

	if (ItemDetail2)
	{
		ItemDetail2->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPurchaseShop::OnItemSlot3ButtonHovered()
{
	SetItemSpriteColor(3, FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 주황색

	if (ItemDetail3)
	{
		ItemDetail3->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UPurchaseShop::OnItemSlot3ButtonUnhovered()
{
	ResetItemSpriteColor(3);

	if (ItemDetail3)
	{
		ItemDetail3->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPurchaseShop::OnItemSlot4ButtonHovered()
{
	SetItemSpriteColor(4, FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 주황색

	if (ItemDetail4)
	{
		ItemDetail4->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UPurchaseShop::OnItemSlot4ButtonUnhovered()
{
	ResetItemSpriteColor(4);

	if (ItemDetail4)
	{
		ItemDetail4->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPurchaseShop::OnItemSlot5ButtonHovered()
{
	SetItemSpriteColor(5, FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 주황색

	if (ItemDetail5)
	{
		ItemDetail5->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UPurchaseShop::OnItemSlot5ButtonUnhovered()
{
	ResetItemSpriteColor(5);

	if (ItemDetail5)
	{
		ItemDetail5->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UPurchaseShop::UpdateSlotCount(int32 SlotIndex, int32 NewCount)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	// 0~재고 수량 범위 내에서만 Count 설정
	int32 MaxStack = SlotStacks[SlotIndex - 1];
	NewCount = FMath::Clamp(NewCount, 0, MaxStack);
	
	SlotCounts[SlotIndex - 1] = NewCount;
	
	switch (SlotIndex)
	{
	case 1: if (Slot1Count) Slot1Count->SetText(FText::FromString(FString::FromInt(NewCount))); break;
	case 2: if (Slot2Count) Slot2Count->SetText(FText::FromString(FString::FromInt(NewCount))); break;
	case 3: if (Slot3Count) Slot3Count->SetText(FText::FromString(FString::FromInt(NewCount))); break;
	case 4: if (Slot4Count) Slot4Count->SetText(FText::FromString(FString::FromInt(NewCount))); break;
	case 5: if (Slot5Count) Slot5Count->SetText(FText::FromString(FString::FromInt(NewCount))); break;
	}
}

void UPurchaseShop::UpdateSlotPrice(int32 SlotIndex, int32 NewPrice)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	SlotPrices[SlotIndex - 1] = NewPrice;
	
	// 할인 여부에 따라 가격 텍스트 결정
	FString PriceText;
	if (SlotHasDiscount[SlotIndex - 1])
	{
		PriceText = FString::Printf(TEXT("할인가: %d"), NewPrice);
	}
	else
	{
		PriceText = FString::Printf(TEXT("가격: %d"), NewPrice);
	}
	
	// 텍스트와 색상 모두 업데이트
	switch (SlotIndex)
	{
	case 1: 
		if (Slot1Price) 
		{
			Slot1Price->SetText(FText::FromString(PriceText));
			UpdateSlotPriceColor(1, SlotHasDiscount[0]);
		}
		break;
	case 2: 
		if (Slot2Price) 
		{
			Slot2Price->SetText(FText::FromString(PriceText));
			UpdateSlotPriceColor(2, SlotHasDiscount[1]);
		}
		break;
	case 3: 
		if (Slot3Price) 
		{
			Slot3Price->SetText(FText::FromString(PriceText));
			UpdateSlotPriceColor(3, SlotHasDiscount[2]);
		}
		break;
	case 4: 
		if (Slot4Price) 
		{
			Slot4Price->SetText(FText::FromString(PriceText));
			UpdateSlotPriceColor(4, SlotHasDiscount[3]);
		}
		break;
	case 5: 
		if (Slot5Price) 
		{
			Slot5Price->SetText(FText::FromString(PriceText));
			UpdateSlotPriceColor(5, SlotHasDiscount[4]);
		}
		break;
	}
}

void UPurchaseShop::UpdateSlotOriginalPrice(int32 SlotIndex, int32 OriginalPrice)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	SlotOriginalPrices[SlotIndex - 1] = OriginalPrice;
}

void UPurchaseShop::UpdateSlotDiscountInfo(int32 SlotIndex, bool bHasDiscount)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	SlotHasDiscount[SlotIndex - 1] = bHasDiscount;
	
	// 할인 정보가 변경되면 가격 표시도 다시 업데이트
	UpdateSlotPrice(SlotIndex, SlotPrices[SlotIndex - 1]);
}

void UPurchaseShop::UpdateSlotStack(int32 SlotIndex, int32 NewStack)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	SlotStacks[SlotIndex - 1] = NewStack;
	
	FString StackText = FString::Printf(TEXT("재고: %d"), NewStack);
	switch (SlotIndex)
	{
	case 1: if (Slot1Stack) Slot1Stack->SetText(FText::FromString(StackText)); break;
	case 2: if (Slot2Stack) Slot2Stack->SetText(FText::FromString(StackText)); break;
	case 3: if (Slot3Stack) Slot3Stack->SetText(FText::FromString(StackText)); break;
	case 4: if (Slot4Stack) Slot4Stack->SetText(FText::FromString(StackText)); break;
	case 5: if (Slot5Stack) Slot5Stack->SetText(FText::FromString(StackText)); break;
	}
}

void UPurchaseShop::UpdateSlotPriceColor(int32 SlotIndex, bool bIsDiscount)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	FSlateColor TextColor = bIsDiscount ? FSlateColor(FLinearColor::Red) : FSlateColor(FLinearColor::White);
	
	switch (SlotIndex)
	{
	case 1: if (Slot1Price) Slot1Price->SetColorAndOpacity(TextColor); break;
	case 2: if (Slot2Price) Slot2Price->SetColorAndOpacity(TextColor); break;
	case 3: if (Slot3Price) Slot3Price->SetColorAndOpacity(TextColor); break;
	case 4: if (Slot4Price) Slot4Price->SetColorAndOpacity(TextColor); break;
	case 5: if (Slot5Price) Slot5Price->SetColorAndOpacity(TextColor); break;
	}
}


void UPurchaseShop::UpdateTotalPrice()
{
	int32 TotalPrice = 0;
	
	// 모든 슬롯의 Price * Count 합계 계산
	for (int32 i = 0; i < 5; i++)
	{
		TotalPrice += SlotPrices[i] * SlotCounts[i];
	}
	
	// PurchaseText 업데이트
	if (PurchaseText)
	{
		FString TotalText = FString::Printf(TEXT("총 금액: %d"), TotalPrice);
		PurchaseText->SetText(FText::FromString(TotalText));
	}
}

void UPurchaseShop::SetupPurchaseShop(UShopComponent* ShopComponent, UItemComponent* ItemComponent)
{
	if (!ShopComponent)
	{
		return;
	}

	if (!ItemComponent)
	{
		return;
	}

	CachedShopComponent = ShopComponent;
	CachedItemComponent = ItemComponent;

	// ShopComponent에 플레이어의 ItemComponent 설정 (리롤 시 최신 아이템 데이터 사용)
	CachedShopComponent->SetPlayerItemComponent(ItemComponent);

	// ItemComponent의 OnItemDataChanged 델리게이트 바인딩
	BindToItemDataChanged();
	
	// 상점 아이템들을 슬롯에 배치
	TArray<UShopItem*> ShopItems = ShopComponent->GetAllShopItems();
	
	// 모든 슬롯 초기화
	for (int32 i = 0; i < 5; i++)
	{
		SlotItemIDs[i] = NAME_None;
		SlotCounts[i] = 0;
		SlotPrices[i] = 0;
		SlotOriginalPrices[i] = 0;
		SlotStacks[i] = 0;
		SlotHasDiscount[i] = false;
		UpdateSlotCount(i + 1, 0);
		UpdateSlotPrice(i + 1, 0);
		UpdateSlotStack(i + 1, 0);
	}
	
	// 상점 아이템들을 슬롯에 배치 (실제 아이템 개수와 UI 슬롯 중 작은 값)
	int32 MaxSlots = FMath::Min(5, FMath::Min(ShopItems.Num(), ShopComponent->GetShopSlotCount()));
	
	// ShopItems 개수에 따라 Border 가시성 설정
	SetBorderVisibility(ShopItems.Num());  //추가3
	
	// ShopItems 개수에 따라 ItemSlot 버튼 호버 설정
	SetItemSlotButtonHoverable(ShopItems.Num());  //추가3
	
	for (int32 i = 0; i < MaxSlots; i++)
	{
		UShopItem* ShopItem = ShopItems[i];
		if (!ShopItem) 
		{
			continue;
		}
		
		FShopItemData ItemData = ShopItem->GetShopItemData();
		
		// 슬롯에 아이템 정보 설정
		SlotItemIDs[i] = ItemData.ItemID;
		
		// 가격과 재고를 배열과 UI에 모두 설정
		int32 CurrentPrice = ShopItem->GetCurrentPrice();
		int32 OriginalPrice = ItemData.Price;
		int32 CurrentStock = ShopItem->GetCurrentStock();
		bool bHasDiscount = ItemData.DiscountInfo.bIsActive && ItemData.DiscountInfo.DiscountAmount > 0;
		
		SlotPrices[i] = CurrentPrice;
		SlotOriginalPrices[i] = OriginalPrice;
		SlotStacks[i] = CurrentStock;
		SlotHasDiscount[i] = bHasDiscount;
		
		UpdateSlotPrice(i + 1, CurrentPrice);
		UpdateSlotOriginalPrice(i + 1, OriginalPrice);
		UpdateSlotDiscountInfo(i + 1, bHasDiscount);
		UpdateSlotStack(i + 1, CurrentStock);
		
		// 아이템 이미지와 텍스트 설정
		UpdateItemSlotImage(i + 1, ItemData.Icon);
		UpdateItemDetailImage(i + 1, ItemData.Icon);
		UpdateItemDetailName(i + 1, ItemData.ItemName);
		UpdateItemDetailText(i + 1, ItemData.Description);
	}
	
	// 총 가격 업데이트
	UpdateTotalPrice();
	
	// 새로고침 횟수 텍스트 업데이트
	UpdateRefreshText();
}

void UPurchaseShop::RefreshShopData()
{
	if (!CachedShopComponent)
	{
		return;
	}

	// 각 슬롯의 재고와 가격 업뎃
	for (int32 i = 0; i < 5; i++)
	{
		if (SlotItemIDs[i] != NAME_None)
		{
			FName ItemID = SlotItemIDs[i];
			
		// 현재 가격과 재고 가져오기
		int32 CurrentPrice = CachedShopComponent->GetShopItemPrice(ItemID);
		int32 CurrentStock = CachedShopComponent->GetShopItemStock(ItemID);
		
		// 아이템 정보 업뎃 (아이콘, 이름, 설명, 할인 정보)
		UShopItem* ShopItem = CachedShopComponent->GetShopItem(ItemID);
		if (ShopItem)
		{
			FShopItemData ItemData = ShopItem->GetShopItemData();
			int32 OriginalPrice = ItemData.Price;
			bool bHasDiscount = ItemData.DiscountInfo.bIsActive && ItemData.DiscountInfo.DiscountAmount > 0;
			
			// 배열과 UI 모두 업뎃
			SlotPrices[i] = CurrentPrice;
			SlotOriginalPrices[i] = OriginalPrice;
			SlotStacks[i] = CurrentStock;
			SlotHasDiscount[i] = bHasDiscount;
			
			UpdateSlotPrice(i + 1, CurrentPrice);
			UpdateSlotOriginalPrice(i + 1, OriginalPrice);
			UpdateSlotDiscountInfo(i + 1, bHasDiscount);
			UpdateSlotStack(i + 1, CurrentStock);
			
			// 아이템 정보도 업데이트 (아이콘, 이름, 설명)
			UpdateItemSlotImage(i + 1, ItemData.Icon);
			UpdateItemDetailImage(i + 1, ItemData.Icon);
			UpdateItemDetailName(i + 1, ItemData.ItemName);
			UpdateItemDetailText(i + 1, ItemData.Description);
		}
		else
		{
			// ShopItem이 없으면 기본 업데이트만
			SlotPrices[i] = CurrentPrice;
			SlotStacks[i] = CurrentStock;
			
			UpdateSlotPrice(i + 1, CurrentPrice);
			UpdateSlotStack(i + 1, CurrentStock);
		}
			
			// 현재 수량이 재고를 초과하면 조정
			if (SlotCounts[i] > CurrentStock)
			{
				SlotCounts[i] = CurrentStock;
				UpdateSlotCount(i + 1, CurrentStock);
			}
		}
	}
	
	// 총 가격 업데이트
	UpdateTotalPrice();
}


void UPurchaseShop::UpdateItemSlotImage(int32 SlotIndex, UTexture2D* Icon)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	switch (SlotIndex)
	{
	case 1: if (ItemSlot1Image && Icon) ItemSlot1Image->SetBrushFromTexture(Icon); break;
	case 2: if (ItemSlot2Image && Icon) ItemSlot2Image->SetBrushFromTexture(Icon); break;
	case 3: if (ItemSlot3Image && Icon) ItemSlot3Image->SetBrushFromTexture(Icon); break;
	case 4: if (ItemSlot4Image && Icon) ItemSlot4Image->SetBrushFromTexture(Icon); break;
	case 5: if (ItemSlot5Image && Icon) ItemSlot5Image->SetBrushFromTexture(Icon); break;
	}
}

void UPurchaseShop::UpdateItemDetailImage(int32 SlotIndex, UTexture2D* Icon)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	switch (SlotIndex)
	{
	case 1: if (ItemDetail1Image && Icon) ItemDetail1Image->SetBrushFromTexture(Icon); break;
	case 2: if (ItemDetail2Image && Icon) ItemDetail2Image->SetBrushFromTexture(Icon); break;
	case 3: if (ItemDetail3Image && Icon) ItemDetail3Image->SetBrushFromTexture(Icon); break;
	case 4: if (ItemDetail4Image && Icon) ItemDetail4Image->SetBrushFromTexture(Icon); break;
	case 5: if (ItemDetail5Image && Icon) ItemDetail5Image->SetBrushFromTexture(Icon); break;
	}
}

void UPurchaseShop::UpdateItemDetailName(int32 SlotIndex, const FString& ItemName)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	switch (SlotIndex)
	{
	case 1: if (ItemDetail1ItemName) ItemDetail1ItemName->SetText(FText::FromString(ItemName)); break;
	case 2: if (ItemDetail2ItemName) ItemDetail2ItemName->SetText(FText::FromString(ItemName)); break;
	case 3: if (ItemDetail3ItemName) ItemDetail3ItemName->SetText(FText::FromString(ItemName)); break;
	case 4: if (ItemDetail4ItemName) ItemDetail4ItemName->SetText(FText::FromString(ItemName)); break;
	case 5: if (ItemDetail5ItemName) ItemDetail5ItemName->SetText(FText::FromString(ItemName)); break;
	}
}

void UPurchaseShop::UpdateItemDetailText(int32 SlotIndex, const FString& Description)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	switch (SlotIndex)
	{
	case 1: if (ItemDetail1ItemDetail) ItemDetail1ItemDetail->SetText(FText::FromString(Description)); break;
	case 2: if (ItemDetail2ItemDetail) ItemDetail2ItemDetail->SetText(FText::FromString(Description)); break;
	case 3: if (ItemDetail3ItemDetail) ItemDetail3ItemDetail->SetText(FText::FromString(Description)); break;
	case 4: if (ItemDetail4ItemDetail) ItemDetail4ItemDetail->SetText(FText::FromString(Description)); break;
	case 5: if (ItemDetail5ItemDetail) ItemDetail5ItemDetail->SetText(FText::FromString(Description)); break;
	}
}

void UPurchaseShop::SetItemSpriteColor(int32 SlotIndex, FLinearColor Color)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	UImage* ItemSprites[] = { ItemSprite1, ItemSprite2, ItemSprite3, ItemSprite4, ItemSprite5 };
	if (ItemSprites[SlotIndex - 1])
	{
		ItemSprites[SlotIndex - 1]->SetColorAndOpacity(Color);
	}
}

void UPurchaseShop::ResetItemSpriteColor(int32 SlotIndex)
{
	if (SlotIndex < 1 || SlotIndex > 5) return;

	UImage* ItemSprites[] = { ItemSprite1, ItemSprite2, ItemSprite3, ItemSprite4, ItemSprite5 };
	if (ItemSprites[SlotIndex - 1])
	{
		ItemSprites[SlotIndex - 1]->SetColorAndOpacity(DefaultItemSpriteColors[SlotIndex - 1]);
	}
}

void UPurchaseShop::UpdateRefreshText()
{
	if (!CachedShopComponent || !RefreshText)
	{
		return;
	}

	int32 RefreshCount = CachedShopComponent->GetRefreshCount();
	FString RefreshTextString = FString::FromInt(RefreshCount);
	RefreshText->SetText(FText::FromString(RefreshTextString));
}

void UPurchaseShop::BindToItemDataChanged()
{
	if (CachedItemComponent)
	{
		// 기존 바인딩 해제
		CachedItemComponent->OnItemDataChanged.RemoveDynamic(this, &UPurchaseShop::OnItemDataChanged);

		// 새로운 바인딩
		CachedItemComponent->OnItemDataChanged.AddDynamic(this, &UPurchaseShop::OnItemDataChanged);
	}
}

void UPurchaseShop::OnItemDataChanged(FName ItemID)
{
	if (!CachedItemComponent || !CachedShopComponent)
	{
		return;
	}

	// 해당 아이템의 새로운 데이터 가져오기
	UItem* Item = CachedItemComponent->GetAllItem(ItemID);
	if (Item)
	{
		FItemData NewItemData = Item->GetItemData();

		// ShopComponent를 통해 상점 데이터 동기화
		CachedShopComponent->SyncShopItemData(ItemID, NewItemData);
	}

	// 아이템 데이터가 변경되었으므로 상점 데이터 새로고침
	RefreshShopData();
}

void UPurchaseShop::SetBorderVisibility(int32 ItemCount)  //추가3
{
	// 모든 Border를 먼저 숨김
	if (Border1) Border1->SetVisibility(ESlateVisibility::Visible);
	if (Border2) Border2->SetVisibility(ESlateVisibility::Visible);
	if (Border3) Border3->SetVisibility(ESlateVisibility::Visible);
	if (Border4) Border4->SetVisibility(ESlateVisibility::Visible);
	if (Border5) Border5->SetVisibility(ESlateVisibility::Visible);

	// ItemCount에 따라 Border들을 visible로 설정
	if (ItemCount >= 1 && Border1) Border1->SetVisibility(ESlateVisibility::Hidden);
	if (ItemCount >= 2 && Border2) Border2->SetVisibility(ESlateVisibility::Hidden);
	if (ItemCount >= 3 && Border3) Border3->SetVisibility(ESlateVisibility::Hidden);
	if (ItemCount >= 4 && Border4) Border4->SetVisibility(ESlateVisibility::Hidden);
	if (ItemCount >= 5 && Border5) Border5->SetVisibility(ESlateVisibility::Hidden);
}

void UPurchaseShop::SetItemSlotButtonHoverable(int32 ItemCount)  //추가3
{
	// 모든 ItemSlot 버튼들을 먼저 비활성화
	if (ItemSlot1Button) ItemSlot1Button->SetIsEnabled(false);
	if (ItemSlot2Button) ItemSlot2Button->SetIsEnabled(false);
	if (ItemSlot3Button) ItemSlot3Button->SetIsEnabled(false);
	if (ItemSlot4Button) ItemSlot4Button->SetIsEnabled(false);
	if (ItemSlot5Button) ItemSlot5Button->SetIsEnabled(false);

	// ItemCount에 따라 해당하는 버튼들만 활성화
	if (ItemCount >= 1 && ItemSlot1Button) ItemSlot1Button->SetIsEnabled(true);
	if (ItemCount >= 2 && ItemSlot2Button) ItemSlot2Button->SetIsEnabled(true);
	if (ItemCount >= 3 && ItemSlot3Button) ItemSlot3Button->SetIsEnabled(true);
	if (ItemCount >= 4 && ItemSlot4Button) ItemSlot4Button->SetIsEnabled(true);
	if (ItemCount >= 5 && ItemSlot5Button) ItemSlot5Button->SetIsEnabled(true);
}