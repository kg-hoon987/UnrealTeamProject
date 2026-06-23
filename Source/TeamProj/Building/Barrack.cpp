// Fill out your copyright notice in the Description page of Project Settings.


#include "Barrack.h"
#include "ProductionCompo/BPDComponent_Barrack.h"
ABarrack::ABarrack()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_Barrack>(TEXT("ProductionComp"));

	curIden = EIdentity::L;
}

void ABarrack::SetSpawnposList()
{
	FBoxSphereBounds bound = Mesh->GetStaticMesh()->GetBounds();

	/*Boxcollision->SetWorldLocation(bound.Origin);
	Boxcollision->SetBoxExtent(bound.BoxExtent);*/
	CurSpawnPos.Empty();
	for (int i =0;i<SpawnPosList.Num();i++)
	{
		CurSpawnPos.Add(SpawnPosList[i] + GetActorLocation());
		CurSpawnPos[i] -= FVector(bound.BoxExtent.X, 0.f, 0.f);
		CurSpawnPos[i] = GetActorRotation().RotateVector(CurSpawnPos[i]);
	//디버그용
		//DrawDebugSphere(GetWorld(), CurSpawnPos[i], 50.f, 4, FColor::Blue, false, 5.f);
	}
	// 스폰할때 
}

void ABarrack::BeginPlay()
{
	Super::BeginPlay();
	if (UObject* dt = StaticLoadObject(UDataTable::StaticClass(), nullptr, *chPath))
	{
		Cast<UBPDComponent_Barrack>(ProductionComp)->CharInfoTable = Cast<UDataTable>(dt);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("DT bad!!!!!!!!!!!!!!!!!!"));
	if (CurrentLevel >= 1)
	{
		InitIdentity();
	}

}

void ABarrack::PostApplyRow()
{
	SetSpawnposList();
	if (CurrentLevel >= 1)
	{
		InitIdentity();
	}
}

void ABarrack::InitIdentity()
{
	if (UBPDComponent_Barrack* Prod = Cast<UBPDComponent_Barrack>(ProductionComp))
	{
		Prod->InitLRCharacter(curIden);
	}
}

void ABarrack::SelectIdentity(EIdentity EIden)
{
	if (UBPDComponent_Barrack* Prod = Cast<UBPDComponent_Barrack>(ProductionComp))
	{
		
		Prod->SetAllyIdentity(EIden);
		curIden = EIden;
	}
}

void ABarrack::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Cast<UBPDComponent_Barrack>(ProductionComp)->End();
}


void ABarrack::InitSelectL()
{
	if (curIden == EIdentity::L) return;
	SelectIdentity(EIdentity::L);
}

void ABarrack::InitSelectR()
{
	if (curIden == EIdentity::R) return;
	SelectIdentity(EIdentity::R);
}


