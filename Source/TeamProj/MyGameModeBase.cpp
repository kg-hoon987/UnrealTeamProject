// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "MyGameController.h"
#include "MyGamePlayerState.h"
#include "MyGameHUD.h"
#include "AITypes.h"
#include "GenericTeamAgentInterface.h"
#include "Character/GameCharacterBase.h"


AMyGameModeBase::AMyGameModeBase()
{
	PlayerStateClass = AMyGamePlayerState::StaticClass();
	PlayerControllerClass = AMyGameController::StaticClass();
	HUDClass = AMyGameHUD::StaticClass();
	

	DefaultPawnClass = APlayerCharacter::StaticClass();
}

void AMyGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	

	
}

static ETeamAttitude::Type SolveAttitude(FGenericTeamId A, FGenericTeamId B)
{
	if (A == B) return ETeamAttitude::Friendly;

	const uint8 a = A.GetId();
	const uint8 b = B.GetId();
	const bool aIsPAB = (a == 1 || a == 2 || a == 3); //플레이어 아군 빌딩
	const bool bIsPAB = (b == 1 || b == 2 || b == 3);

	if (aIsPAB && bIsPAB) return ETeamAttitude::Friendly;
	return ETeamAttitude::Hostile;
}