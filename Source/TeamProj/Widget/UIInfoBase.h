// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIInfoBase.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UUIInfoBase : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* OKBtn{};
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* BtnText{};
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* UITypeText{};

};
