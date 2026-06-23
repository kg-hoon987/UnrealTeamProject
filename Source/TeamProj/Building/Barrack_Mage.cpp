// Fill out your copyright notice in the Description page of Project Settings.


#include "Barrack_Mage.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/Barrack_MageInfo.h"
#include "ProductionCompo/BPDComponent_Barrack.h"

ABarrack_Mage::ABarrack_Mage()
{
	ClassId = 20;
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Barrack/W_BarrackBuilding_Mage.W_BarrackBuilding_Mage_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ABarrack_Mage::BeginPlay()
{
	Super::BeginPlay();
	Cast<UBPDComponent_Barrack>(ProductionComp)->SetClassId(ClassId);
}

void ABarrack_Mage::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UBarrack_MageInfo* BarrackMageUI = Cast<UBarrack_MageInfo>(BuildingUI))
			{
				BarrackMageUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}

void ABarrack_Mage::PreInit()
{
	SetDataTable(Path);
}

