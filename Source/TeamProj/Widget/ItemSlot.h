#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "ItemSlot.generated.h"

class UItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotClicked, FName, ItemID);

UCLASS()
class TEAMPROJ_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlotButton;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSlotImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemSprite;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemSlotQuantity;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetailImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailQuantity;

	// Quantity Text Visibility Controls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot Visibility")
	bool bShowSlotQuantity = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot Visibility")
	bool bShowDetailQuantity = true;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailItemDetail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot")
	bool EnableClick = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot", meta = (ExposeOnSpawn = true))
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot", meta = (ExposeOnSpawn = true))
	int32 ItemQuantity = 0;

	UPROPERTY()
	UItemComponent* CachedItemComponent;

	// 기본 색상 저장
	UPROPERTY()
	FLinearColor DefaultItemSpriteColor = FLinearColor::White;

public:
	UPROPERTY(BlueprintAssignable, Category = "Item Slot")
	FOnItemSlotClicked OnItemSlotClicked;

	UFUNCTION(BlueprintCallable, Category = "Item Slot")
	void SetupItemSlot(UItemComponent* ItemComponent);

	UFUNCTION(BlueprintCallable, Category = "Item Slot")
	void SetItemSlotData(FName NewItemID, int32 NewQuantity);

	UFUNCTION(BlueprintCallable, Category = "Item Slot")
	void ChangeQuantity(FName ChangedItemID);

	// ItemSprite 색상 변경 함수들
	UFUNCTION(BlueprintCallable, Category = "Item Slot")
	void SetItemSpriteColor(FLinearColor Color);

	UFUNCTION(BlueprintCallable, Category = "Item Slot")
	void ResetItemSpriteColor();

	// ItemID 반환 함수
	UFUNCTION(BlueprintCallable, Category = "Item Slot")
	FName GetItemID() const { return ItemID; }

protected:
	UFUNCTION()
	void OnItemSlotButtonHovered();

	UFUNCTION()
	void OnItemSlotButtonUnhovered();

	UFUNCTION()
	void OnItemSlotButtonClicked();

	UFUNCTION()
	void OnItemSlotButtonPressed();

	UFUNCTION(BlueprintCallable)
	bool ButtonEnable();
};
