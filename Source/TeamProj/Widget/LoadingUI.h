// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingUI.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ULoadingUI : public UUserWidget
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void OnLevelLoaded();
	float curPercent{}, targetPercent{};
	float DotTimer{};  // Loading . . . 점 깜빡이는 타이머
	float BorderTimer{};  // 보더 회전하며 빛나는 타이머  // 둘 다 이터널 리턴 로딩창에서 영감

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UProgressBar* Bar;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* PercentText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* LoadingText;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* Border1;  // 좌상
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* Border2;  // 우상
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* Border3;  // 우하
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* Border4;  // 좌하
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
public:
	virtual bool Initialize() override;
};
