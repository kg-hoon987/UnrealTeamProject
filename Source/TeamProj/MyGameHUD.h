// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamProj.h"
#include "GameFramework/HUD.h"
#include "Interface/WidgetInterface.h"
#include "MyGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AMyGameHUD : public AHUD, public IWidgetInterface
{
	GENERATED_BODY()
	
protected:
	FVector2D StartMousePos;
	FVector2D CurrentMousePos;
	bool isDrawing;
	TArray<AActor*> SelectUnits;
	TArray<AActor*> SelectUnitsInRect;

public:
	AMyGameHUD();
	virtual void DrawHUD() override;
	virtual void DragPressed_Implementation();
	virtual void DragReleased_Implementation();
	virtual void DragHold_Implementation();
	virtual void Move_Implementation(FVector2D movePos);

	
};
