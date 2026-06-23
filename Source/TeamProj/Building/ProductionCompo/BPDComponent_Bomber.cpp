// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDComponent_Bomber.h"
#include "Kismet/GameplayStatics.h"
#include "../../MyGamePlayerState.h"

UBPDComponent_Bomber::UBPDComponent_Bomber()
{
	OwnEffect = { EBuildingEffectType::Bomber_Explosion };
}

void UBPDComponent_Bomber::InitChild()
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

void UBPDComponent_Bomber::RegisterEffect(const FBuildingEffect& Effect)
{
	FGLBuildingEF myGlef;
	myGlef.SourceBuilding = *this->GetName();
	myGlef.Type = Effect.Type;
	myGlef.targetTeams = Effect.targetTeams;
	myGlef.OnOffEf = true;
	myGlef.Damage = Effect.intValue;
	myGlef.Radius = Effect.floatValue_1;

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	Ps->BuildingCom->RegisterGlobalEf(myGlef);
}

void UBPDComponent_Bomber::UnRegisterEffect(const FBuildingEffect& Effect)
{
	FGLBuildingEF myGlef;
	myGlef.SourceBuilding = *this->GetName();
	myGlef.Type = Effect.Type;
	myGlef.targetTeams = Effect.targetTeams;
	myGlef.OnOffEf = true;
	myGlef.Damage = Effect.intValue;
	myGlef.Radius = Effect.floatValue_1;

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	Ps->BuildingCom->UnRegisterGlobalEf(myGlef, *this->GetName());
}
