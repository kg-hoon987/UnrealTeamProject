// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyGameAIController.h"
#include "EnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AEnemyAIController : public AMyGameAIController
{
	GENERATED_BODY()
	
public:
	AEnemyAIController();
	virtual void OnPossess(APawn* InPawn) override;
	
};
