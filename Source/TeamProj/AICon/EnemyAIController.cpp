// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"


AEnemyAIController::AEnemyAIController()
{
	Team = ETeam::Enemy;

	//타입에 따라 비헤이비엍리랑 블랙보드 나눠야되네...
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>
		Tree(TEXT("/Game/Data/AI/Enemy/BT_Enemy.BT_Enemy"));
	if (Tree.Succeeded())
		BTData = Tree.Object;

	static ConstructorHelpers::FObjectFinder<UBlackboardData>
		Data(TEXT("/Game/Data/AI/Enemy/BB_Enemy.BB_Enemy"));
	if (Data.Succeeded())
		BBData = Data.Object;
}

void AEnemyAIController::OnPossess(APawn* InPawn)
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
	PerceptionComp->RequestStimuliListenerUpdate();

}
