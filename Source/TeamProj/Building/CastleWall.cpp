// Fill out your copyright notice in the Description page of Project Settings.


#include "CastleWall.h"
#include "ProductionCompo/BPDComponent_NoEffectBuilding.h"

ACastleWall::ACastleWall()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_NoEffectBuilding>(TEXT("ProductionComp"));
	DefaultMesh->SetVisibility(false);
	DefaultMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	bisDoorWall = true;
}

void ACastleWall::PreInit()
{
	SetDataTable(Path);
}
