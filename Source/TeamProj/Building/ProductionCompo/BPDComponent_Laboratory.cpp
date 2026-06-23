// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDComponent_Laboratory.h"
#include "Kismet/GameplayStatics.h"
#include "../../MyGamePlayerState.h"

UBPDComponent_Laboratory::UBPDComponent_Laboratory()
{
	OwnEffect = { EBuildingEffectType::Laboratory_HPRegenPerSec };
}

void UBPDComponent_Laboratory::InitChild()
{
	for (FBuildingEffect ef : Effects)
	{
		if (CheckEffect(ef))
		{
			OnActivate(ef);
		}
	}
}

void UBPDComponent_Laboratory::RegisterEffect(const FBuildingEffect& Effect)
{
	FStatModifier Lab_HPRegen;
	// ¸®Á¨ÀÌ ¾ø¾î?

	FGLBuildingEF myef;
	myef.SourceBuilding = *this->GetName();
	myef.Type = Effect.Type;
	myef.targetTeams = Effect.targetTeams;
	myef.OnOffEf = true;
	myef.Damage = Effect.intValue;

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	Ps->BuildingCom->RegisterGlobalEf(myef);

}

void UBPDComponent_Laboratory::UnRegisterEffect(const FBuildingEffect& Effect)
{
	FStatModifier Lab_HPRegen;
	// ¸®Á¨ÀÌ ¾ø¾î?

	FGLBuildingEF myef;
	myef.SourceBuilding = *this->GetName();
	myef.Type = Effect.Type;
	myef.targetTeams = Effect.targetTeams;
	myef.StatEf = Lab_HPRegen;

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	Ps->BuildingCom->UnRegisterGlobalEf(myef, *this->GetName());
}
