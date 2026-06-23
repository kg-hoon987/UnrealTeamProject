// Fill out your copyright notice in the Description page of Project Settings.


#include "Barrack_Archer.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/Barrack_ArcherInfo.h"
#include "ProductionCompo/BPDComponent_Barrack.h"

ABarrack_Archer::ABarrack_Archer()
{
	ClassId = 10;
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Barrack/W_BarrackBuilding_Archer.W_BarrackBuilding_Archer_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ABarrack_Archer::BeginPlay()
{
	Super::BeginPlay();

	Cast<UBPDComponent_Barrack>(ProductionComp)->SetClassId(ClassId);
}

void ABarrack_Archer::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UBarrack_ArcherInfo* BarrackArcherUI = Cast<UBarrack_ArcherInfo>(BuildingUI))
			{
				BarrackArcherUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}

void ABarrack_Archer::PreInit()
{
	SetDataTable(Path);
}
