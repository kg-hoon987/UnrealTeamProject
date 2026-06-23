// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SkillEvent.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UAnimNotify_SkillEvent : public UAnimNotify
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FName EventName = TEXT("Fire");

	virtual FString GetNotifyName_Implementation() const override
	{
		return EventName.IsNone() ? TEXT("SkillEvent") : EventName.ToString();
	}

	virtual bool Received_Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;

};
