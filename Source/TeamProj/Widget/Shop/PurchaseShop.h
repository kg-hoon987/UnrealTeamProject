// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/Border.h"  //추가3
#include "PurchaseShop.generated.h"

class UShopComponent;
class UItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPurchaseSuccess);  // 구매 성공 델리게이트

UCLASS()
class TEAMPROJ_API UPurchaseShop : public UUserWidget
{
	GENERATED_BODY()

public:
	UPurchaseShop(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FOnPurchaseSuccess OnPurchaseSuccess;

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* PurchaseButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PurchaseText;

	UPROPERTY(meta = (BindWidget))
	class UButton* RefreshButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RefreshText;

	// Slot 1
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot1Count;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot1Down;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot1Up;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot1Price;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot1Stack;

	// Slot 2
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot2Count;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot2Down;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot2Up;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot2Price;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot2Stack;

	// Slot 3
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot3Count;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot3Down;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot3Up;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot3Price;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot3Stack;

	// Slot 4
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot4Count;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot4Down;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot4Up;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot4Price;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot4Stack;

	// Slot 5
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot5Count;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot5Down;

	UPROPERTY(meta = (BindWidget))
	class UButton* Slot5Up;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot5Price;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Slot5Stack;

	// Item Detail 사이즈박스
	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail1;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail2;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail3;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail4;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail5;

	// Border 컴포넌트들
	UPROPERTY(meta = (BindWidget))
	class UBorder* Border1;  //추가3

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border2;  //추가3

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border3;  //추가3

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border4;  //추가3

	UPROPERTY(meta = (BindWidget))
	class UBorder* Border5;  //추가3

	// Item Slot 버튼
	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlot1Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlot2Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlot3Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlot4Button;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlot5Button;

	// Item Slot 이미지
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot1Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot2Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot3Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot4Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlot5Image;

	// Item Sprite 이미지
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite1;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite2;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite3;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite4;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite5;

	// Item Detail 이미지
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetail1Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetail2Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetail3Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetail4Image;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetail5Image;

	// Item Detail Item Name 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail1ItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail2ItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail3ItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail4ItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail5ItemName;

	// Item Detail Item Detail 텍스트
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail1ItemDetail;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail2ItemDetail;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail3ItemDetail;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail4ItemDetail;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetail5ItemDetail;

	UFUNCTION()
	void OnPurchaseButtonClicked();

	UFUNCTION()
	void OnRefreshButtonClicked();

	UFUNCTION()
	void OnSlot1DownClicked();

	UFUNCTION()
	void OnSlot1UpClicked();

	UFUNCTION()
	void OnSlot2DownClicked();

	UFUNCTION()
	void OnSlot2UpClicked();

	UFUNCTION()
	void OnSlot3DownClicked();

	UFUNCTION()
	void OnSlot3UpClicked();

	UFUNCTION()
	void OnSlot4DownClicked();

	UFUNCTION()
	void OnSlot4UpClicked();

	UFUNCTION()
	void OnSlot5DownClicked();

	UFUNCTION()
	void OnSlot5UpClicked();

	// Item Slot Button Hover Functions
	UFUNCTION()
	void OnItemSlot1ButtonHovered();

	UFUNCTION()
	void OnItemSlot1ButtonUnhovered();

	UFUNCTION()
	void OnItemSlot2ButtonHovered();

	UFUNCTION()
	void OnItemSlot2ButtonUnhovered();

	UFUNCTION()
	void OnItemSlot3ButtonHovered();

	UFUNCTION()
	void OnItemSlot3ButtonUnhovered();

	UFUNCTION()
	void OnItemSlot4ButtonHovered();

	UFUNCTION()
	void OnItemSlot4ButtonUnhovered();

	UFUNCTION()
	void OnItemSlot5ButtonHovered();

	UFUNCTION()
	void OnItemSlot5ButtonUnhovered();

	// Slot Management Functions
	UFUNCTION(BlueprintCallable)
	void UpdateSlotCount(int32 SlotIndex, int32 NewCount);

	UFUNCTION(BlueprintCallable)
	void UpdateSlotPrice(int32 SlotIndex, int32 NewPrice);

	UFUNCTION(BlueprintCallable)
	void UpdateSlotOriginalPrice(int32 SlotIndex, int32 OriginalPrice);

	UFUNCTION(BlueprintCallable)
	void UpdateSlotDiscountInfo(int32 SlotIndex, bool bHasDiscount);

	UFUNCTION(BlueprintCallable)
	void UpdateSlotStack(int32 SlotIndex, int32 NewStack);

	UFUNCTION(BlueprintCallable)
	void UpdateSlotPriceColor(int32 SlotIndex, bool bIsDiscount);


	UFUNCTION(BlueprintCallable)
	void UpdateTotalPrice();

	// Shop Component Integration
	UFUNCTION(BlueprintCallable)
	void SetupPurchaseShop(UShopComponent* ShopComponent, UItemComponent* ItemComponent);


	UFUNCTION(BlueprintCallable)
	void RefreshShopData();

	// Item Display Functions
	UFUNCTION(BlueprintCallable)
	void UpdateItemSlotImage(int32 SlotIndex, UTexture2D* Icon);

	UFUNCTION(BlueprintCallable)
	void UpdateItemDetailImage(int32 SlotIndex, UTexture2D* Icon);

	UFUNCTION(BlueprintCallable)
	void UpdateItemDetailName(int32 SlotIndex, const FString& ItemName);

	UFUNCTION(BlueprintCallable)
	void UpdateItemDetailText(int32 SlotIndex, const FString& Description);

	// ItemSprite 색상 변경 함수들
	UFUNCTION(BlueprintCallable, Category = "Purchase Shop")
	void SetItemSpriteColor(int32 SlotIndex, FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "Purchase Shop")
	void ResetItemSpriteColor(int32 SlotIndex);

	// 새로고침(아이템 판매명단) 횟수 업데이트 함수
	UFUNCTION(BlueprintCallable)
	void UpdateRefreshText();

	// Border 가시성 설정 함수
	UFUNCTION(BlueprintCallable)
	void SetBorderVisibility(int32 ItemCount);  //추가3

	// ItemSlot 버튼 호버 활성화/비활성화 함수
	UFUNCTION(BlueprintCallable)
	void SetItemSlotButtonHoverable(int32 ItemCount);  //추가3

	// ItemComponent의 OnItemDataChanged 델리게이트 바인딩
	void BindToItemDataChanged();

	// 아이템 데이터 변경 시 호출되는 함수
	UFUNCTION()
	void OnItemDataChanged(FName ItemID);

private:
	TArray<int32> SlotCounts;			// 선택된 수
	TArray<int32> SlotPrices;			// 할인된 가격 (현재 가격)
	TArray<int32> SlotOriginalPrices;	// 원래 가격
	TArray<int32> SlotStacks;			// 재고량
	TArray<FName> SlotItemIDs;			// 각 슬롯의 아이템 ID 저장
	TArray<bool> SlotHasDiscount;		// 할인 여부

	UPROPERTY()
	UShopComponent* CachedShopComponent;

	UPROPERTY()
	UItemComponent* CachedItemComponent;

	// 기본 색상 저장 (5개 슬롯)
	TArray<FLinearColor> DefaultItemSpriteColors;
};
