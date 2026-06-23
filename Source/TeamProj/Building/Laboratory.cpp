// Fill out your copyright notice in the Description page of Project Settings.


#include "Laboratory.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/LaboratoryInfo.h"
#include "ProductionCompo/BPDComponent_Laboratory.h"

ALaboratory::ALaboratory()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_Laboratory>(TEXT("ProductionComp"));
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Laboratory/W_Laboratory.W_Laboratory_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ALaboratory::PreInit()
{
	SetDataTable(Path);
}

void ALaboratory::PostInit()
{
}

void ALaboratory::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (ULaboratoryInfo* LaboratoryUI = Cast<ULaboratoryInfo>(BuildingUI))
			{
				LaboratoryUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}
