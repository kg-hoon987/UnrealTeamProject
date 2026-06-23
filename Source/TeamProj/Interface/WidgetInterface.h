// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TEAMPROJ_API IWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void DragPressed();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void DragHold();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void DragReleased();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void SelectUnit();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void DeSelectUnit();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "MyInterface")
	void Move(FVector2D MovePos);

};

UINTERFACE(BlueprintType)
class UHUDWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class IHUDWidgetInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RoundChangeToMorning();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RoundChangeToNight();
};
