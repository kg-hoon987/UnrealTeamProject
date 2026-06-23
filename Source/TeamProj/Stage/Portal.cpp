// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"
#include "Kismet/GameplayStatics.h"
#include "RoundComponent.h"
#include "../MyGamePlayerState.h"
#include "../AICon/EnemyAIController.h"
#include "../Building/BuildingStateComponent.h"
#include "../Character/GameCharacterBase.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;
	isReady = false;
	SpawnPlan.Empty();
	GenTickPlan.Empty();
	GenMocCntPlan.Empty();

}

void APortal::BeginPlay()
{
	Super::BeginPlay();

	if (AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0)))
	{
		if (URoundComponent* RoundComp = PS->FindComponentByClass<URoundComponent>())
		{
			RoundComp->OnWarStart.AddDynamic(this, &APortal::ReadySpawn);
		}
	}
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (isReady)
	{
		for (int32 i = 0; i < SpawnPlan.Num(); i++)
		{
			SpawnMob(i, DeltaTime);
		}

	}
	
}

void APortal::BuildSpawnPlanFromIDs(TArray<FSpawnEntry> pptData)
{
	SpawnPlan.Reset();

	if (!CharInfoTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Portal] CharInfoTable is null."));
		return;
	}

	for (const FSpawnEntry& Entry : pptData)
	{
		const FCharInfo* Row = CharInfoTable->FindRow<FCharInfo>(Entry.MobID, TEXT("PortalBuild"), true);
		if (!Row)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Portal] CharInfo row NOT found for MobID: %s"), *Entry.MobID.ToString());
			continue;
		}
		FPortalSpawnInfo Info;
		Info.Entry = Entry;
		Info.CharInfo = *Row;
		SpawnPlan.Add(Info);
		GenTickPlan.Add(Info.Entry.GenTick);
		GenMocCntPlan.Add(Info.Entry.MobCount);
		
	}

}

void APortal::SpawnMob(int32 MobIndex, float DeltaTime)
{
	if (GenMocCntPlan[MobIndex] <= 0) return;

	UWorld* W = GetWorld();
	if (!W) return;

	GenTickPlan[MobIndex] -= DeltaTime;

	if (GenTickPlan[MobIndex] <= 0.f)
	{
		GenTickPlan[MobIndex] = SpawnPlan[MobIndex].Entry.GenTick;
		GenMocCntPlan[MobIndex]--;
		FActorSpawnParameters SpawnPara;
		SpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AEnemyCharacter* EC = W->SpawnActor<AEnemyCharacter>(EnemyBP, GetActorLocation(), FRotator(0.f, 0.f, 0.f), SpawnPara);
		EC->InitCharInfo(SpawnPlan[MobIndex].CharInfo.ClassType, SpawnPlan[MobIndex].CharInfo.StatID, SpawnPlan[MobIndex].CharInfo.SkillID
			, SpawnPlan[MobIndex].CharInfo.SkillLevel, SpawnPlan[MobIndex].CharInfo.Chest, SpawnPlan[MobIndex].CharInfo.Parts, SpawnPlan[MobIndex].CharInfo.WeaponLeft, SpawnPlan[MobIndex].CharInfo.WeaponRight
			, SpawnPlan[MobIndex].CharInfo.AnimClass, SpawnPlan[MobIndex].CharInfo.CharScale);
		EC->Tags.AddUnique(SpawnPlan[MobIndex].CharInfo.StatID.WeaponType);
		AEnemyAIController* EAC = Cast<AEnemyAIController>(EC->GetController());
		if (!EAC) return;
		AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
		if (!PS) return;
		UBuildingStateComponent* BSComp = PS->FindComponentByClass<UBuildingStateComponent>();
		if (!BSComp) return;
		EAC->SetMainCastle(BSComp->MainCastle);
	}
	


}

void APortal::ReadySpawn()
{
	isReady = true;
}


void APortal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{

	if (AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0)))
	{
		if (URoundComponent* RoundComp = PS->FindComponentByClass<URoundComponent>())
		{
			RoundComp->OnWarStart.RemoveDynamic(this, &APortal::ReadySpawn);
		}
	}
}
