// Fill out your copyright notice in the Description page of Project Settings.


#include "Barrack_Priest.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/Barrack_PriestInfo.h"
#include "ProductionCompo/BPDComponent_Barrack.h"

ABarrack_Priest::ABarrack_Priest()
{
	ClassId = 30;
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Barrack/W_BarrackBuilding_Priest.W_BarrackBuilding_Priest_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ABarrack_Priest::BeginPlay()
{
	Super::BeginPlay();
	Cast<UBPDComponent_Barrack>(ProductionComp)->SetClassId(ClassId);
}

void ABarrack_Priest::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UBarrack_PriestInfo* BarrackPriestUI = Cast<UBarrack_PriestInfo>(BuildingUI))
			{
				BarrackPriestUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}


void ABarrack_Priest::PreInit()
{
	SetDataTable(Path);
}
