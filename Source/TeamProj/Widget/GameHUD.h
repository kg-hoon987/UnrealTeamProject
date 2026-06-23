// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Components/Button.h"
#include "GameHUD.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UGameHUD : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* CntLevel;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UTextBlock* CntMoney;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* ExpProgress;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* CntStatusBtn;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUserWidget* CntStatus;
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUserWidget* CntGameStage;	//게임 진행도
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUserWidget* SkillSlot;		//퀵슬롯
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UUserWidget* Minimap;		//미니맵
	//현재버프 추후 추가
	//[SYSTEM] 메시지 추후 추가

public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void OnCntStatusBtnHovered();
	UFUNCTION()
	void OnCntStatusBtnUnhovered();
	UFUNCTION()
	void OnCntStatusClicked();

	UFUNCTION(BlueprintCallable)
	void SetSkillSlotVisibility();

	UFUNCTION(BlueprintCallable)
	void ChangeToMorning();

	UFUNCTION(BlueprintCallable)
	void ChangeToNight();

};
