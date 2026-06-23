// Fill out your copyright notice in the Description page of Project Settings.


#include "Bomber.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/BomberInfo.h"
#include "ProductionCompo/BPDComponent_Bomber.h"

ABomber::ABomber()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_Bomber>(TEXT("ProductionComp"));
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Bomber/W_BomberBuilding.W_BomberBuilding_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ABomber::PreInit()
{
	SetDataTable(Path);
}

void ABomber::PostInit()
{
}

void ABomber::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UBomberInfo* BomberUI = Cast<UBomberInfo>(BuildingUI))
			{
				BomberUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}
