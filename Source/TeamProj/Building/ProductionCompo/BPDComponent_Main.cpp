// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDComponent_Main.h"
#include "Kismet/GameplayStatics.h"
#include "../../MyGamePlayerState.h"
UBPDComponent_Main::UBPDComponent_Main()
{
	OwnEffect = { EBuildingEffectType::Main_AddReRoll,EBuildingEffectType::Main_AddStat };
}

void UBPDComponent_Main::InitChild()
{
	//생성 시 바로 발동....
	for (FBuildingEffect ef : Effects) 
	{
		if (CheckEffect(ef))
		{
			OnActivate(ef);
		}
	}
}

void UBPDComponent_Main::ActivateEffect(const FBuildingEffect& Effect)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	// 지금은 그냥 더해주는 방식.
	Ps->Reroll += Effect.intValue;	// 리롤 소모성으로 해서 추가해줄 건지 매번 몇개의 리롤을 줄지 정해야할듯.
}

void UBPDComponent_Main::RegisterEffect(const FBuildingEffect& Effect)
{
	FStatModifier Main_AddStat;
	Main_AddStat.AddHP = Effect.floatValue_1;
	Main_AddStat.AddAtk = Effect.floatValue_2;
	Main_AddStat.AddDef = Effect.floatValue_3;

	FGLBuildingEF myGlef;
	myGlef.SourceBuilding = *this->GetName();
	myGlef.Type = Effect.Type;
	myGlef.targetTeams = Effect.targetTeams;
	myGlef.hasStatEf = true;
	myGlef.StatEf = Main_AddStat;

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	Ps->BuildingCom->RegisterGlobalEf(myGlef);
}

void UBPDComponent_Main::UnRegisterEffect(const FBuildingEffect& Effect)
{
	FStatModifier Main_AddStat;
	Main_AddStat.AddHP = Effect.floatValue_1;
	Main_AddStat.AddAtk = Effect.floatValue_2;
	Main_AddStat.AddDef = Effect.floatValue_3;

	FGLBuildingEF myGlef;
	myGlef.SourceBuilding = *this->GetName();
	myGlef.Type = Effect.Type;
	myGlef.targetTeams = Effect.targetTeams;
	myGlef.hasStatEf = true;
	myGlef.StatEf = Main_AddStat;

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	Ps->BuildingCom->UnRegisterGlobalEf(myGlef, *this->GetName());
}
