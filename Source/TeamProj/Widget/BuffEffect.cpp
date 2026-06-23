// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffEffect.h"


void UBuffEffect::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateCoolVisual();
}

void UBuffEffect::Setup(FName InName, UTexture2D* InIcon, float InDuration, bool bInDeBuff)
{
	EffectName = InName;
	Duration = FMath::Max(0.01f, InDuration);
	RemainingTime = InDuration;
	bIsDeBuff = bInDeBuff;

	PendingIcon = InIcon;
	bHasPending = true;

	if (BuffIcon && InIcon)
	{
		const FSlateBrush Brush = MakeBrushFromTexture(InIcon, FVector2D(15.f, 15.f));
		BuffIcon->SetBrush(Brush);
		
	}


	UpdateCoolVisual();
}

void UBuffEffect::UpdateRemaining(float NewRemaining)
{
	
	RemainingTime = FMath::Clamp(NewRemaining, 0.f, Duration);
	UpdateCoolVisual();
}

void UBuffEffect::UpdateCoolVisual()
{
	
	if (!BuffCool || Duration <= 0.f) return;
	
	const float Pct = RemainingTime / Duration;

	if (UMaterialInstanceDynamic* MID = Cast<UMaterialInstanceDynamic>(BuffCool->GetDynamicMaterial()))
	{
		MID->SetScalarParameterValue(FName("Percent"), Pct);
		return;
	}
}