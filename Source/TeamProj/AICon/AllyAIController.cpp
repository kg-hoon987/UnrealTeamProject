// Fill out your copyright notice in the Description page of Project Settings.


#include "AllyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../Character/GameCharacterBase.h"
#include "Kismet/GameplayStatics.h"

AAllyAIController::AAllyAIController()
{
	Team = ETeam::Ally;

	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		Tree(TEXT("/Game/Data/AI/Ally/BT_Ally.BT_Ally"));
	if (Tree.Succeeded())
		BTData = Tree.Object;

	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		Data(TEXT("/Game/Data/AI/Ally/BB_Ally.BB_Ally"));
	if (Data.Succeeded())
		BBData = Data.Object;

	
}

void AAllyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	SightConfig = NewObject<UAISenseConfig_Sight>(this, TEXT("Sight"));
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2200.f;
	SightConfig->PeripheralVisionAngleDegrees = 70.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());

	//맞았을대 걔한테 어그로 ㅇㅇ
	DamageConfig = NewObject<UAISenseConfig_Damage>(this, TEXT("Damage"));
	PerceptionComp->ConfigureSense(*DamageConfig);

	
}

void AAllyAIController::MoveFromMousePick(FVector2D MovePos)
{
	if (!GetPawn()) return;
	/*
	const FVector Dest(MovePos.X, MovePos.Y, GetPawn()->GetActorLocation().Z);
	FAIMoveRequest Req(Dest);

	if (Blackboard)
	{
		const float Accept = Blackboard->GetValueAsFloat(TEXT("MoveAcceptRadius"));
		if (Accept > 0.f) Req.SetAcceptanceRadius(Accept);
	}

	MoveTo(Req);
	*/
	MoveToLocation(FVector(MovePos.X, MovePos.Y, GetPawn()->GetActorLocation().Z));
	
}
