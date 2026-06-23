#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "SellItemSlot.generated.h"

class UItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSellItemSlotClicked, FName, ItemID);  // 클릭 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSellItemSlotHovered, FName, ItemID);  //호버 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSellItemSlotUnhovered, FName, ItemID);  //언호버 델리게이트

UCLASS()
class TEAMPROJ_API USellItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	USellItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* SellItemButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* SellItemImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SellItemQuantity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sell Item Slot")
	FName ItemID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sell Item Slot")
	int32 ItemQuantity = 0;


	UPROPERTY()
	UItemComponent* CachedItemComponent;

	// 기본 색상
	UPROPERTY()
	FLinearColor DefaultItemSpriteColor = FLinearColor::White;

	// 인벤토리 모드 (t: 보유 수량 표시, f: 판매 수량 표시)
	UPROPERTY(BlueprintReadOnly, Category = "Sell Item Slot")
	bool bIsInventoryMode = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Sell Item Slot")
	FOnSellItemSlotClicked OnSellItemSlotClicked;

	UPROPERTY(BlueprintAssignable, Category = "Sell Item Slot")
	FOnSellItemSlotHovered OnSellItemSlotHovered;

	UPROPERTY(BlueprintAssignable, Category = "Sell Item Slot")
	FOnSellItemSlotUnhovered OnSellItemSlotUnhovered;

	// 초기 세팅
	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void SetupSellItemSlot(UItemComponent* ItemComponent, FName InItemID, int32 InQuantity);

	// UI 업데이트 위해서(SetupSellItemSlot에서 사용)
	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void SetSellItemData(FName InItemID, int32 InQuantity);

	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void UpdateQuantity(int32 NewQuantity);

	UFUNCTION(BlueprintPure, Category = "Sell Item Slot")
	FName GetItemID() const { return ItemID; }

	UFUNCTION(BlueprintPure, Category = "Sell Item Slot")
	int32 GetItemQuantity() const { return ItemQuantity; }


	// 아이템 컴포넌트의 보유 수량 확인
	UFUNCTION(BlueprintPure, Category = "Sell Item Slot")
	int32 GetAvailableQuantity() const;

	// 인벤토리 모드 설정(t: 보유 수량 표시, f: 판매 수량 표시)
	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void SetInventoryMode(bool bInInventoryMode);

	// 수량 강제 업뎃 (외부에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void ForceUpdateQuantity();

	// ItemSprite 색상 변경 함수 (Hover나 Press 될 때)
	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void SetItemSpriteColor(FLinearColor Color);

	// ItemSprite 색상 초기화 함수 (UnHover 될 때)
	UFUNCTION(BlueprintCallable, Category = "Sell Item Slot")
	void ResetItemSpriteColor();

protected:
	UFUNCTION()
	void OnSellItemButtonClicked();

	UFUNCTION()
	void OnSellItemButtonHovered();

	UFUNCTION()
	void OnSellItemButtonUnhovered();

	UFUNCTION()
	void OnSellItemButtonPressed();

	UPROPERTY(BlueprintReadOnly)
	bool ButtonEnable;

	UPROPERTY(BlueprintReadOnly)
	bool bSelected;

public:
	UFUNCTION()
	void SetDeSelected() { bSelected = false; ResetItemSpriteColor(); UE_LOG(LogTemp, Warning, TEXT("ResetSelected")); }
};
