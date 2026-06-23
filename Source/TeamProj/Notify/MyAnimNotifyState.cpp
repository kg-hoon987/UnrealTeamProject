// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimNotifyState.h"
#include "../AnimInstance/MyAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UUpperBlendAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
}

void UUpperBlendAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp)
	{
		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		if (AnimInstance)
		{
			UPlayerInstanceBase* PlayerAnimInstance = Cast<UPlayerInstanceBase>(AnimInstance);
			if (PlayerAnimInstance)
			{
				PlayerAnimInstance->UpperBlendFunc(true);
			}
		}
	}
}

void UUpperBlendAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		if (AnimInstance)
		{
			UPlayerInstanceBase* PlayerAnimInstance = Cast<UPlayerInstanceBase>(AnimInstance);
			if (PlayerAnimInstance)
			{
				PlayerAnimInstance->UpperBlendFunc(false);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------

void UFlyingModeAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		AActor* Owner = MeshComp->GetOwner();
		if (Owner)
		{
			ACharacter* Character = Cast<ACharacter>(Owner);
			if (Character)
			{
				UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
				if (MovementComp)
				{
					MovementComp->SetMovementMode(MOVE_Flying);
				}
			}
		}
	}
}

void UFlyingModeAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UFlyingModeAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		AActor* Owner = MeshComp->GetOwner();
		if (Owner)
		{
			ACharacter* Character = Cast<ACharacter>(Owner);
			if (Character)
			{
				UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement();
				if (MovementComp)
				{
					MovementComp->SetMovementMode(MOVE_Walking);
				}
			}
		}
	}
}