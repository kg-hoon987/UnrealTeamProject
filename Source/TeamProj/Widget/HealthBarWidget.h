// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/SizeBox.h"
#include "BuffEffect.h"
#include "HealthBarWidget.generated.h"

class AGameCharacterBase;
class UDamageComponent;
/**
 * 
 */
UCLASS()
class TEAMPROJ_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* m_ProgressBack=nullptr;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* m_ProgressFront = nullptr;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UImage* LevelImage = nullptr;
	
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* Level = nullptr;

	UPROPERTY(meta = (BindWidget))
	USizeBox* UpBuffDesp = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	USizeBox* DownBuffDesp = nullptr;
	


protected:
	UPROPERTY(BlueprintReadOnly)
	AGameCharacterBase* Class = nullptr;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUniformGridPanel* UpBuff = nullptr;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUniformGridPanel* DownBuff = nullptr;

	UPROPERTY(EditAnywhere, Category = "BuffUI")
	TSubclassOf<UBuffEffect> BuffWidgetClass;

	UPROPERTY(EditAnywhere, Category = "BuffUI")
	int32 MaxColumns = 0;

	TMap<FName, UBuffEffect*> UpBuffMap;
	TMap<FName, UBuffEffect*> DownBuffMap;

	FTimerHandle BuffRefreshTimer;

public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void Init(AGameCharacterBase* pClass);

private:
	void StartRefreshTimer();
	void RefreshBuffs();
	void RebuildGrid(UUniformGridPanel* Panel, const TMap<FName, UBuffEffect*>& Map);
	int32 PlaceInGrid(UUniformGridPanel* Panel, int32 Index, UBuffEffect* Widget);

	

	
};
