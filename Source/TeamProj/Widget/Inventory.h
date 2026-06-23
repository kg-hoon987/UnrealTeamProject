#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Button.h"
#include "Inventory.generated.h"

class UItemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryClose);

/**
 * 인벤토리 위젯 클래스
 */
UCLASS()
class TEAMPROJ_API UInventory : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventory(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

public:
	// 이벤트 디스패처
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryClose OnInventoryClose;

	// 인벤토리 데이터 설정 함수
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetupInventory(UItemComponent* ItemComponent);

	// 아이템 슬롯 클릭 이벤트
	UFUNCTION()
	void ItemSlotClicked(FName ItemID);

	// ItemComponent의 OnItemDataChanged 델리게이트 바인딩
	void BindToItemDataChanged();

	// ItemComponent의 OnItemQuantityChanged 델리게이트 바인딩
	void BindToItemQuantityChanged();

	// 아이템 데이터 변경 시 호출되는 함수
	UFUNCTION()
	void OnItemDataChanged(FName ItemID);

	// 아이템 수량 변경 시 호출되는 함수
	UFUNCTION()
	void OnItemQuantityChanged(FName ItemID);

protected:
	// 위젯 바인딩
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* ItemSlotUniformGridPanel;


private:
	// 아이템 컴포넌트 참조
	UPROPERTY()
	UItemComponent* CachedItemComponent;
};
