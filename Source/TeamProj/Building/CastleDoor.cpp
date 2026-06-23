// Fill out your copyright notice in the Description page of Project Settings.


#include "CastleDoor.h"
#include "ProductionCompo/BPDComponent_NoEffectBuilding.h"

ACastleDoor::ACastleDoor()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_NoEffectBuilding>(TEXT("ProductionComp"));
	DefaultMesh->SetVisibility(false);
	DefaultMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bisDoorWall = true;
}

void ACastleDoor::PreInit()
{
	SetDataTable(Path);
}
