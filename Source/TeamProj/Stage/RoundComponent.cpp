// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundComponent.h"
#include "Portal.h"
#include "../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../Widget/GameHUD.h"
#include "../Perk/PerkComponent.h"
#include "../MyGameController.h"
#include "StageComponent.h"

URoundComponent::URoundComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	TotalMobCount = 0;
	RoundNum = 1;

	static ConstructorHelpers::FClassFinder<UUserWidget>RS(TEXT("/Game/Widget/UI/RoundResult/W_RoundStart.W_RoundStart_C"));
	if (RS.Succeeded())
	{
		RoundStartUI = RS.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>WS(TEXT("/Game/Widget/UI/RoundResult/W_WarStart.W_WarStart_C"));
	if (WS.Succeeded())
	{
		WarStartUI = WS.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>WE(TEXT("/Game/Widget/UI/RoundResult/W_WarEnd.W_WarEnd_C"));
	if (WE.Succeeded())
	{
		WarEndUI = WE.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>VC(TEXT("/Game/Widget/UI/RoundResult/W_Victory.W_Victory_C"));
	if (VC.Succeeded())
	{
		VictoryUI = VC.Class;
	}
	static ConstructorHelpers::FClassFinder<UUserWidget>DF(TEXT("/Game/Widget/UI/RoundResult/W_Defeat.W_Defeat_C"));
	if (DF.Succeeded())
	{
		DefeatUI = DF.Class;
	}


}

void URoundComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



void URoundComponent::InitRound(FStage* stageInfo, FRound& roundInfo)
{
	SetPortalPos(stageInfo->PortalPos, stageInfo->PortalRot);
	TotalMobCount = 0;

	for (FSpawnPortal Pt : roundInfo.PortalInfo)
	{
		FString L, R;
		if (Pt.PortalID.ToString().Split(TEXT("_"), &L, &R))
		{
			if (R.IsNumeric())
			{
				int id = FCString::Atoi(*R);
				FActorSpawnParameters SpawnPara;
				SpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				APortal* SpawnPt = GetWorld()->SpawnActor<APortal>(PortalBP, PortalPos[id - 1], PortalRot[id - 1], SpawnPara);
				if (!SpawnPt)
					return;
				SpawnPt->BuildSpawnPlanFromIDs(Pt.MobInfo);
			}
		}
		
		for (FSpawnEntry mob : Pt.MobInfo)
			TotalMobCount += mob.MobCount;
	}
	UE_LOG(LogTemp, Warning, TEXT("Current Rind Total mob Cnt = %d, %d"), TotalMobCount,RoundNum);
	if (RoundNum != 1)
		InitRoundStart();
}

void URoundComponent::SetState(ERoundState state)
{
	switch (state)
	{
	case ERoundState::None:
		break;
	case ERoundState::RoundStart:
	{
		RoundState = ERoundState::RoundStart;
		OnRoundStart.Broadcast();
		if (RoundStartUI)
		{
			RSUI = CreateWidget<UUserWidget>(GetWorld(), RoundStartUI);
			RSUI->AddToViewport();
		}

		APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Pawn)
		{
			if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
			{
				UStageComponent* stageCom = Cast<UStageComponent>(MyPS->StageCom);
				if (RoundNum != 1)
				{
					Cast<AMyGameController>(Pawn->GetController())->ToggleDayNight();
				}

				if (MyPS->PerkComp)
				{
					MyPS->PerkComp->OnRoundStart();
				}
			}
		}
		break;
	}
	case ERoundState::WarStart:
		RoundState = ERoundState::WarStart;
		OnWarStart.Broadcast();
		if (AMyGameController* GC = Cast<AMyGameController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			if (UGameHUD* HU = Cast<UGameHUD>(GC->MainWidget))
			{
				HU->ChangeToNight();
			}
		}
		if (WarStartUI)
		{
			WSUI = CreateWidget<UUserWidget>(GetWorld(), WarStartUI);
			WSUI->AddToViewport();
		}

		break;
	case ERoundState::WarEnd:
		RoundState = ERoundState::WarEnd;
		OnWarEnd.Broadcast();
		if (AMyGameController* GC = Cast<AMyGameController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
		{
			if (UGameHUD* HU = Cast<UGameHUD>(GC->MainWidget))
			{
				HU->ChangeToMorning();
			}
		}
		if (WarEndUI)
		{
			WEUI = CreateWidget<UUserWidget>(GetWorld(), WarEndUI);
			WEUI->AddToViewport();
		}
		InitRoundEnd();
		break;
	case ERoundState::RoundEnd:
	{
		RoundState = ERoundState::RoundEnd;
		OnRoundEnd.Broadcast();

		APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (Pawn)
		{
			if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
			{
				if (MyPS->PerkComp)
				{
					MyPS->PerkComp->OnRoundEnd();
				}
			}
		}
		RoundNum++;
		Cast<UStageComponent>(GetOwner()->FindComponentByClass(UStageComponent::StaticClass()))->SetRound();
		break;
	}
	default:
		break;
	}
}

void URoundComponent::InitRoundStart()
{
	if (RoundState != ERoundState::RoundStart)
		SetState(ERoundState::RoundStart);
}

void URoundComponent::InitRoundEnd()
{
	if (RoundState != ERoundState::RoundEnd)
		SetState(ERoundState::RoundEnd);
}

void URoundComponent::InitWarStart()
{
	if (RoundState != ERoundState::WarStart)
		SetState(ERoundState::WarStart);
}

void URoundComponent::InitWarEnd()
{
	if (RoundState != ERoundState::WarEnd)
		SetState(ERoundState::WarEnd);
}




void URoundComponent::SetPortalPos(TArray<FVector> ptps, TArray<FRotator> ptpr)
{
	for (FVector ps : ptps)
	{
		PortalPos.Add(ps);
	}
	for (FRotator pr : ptpr)
	{
		PortalRot.Add(pr);
	}
}

void URoundComponent::CheckMonsterCount()
{
	TotalMobCount--;
	if (TotalMobCount <= 0)
		InitWarEnd();
}




void URoundComponent::BeginPlay()
{
	Super::BeginPlay();

	// 라운드 받아오기
	// 그거로 인잇
}