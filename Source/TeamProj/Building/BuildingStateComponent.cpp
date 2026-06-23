// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingStateComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Stage/RoundComponent.h"
#include "../MyGamePlayerState.h"
#include "BuildingBase.h"
#include "DoorNWall.h"

UBuildingStateComponent::UBuildingStateComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	GLEfNum = 0;

	TArray<FString> NiagaraPaths = {
		TEXT("/Game/Stylized_Projectiles/VFX_Niagara/Hit/NS_Stylized_Projectiles_Hit21.NS_Stylized_Projectiles_Hit21"),
		TEXT("/Game/Stylized_Projectiles/VFX_Niagara/Hit/NS_Stylized_Projectiles_Hit3.NS_Stylized_Projectiles_Hit3"),
		TEXT("/Game/Stylized_Projectiles/VFX_Niagara/Hit/NS_Stylized_Projectiles_Hit4.NS_Stylized_Projectiles_Hit4"),
		TEXT("/Game/Stylized_Projectiles/VFX_Niagara/Hit/NS_Stylized_Projectiles_Hit3.NS_Stylized_Projectiles_Hit3"),
		TEXT("/Game/Stylized_Projectiles/VFX_Niagara/Hit/NS_Stylized_Projectiles_Hit4.NS_Stylized_Projectiles_Hit4")
	};

	for (const FString& Path : NiagaraPaths)
	{
		ConstructorHelpers::FObjectFinder<UNiagaraSystem> FX(*Path);
		if (FX.Succeeded())
		{
			ExplosionFXs.Add(FX.Object);
		}
	}

}

void UBuildingStateComponent::RegisterGlobalEf(FGLBuildingEF ef)
{
	GlobalBuildingEf.Add(ef);
	for (ETeam team : ef.targetTeams)
	{
		if (TArray<FGLBuildingEF>** Found = TeamMap.Find(team))
		{
			(*Found)->Add(ef);
		}
	}
}

void UBuildingStateComponent::UnRegisterGlobalEf(FGLBuildingEF ef, FName actor)
{
	for (int i = 0; i < GlobalBuildingEf.Num() - 1; i++)
	{
		if (GlobalBuildingEf[i].SourceBuilding == actor)
		{
			auto RemoveByActor = [actor](TArray<FGLBuildingEF>& Arr)
				{
					Arr.RemoveAll([actor](const FGLBuildingEF& Ef)
						{
							return Ef.SourceBuilding == actor;
						});
				};

			for (ETeam team : GlobalBuildingEf[i].targetTeams)
			{
				if (TArray<FGLBuildingEF>** Found = TeamMap.Find(team))
				{
					RemoveByActor(**Found);
					GlobalEfBroadcast(team);
				}
			}
		}
	}
	GlobalBuildingEf.RemoveAll([&](const FGLBuildingEF& Ef)
		{
			return Ef.SourceBuilding == actor;
		});
}

TArray<FGLBuildingEF> UBuildingStateComponent::GetGlobalEfList(ETeam team)
{
	if (TArray<FGLBuildingEF>** Found = TeamMap.Find(team))
		return **Found;
	return TArray<FGLBuildingEF>();
}

void UBuildingStateComponent::SpawnBuldings()
{
	for (const FBuildingStateInfo info : AllBuildingStateInfo)
	{


		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* building = GetWorld()->SpawnActor<AActor>(info.BuildingClass, info.Location, info.Rotation, Params);
		if (info.isManager)
		{
			Cast<ADoorNWall>(building)->RecieveData(info.WallDoorManager);
		}
		Cast<ABuildingBase>(building)->ImportStateData(info.Level);
		
	}

	AllBuildingStateInfo.Empty();
}

void UBuildingStateComponent::GlobalEfBroadcast(ETeam team)
{
	switch (team)
	{
	case ETeam::None:
		break;
	case ETeam::Player:
		OnActivate_Player.Broadcast();
		break;
	case ETeam::Ally:
		OnActivate_Ally.Broadcast();
		break;
	case ETeam::Building:
		OnActivate_Building.Broadcast();
		break;
	case ETeam::Enemy:
		OnActivate_Enemy.Broadcast();
		break;
	default:
		break;
	}
}

void UBuildingStateComponent::BeginPlay()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnRoundStart.AddDynamic(this, &UBuildingStateComponent::SpawnBuldings);
		}
	}
}

void UBuildingStateComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnRoundStart.RemoveDynamic(this, &UBuildingStateComponent::SpawnBuldings);
		}
	}
}


