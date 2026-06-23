// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "MyAnimInstance.generated.h"

/**
 *
 */
UCLASS()
class TEAMPROJ_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	ACharacter* OwningPlayer;

	UPROPERTY(BlueprintReadOnly)
	UCharacterMovementComponent* CharacterMovement;

	UPROPERTY(BlueprintReadOnly)
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldMove;

	UPROPERTY(BlueprintReadOnly)
	bool bIsFalling;

	/*UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnAnimationInitialized();
	virtual void OnAnimationInitialized_Implementation();*/

public:
	UMyAnimInstance();

protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};






UCLASS()
class TEAMPROJ_API UPlayerInstanceBase : public UMyAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly)
	bool bUpperBlend;

public:
	UPlayerInstanceBase();

	UFUNCTION(BlueprintCallable)
	void UpperBlendFunc(bool bEnable);
};

UCLASS()
class TEAMPROJ_API UAllyInstanceBase : public UMyAnimInstance
{
	GENERATED_BODY()

public:
	UAllyInstanceBase();
};

UCLASS()
class TEAMPROJ_API UEnemyInstanceBase : public UMyAnimInstance
{
	GENERATED_BODY()

public:
	UEnemyInstanceBase();
};