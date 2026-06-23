// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "ItemStore.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AItemStore : public ABuildingBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_ItemShop.DT_ItemShop"; // 데이터테이블 경로 넣기

	UPROPERTY(BlueprintReadOnly)
	class UShopComponent* ShopComp;

	UPROPERTY(BlueprintReadOnly)
	bool bOpenShop;

	virtual void PreInit() override;
	virtual void PostInit() override;


public:
	AItemStore();
	virtual void OpenUI() override;
	
	UFUNCTION(BlueprintCallable)
	void LevelUpShopUpdate();

	UFUNCTION(BlueprintCallable)
	bool getOpenShop() { return bOpenShop; }
};
