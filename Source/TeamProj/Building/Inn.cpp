// Fill out your copyright notice in the Description page of Project Settings.


#include "Inn.h"
#include "Blueprint/UserWidget.h"
#include "../Widget/UI/InnInfo.h"
#include "ProductionCompo/BPDComponent_Inn.h"

AInn::AInn()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_Inn>(TEXT("ProductionComp"));
	//widgetclass
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Inn/W_InnBuilding.W_InnBuilding_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}

}

void AInn::PreInit()
{
	SetDataTable(Path);
	//objectwidget Àû¿ë
}

void AInn::PostInit()
{
}

void AInn::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UInnInfo* InnUI = Cast<UInnInfo>(BuildingUI))
			{
				InnUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}
