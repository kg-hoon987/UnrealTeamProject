// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

UMyAnimInstance::UMyAnimInstance()
{
	OwningPlayer = nullptr;
	CharacterMovement = nullptr;

	Velocity = FVector::ZeroVector;
	GroundSpeed = 0.0f;
	bShouldMove = false;
	bIsFalling = false;
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	AActor* Owner = GetOwningActor();

	if (Owner)
	{
		OwningPlayer = Cast<ACharacter>(Owner);

		if (OwningPlayer)
		{
			CharacterMovement = OwningPlayer->GetCharacterMovement();
		}
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (CharacterMovement)
	{
		Velocity = CharacterMovement->Velocity;

		GroundSpeed = FMath::Lerp(GroundSpeed, Velocity.Size2D(), DeltaSeconds * 10.0f);

		bShouldMove = GroundSpeed > 3.0f;

		bIsFalling = CharacterMovement->IsFalling();
	}
	else
	{
		Velocity = FVector::ZeroVector;
		GroundSpeed = 0.0f;
		bShouldMove = false;
		bIsFalling = false;
	}
}

//void UMyAnimInstance::OnAnimationInitialized_Implementation()
//{
//}

//---------------------------------------------------------------

UPlayerInstanceBase::UPlayerInstanceBase()
{
	bUpperBlend = false;
}

void UPlayerInstanceBase::UpperBlendFunc(bool bEnable)
{
	bUpperBlend = bEnable;
}

UAllyInstanceBase::UAllyInstanceBase()
{
}

UEnemyInstanceBase::UEnemyInstanceBase()
{
}
