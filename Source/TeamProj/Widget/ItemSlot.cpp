#include "ItemSlot.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../Item/ItemComponent.h"

UItemSlot::UItemSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultItemSpriteColor = FLinearColor::White;
}

void UItemSlot::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (ItemSlotButton)
	{
		ItemSlotButton->OnHovered.AddDynamic(this, &UItemSlot::OnItemSlotButtonHovered);
		ItemSlotButton->OnUnhovered.AddDynamic(this, &UItemSlot::OnItemSlotButtonUnhovered);
		ItemSlotButton->OnClicked.AddDynamic(this, &UItemSlot::OnItemSlotButtonClicked);
		ItemSlotButton->OnPressed.AddDynamic(this, &UItemSlot::OnItemSlotButtonPressed);
	}

	// ItemSprite 기본 색상 저장
	if (ItemSprite)
	{
		DefaultItemSpriteColor = ItemSprite->GetColorAndOpacity();
	}

	// 초기 설정
	if (ItemDetail)
	{
		ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemSlot::OnItemSlotButtonHovered()
{
	// 호버 시 주황색으로 변경
	SetItemSpriteColor(FLinearColor(1.0f, 0.5f, 0.0f, 1.0f)); // 주황색

	if (ItemDetail)
	{
		ItemDetail->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UItemSlot::OnItemSlotButtonUnhovered()
{
	// 언호버 시 기본 색상으로 복원
	ResetItemSpriteColor();

	if (ItemDetail)
	{
		ItemDetail->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemSlot::OnItemSlotButtonClicked()
{
	if (EnableClick)
	{
		OnItemSlotClicked.Broadcast(ItemID);
	}
}

void UItemSlot::OnItemSlotButtonPressed()
{
	// 프레스 시 빨간색으로 변경
	SetItemSpriteColor(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // 빨간색
}

bool UItemSlot::ButtonEnable()
{
	if (ItemQuantity <= 0)
		return false;
	else
		return true;
}

void UItemSlot::SetItemSpriteColor(FLinearColor Color)
{
	if (ItemSprite)
	{
		ItemSprite->SetColorAndOpacity(Color);
	}
}

void UItemSlot::ResetItemSpriteColor()
{
	if (ItemSprite)
	{
		ItemSprite->SetColorAndOpacity(DefaultItemSpriteColor);
	}
}

void UItemSlot::SetupItemSlot(UItemComponent* ItemComponent)
{
	if (!ItemComponent)
	{
		return;
	}

	// 이전 바인딩 해제
	if (CachedItemComponent)
	{
		CachedItemComponent->OnItemQuantityChanged.RemoveDynamic(this, &UItemSlot::ChangeQuantity);
	}

	// 새 아이템 컴포넌트 캐시 및 바인딩
	CachedItemComponent = ItemComponent;
	CachedItemComponent->OnItemQuantityChanged.AddDynamic(this, &UItemSlot::ChangeQuantity);

	UItem* Item = ItemComponent->GetAllItem(ItemID);
	if (!Item)
	{
		return;
	}

	FItemData ItemData = Item->GetItemData();

	// 아이템 이미지 설정
	if (ItemSlotImage && ItemData.Icon)
	{
		ItemSlotImage->SetBrushFromTexture(ItemData.Icon);
	}

	if (ItemDetailImage && ItemData.Icon)
	{
		ItemDetailImage->SetBrushFromTexture(ItemData.Icon);
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

	// 수량에 따른 EnableClick 설정
	EnableClick = (ItemQuantity > 0);


	// 수량 설정
	if (ItemSlotQuantity)
	{
		ItemSlotQuantity->SetText(FText::AsNumber(ItemQuantity));
	}

	if (ItemDetailQuantity)
	{
		ItemDetailQuantity->SetText(FText::AsNumber(ItemQuantity));
	}
}

void UItemSlot::SetItemSlotData(FName NewItemID, int32 NewQuantity)
{
	ItemID = NewItemID;
	ItemQuantity = NewQuantity;
}

void UItemSlot::ChangeQuantity(FName ChangedItemID)
{
	// 변경된 아이템이 현재 슬롯의 아이템과 같은지 확인
	if (ChangedItemID != ItemID || !CachedItemComponent)
	{
		return;
	}

	// 새로운 수량 가져오기
	ItemQuantity = CachedItemComponent->GetItemQuantity(ItemID);

	// 수량에 따른 EnableClick 설정
	EnableClick = (ItemQuantity > 0);

	// 수량 텍스트 업데이트
	if (ItemSlotQuantity)
	{
		ItemSlotQuantity->SetText(FText::AsNumber(ItemQuantity));
	}

	if (ItemDetailQuantity)
	{
		ItemDetailQuantity->SetText(FText::AsNumber(ItemQuantity));
	}
}
