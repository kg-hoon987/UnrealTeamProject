// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Styling/SlateBrush.h"
#include "BuffEffect.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UBuffEffect : public UUserWidget
{
	
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UImage* BuffIcon{};
	UPROPERTY(meta = (BindWidgetOptional))
	UImage* BuffCool{};

	UPROPERTY() UTexture2D* PendingIcon = nullptr;
	bool bHasPending = false;

	UPROPERTY(BlueprintReadOnly)
	FName EffectName = NAME_None;
	UPROPERTY(BlueprintReadOnly)
	float Duration = 0.f;
	UPROPERTY(BlueprintReadOnly)
	float RemainingTime = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDeBuff = false;
protected:

	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	void Setup(FName InName, UTexture2D* InIcon, float InDuration, bool bInDeBuff);


	UFUNCTION(BlueprintCallable)
	void UpdateRemaining(float NewRemaining);

private:
	void UpdateCoolVisual();
	
	
	
};

inline FSlateBrush MakeBrushFromTexture(UTexture2D* Texture, FVector2D Size /*= FVector2D::ZeroVector*/)
{
	FSlateBrush Brush;
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.Tiling = ESlateBrushTileType::NoTile;
	Brush.Mirroring = ESlateBrushMirrorType::NoMirror;
	Brush.ImageType = ESlateBrushImageType::FullColor;

	if (Texture)
	{
		Brush.SetResourceObject(Texture);
		if (Size.IsNearlyZero())
		{
			const int32 W = Texture->GetSizeX();
			const int32 H = Texture->GetSizeY();
			Size = FVector2D((float)W, (float)H);
		}
	}
	Brush.ImageSize = Size; // 원하는 픽셀 크기
	return Brush;
}