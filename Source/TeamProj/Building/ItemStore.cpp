// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemStore.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/ItemShopInfo.h"
#include "../Shop/ShopComponent.h"
#include "ProductionCompo/BPDCompnent_ItemShop.h"

AItemStore::AItemStore()
{
	ProductionComp = CreateDefaultSubobject<UBPDCompnent_ItemShop>(TEXT("ProductionComp"));
	static ConstructorHelpers::FClassFinder<UUserWidget> UI(TEXT("/Game/Widget/UI/Shop/W_ItemShopBuilding.W_ItemShopBuilding_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
	ShopComp = CreateDefaultSubobject<UShopComponent>(TEXT("ShopComp"));
	bOpenShop = false;
}

void AItemStore::PreInit()
{
	SetDataTable(Path);

	if (CurrentLevel >= 1)
	{
		if (FMath::RandRange(0, 100) < GetRow(GetCurrentLevel())->buildingEffects[0].floatValue_1)
		{
			bOpenShop = true;
			ShopComp->SetupShopComp(GetRow(GetCurrentLevel())->buildingEffects[0].intValue, GetRow(GetCurrentLevel())->buildingEffects[0].floatValue_2, GetRow(GetCurrentLevel())->buildingEffects[0].intValue);
			ShopComp->RefreshShopItems();

		}
		else
			bOpenShop = false;
	}
	else
		bOpenShop = false;
}

void AItemStore::PostInit()
{
}

void AItemStore::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UItemShopInfo* ShopUI = Cast<UItemShopInfo>(BuildingUI))
			{
				ShopUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}
	}
}

void AItemStore::LevelUpShopUpdate()
{
	if (CurrentLevel >= 1)
	{
		if (FMath::RandRange(0, 100) < GetRow(GetCurrentLevel())->buildingEffects[0].floatValue_1)
		{
			bOpenShop = true;
			ShopComp->SetupShopComp(GetRow(GetCurrentLevel())->buildingEffects[0].intValue, GetRow(GetCurrentLevel())->buildingEffects[0].floatValue_2, GetRow(GetCurrentLevel())->buildingEffects[0].intValue);
			ShopComp->RefreshShopItems();

		}
		else
			bOpenShop = false;
	}
	else
		bOpenShop = false;
}
