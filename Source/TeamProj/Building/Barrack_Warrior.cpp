// Fill out your copyright notice in the Description page of Project Settings.


#include "Barrack_Warrior.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/Barrack_WarriorInfo.h"
#include "ProductionCompo/BPDComponent_Barrack.h"

ABarrack_Warrior::ABarrack_Warrior()
{
	ClassId = 0;
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Barrack/W_BarrackBuilding_Warrior.W_BarrackBuilding_Warrior_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ABarrack_Warrior::BeginPlay()
{
	Super::BeginPlay();
	Cast<UBPDComponent_Barrack>(ProductionComp)->SetClassId(ClassId);
}

void ABarrack_Warrior::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UBarrack_WarriorInfo* BarrackWarriorUI = Cast<UBarrack_WarriorInfo>(BuildingUI))
			{
				BarrackWarriorUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}

void ABarrack_Warrior::PreInit()
{
	SetDataTable(Path);
}
