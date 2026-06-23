// Fill out your copyright notice in the Description page of Project Settings.


#include "Statue.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/StatueInfo.h"
#include "ProductionCompo/BPDComponent_Statue.h"
AStatue::AStatue()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_Statue>(TEXT("ProductionComp"));
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Statue/W_StatueBuilding.W_StatueBuilding_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void AStatue::PreInit()
{
	SetDataTable(Path);
}

void AStatue::PostInit()
{
}

void AStatue::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UStatueInfo* StatueUI = Cast<UStatueInfo>(BuildingUI))
			{
				StatueUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}
