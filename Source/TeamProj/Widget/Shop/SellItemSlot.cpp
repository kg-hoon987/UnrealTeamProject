#include "SellItemSlot.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../../Item/ItemComponent.h"

USellItemSlot::USellItemSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ItemID = NAME_None;
	ItemQuantity = 0;
	CachedItemComponent = nullptr;
	bIsInventoryMode = false;
	DefaultItemSpriteColor = FLinearColor::White;
	bSelected = false;
}

void USellItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (SellItemButton)
	{
		// 기존 델리게이트 해제 (중복 바인딩 방지)
		SellItemButton->OnClicked.RemoveDynamic(this, &USellItemSlot::OnSellItemButtonClicked);
		SellItemButton->OnHovered.RemoveDynamic(this, &USellItemSlot::OnSellItemButtonHovered);
		SellItemButton->OnUnhovered.RemoveDynamic(this, &USellItemSlot::OnSellItemButtonUnhovered);
		SellItemButton->OnPressed.RemoveDynamic(this, &USellItemSlot::OnSellItemButtonPressed);
		
		// 새로운 델리게이트 바인딩
		SellItemButton->OnClicked.AddDynamic(this, &USellItemSlot::OnSellItemButtonClicked);
		SellItemButton->OnHovered.AddDynamic(this, &USellItemSlot::OnSellItemButtonHovered);
		SellItemButton->OnUnhovered.AddDynamic(this, &USellItemSlot::OnSellItemButtonUnhovered);
		SellItemButton->OnPressed.AddDynamic(this, &USellItemSlot::OnSellItemButtonPressed);
	}

	if (ItemSprite)
	{
		DefaultItemSpriteColor = ItemSprite->GetColorAndOpacity();
	}
}

void USellItemSlot::SetupSellItemSlot(UItemComponent* ItemComponent, FName InItemID, int32 InQuantity)
{
	if (!ItemComponent)
	{
		return;
	}

	CachedItemComponent = ItemComponent;
	
	// 아이템 데이터 가져오고, 정보 입력
	UItem* Item = ItemComponent->GetAllItem(InItemID);
	if (!Item)
	{
		return;
	}

	FItemData ItemData = Item->GetItemData();
	
	ItemID = InItemID;
	ItemQuantity = InQuantity;

	// UI 업데이트
	SetSellItemData(ItemID, ItemQuantity);
}

void USellItemSlot::SetSellItemData(FName InItemID, int32 InQuantity)
{
	ItemID = InItemID;
	ItemQuantity = InQuantity;

	// QuantityText 텍스트 업데이트 (모드에 따라 다르게)
	if (SellItemQuantity)
	{
		int32 AvailableQty = GetAvailableQuantity();
		FString QuantityText;
		
		if (bIsInventoryMode)
		{
			// 보유 수량 표시
			QuantityText = FString::FromInt(AvailableQty);
		}
		else
		{
			// 판매 수량 표시
			QuantityText = FString::FromInt(ItemQuantity);
		}
		
		SellItemQuantity->SetText(FText::FromString(QuantityText));
		if (AvailableQty <= 0)
			ButtonEnable = false;
		else
			ButtonEnable = true;
	}


	// 아이템 아이콘 설정
	if (CachedItemComponent)
	{
		UItem* Item = CachedItemComponent->GetAllItem(ItemID);
		if (Item && SellItemImage)
		{
			FItemData ItemData = Item->GetItemData();
			if (ItemData.Icon)
			{
				SellItemImage->SetBrushFromTexture(ItemData.Icon);
			}
		}
	}
}

void USellItemSlot::UpdateQuantity(int32 NewQuantity)
{
	ItemQuantity = NewQuantity;
	if (SellItemQuantity)
	{
		int32 AvailableQty = GetAvailableQuantity();
		FString QuantityText;
		
		if (bIsInventoryMode)
		{
			// 보유 수량 표시
			QuantityText = FString::FromInt(AvailableQty);
		}
		else
		{
			// 판매 수량 표시
			QuantityText = FString::FromInt(ItemQuantity);
		}
		
		SellItemQuantity->SetText(FText::FromString(QuantityText));
	}
}

int32 USellItemSlot::GetAvailableQuantity() const
{
	if (CachedItemComponent)
	{
		return CachedItemComponent->GetItemQuantity(ItemID);
	}
	return 0;
}

void USellItemSlot::SetInventoryMode(bool bInInventoryMode)
{
	this->bIsInventoryMode = bInInventoryMode;
	
	// 모드 변경 시 QuantityText 업데이트
	if (SellItemQuantity)
	{
		int32 AvailableQuantity = GetAvailableQuantity();
		FString QuantityText;
		
		if (bIsInventoryMode)
		{
			// 보유 수량 표시
			QuantityText = FString::FromInt(AvailableQuantity);
		}
		else
		{
			// 판매 수량 표시
			QuantityText = FString::FromInt(ItemQuantity);
		}
		
		SellItemQuantity->SetText(FText::FromString(QuantityText));
	}
}

void USellItemSlot::ForceUpdateQuantity()
{
	if (SellItemQuantity)
	{
		int32 AvailableQty = GetAvailableQuantity();
		FString QuantityText;
		
		if (bIsInventoryMode)
		{
			// 보유 수량 표시
			QuantityText = FString::FromInt(AvailableQty);
		}
		else
		{
			// 판매 수량 표시
			QuantityText = FString::FromInt(ItemQuantity);
		}
		
		SellItemQuantity->SetText(FText::FromString(QuantityText));
		if (AvailableQty <= 0)
			ButtonEnable = false;
		else
			ButtonEnable = true;

	}
}

void USellItemSlot::OnSellItemButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("1"));
	OnSellItemSlotClicked.Broadcast(ItemID);
	SetItemSpriteColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f));
	bSelected = true;
}

void USellItemSlot::OnSellItemButtonHovered()
{
	OnSellItemSlotHovered.Broadcast(ItemID);
	UE_LOG(LogTemp, Warning, TEXT("[SelectedSlot] = %s"), (bSelected) ? TEXT("true") : TEXT("false"));
	if (bSelected)
		return;
	else
		SetItemSpriteColor(FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 호버때 주황색
}

void USellItemSlot::OnSellItemButtonPressed()
{
	SetItemSpriteColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // 누를때 빨강 (클릭이 아니라 Press임)
}



void USellItemSlot::OnSellItemButtonUnhovered()
{
	OnSellItemSlotUnhovered.Broadcast(ItemID);
	
	UE_LOG(LogTemp, Warning, TEXT("[SelectedSlot] = %s"), (bSelected) ? TEXT("true") : TEXT("false"));
	if (bSelected)
		return;
	else
		ResetItemSpriteColor();  //언호버때 다시 기본 흰색으로
}

void USellItemSlot::SetItemSpriteColor(FLinearColor Color)
{
	if (ItemSprite)
	{
		ItemSprite->SetColorAndOpacity(Color);
	}
}

void USellItemSlot::ResetItemSpriteColor()
{
	if (ItemSprite)
	{
		ItemSprite->SetColorAndOpacity(DefaultItemSpriteColor);
	}
}
