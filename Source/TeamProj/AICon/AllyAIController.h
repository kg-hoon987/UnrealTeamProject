// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MyGameAIController.h"
#include "AllyAIController.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AAllyAIController : public AMyGameAIController
{
	GENERATED_BODY()
	
public:
	AAllyAIController();
	virtual void OnPossess(APawn* InPawn) override;
	
	void MoveFromMousePick(FVector2D MovePos);
	
};
