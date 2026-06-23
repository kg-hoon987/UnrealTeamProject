// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "SellShop.generated.h"

class UItemComponent;
class USellItemSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSellSuccess);  //판매 성공 델리게이트

UCLASS()
class TEAMPROJ_API USellShop : public UUserWidget
{
	GENERATED_BODY()

public:
	USellShop(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable)
	FOnSellSuccess OnSellSuccess;


protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* InButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OutButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SellButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SellText;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* SellSlotUniformGridPanel;

	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ItemSlotUniformGridPanel;

	UPROPERTY(meta = (BindWidget))
	class USizeBox* ItemDetail;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemDetailImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailQuantity;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailEx1;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailEx2;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailItemName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemDetailItemDetail;

	UFUNCTION()
	void OnInButtonClicked();

	UFUNCTION()
	void OnOutButtonClicked();

	UFUNCTION()
	void OnSellButtonClicked();

	// 초기 세팅
	UFUNCTION(BlueprintCallable)
	void SetupSellShop(UItemComponent* ItemComponent);

	// 판매 버튼 누르면 인벤토리 수량 갱신해야하니까
	UFUNCTION(BlueprintCallable)
	void RefreshSellData();

	UFUNCTION()
	void OnItemSlotClicked(FName ItemID);

	UFUNCTION(BlueprintPure, Category = "Sell Shop")
	FName GetSelectedItemID() const { return SelectedItemID; }

	// 판매 목록에서 아이템 제거
	UFUNCTION(BlueprintCallable, Category = "Sell Shop")
	void RemoveFromSellList(FName ItemID);

	UFUNCTION()
	void OnSellItemSlotClicked(FName ItemID);

private:
	UPROPERTY()
	UItemComponent* CachedItemComponent;

	// 선택된 ItemID 저장
	UPROPERTY(BlueprintReadOnly, Category = "Sell Shop", meta = (AllowPrivateAccess = "true"))
	FName SelectedItemID = NAME_None;

	// 판매 목록에 있는 아이템 슬롯들
	UPROPERTY()
	TArray<USellItemSlot*> SellItemSlots;

	// 총 판매가
	UPROPERTY(BlueprintReadOnly, Category = "Sell Shop", meta = (AllowPrivateAccess = "true"))
	int32 TotalSellPrice = 0;

	// 총 판매가 업뎃
	void UpdateTotalSellPrice();

	// 판매 슬롯들 재배치
	void RearrangeSellSlots();

	// 판매 목록 초기화
	void ClearSellList();

	// ItemComponent의 OnItemQuantityChanged 델리게이트 바인딩
	void BindToItemQuantityChanged();

	// ItemComponent의 OnItemDataChanged 델리게이트 바인딩
	void BindToItemDataChanged();

	// 아이템 수량 변경 시 호출되는 함수
	UFUNCTION()
	void OnItemQuantityChanged(FName ItemID);

	// 아이템 데이터 변경 시 호출되는 함수
	UFUNCTION()
	void OnItemDataChanged(FName ItemID);

	UFUNCTION()
	void OnSellItemSlotHovered(FName ItemID);

	UFUNCTION()
	void OnSellItemSlotUnhovered(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Sell Shop")
	void UpdateItemDetail(FName ItemID);

	UFUNCTION(BlueprintCallable, Category = "Sell Shop")
	void HideItemDetail();

};