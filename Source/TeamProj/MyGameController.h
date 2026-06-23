// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "MyGameHUD.h"
#include "Components/DecalComponent.h"
#include "Interface/WidgetInterface.h"
#include "MyGameController.generated.h"

class ADirectionalLight;
class ASkyLight;
class APostProcessVolume;
class UTimelineComponent;

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AMyGameController : public APlayerController, public IWidgetInterface
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;


	//InputMapping
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_LMB;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_RMB;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_Wheel;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_WheelClick;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_SPACEBAR;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_Q;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_E;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_R;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_F;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_Tab;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")	UInputAction* IA_Alt;

	void OnLMB_Pressed();	void OnLMB_Released();
	void OnRMB_Pressed();	void OnRMB_Released();
	void OnQ_Pressed();	void OnE_Pressed(); void OnR_Pressed(); void OnF_Pressed(); void OnTab_Pressed(); void OnTab_Released();
	void OnWheelAxis_Triggered(const FInputActionInstance& Instance);
	void OnWheelClick_Pressed();void OnWheelClick_Released();
	bool IsLMBPress;
	void OnSPACEBAR_Pressed();
	bool IsDetachCamera;

	void OnTab_Treggered(const FInputActionInstance& Instance);

	void OnAlt_Pressed(const FInputActionInstance& inst);	void OnAlt_Released(const FInputActionInstance& inst);

	//Widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI") TSubclassOf<UUserWidget> MainWidgetClass;
public:
	UPROPERTY(BlueprintReadOnly)												UUserWidget* MainWidget;

protected:
	//WeaponSelectUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI") TSubclassOf<UUserWidget> WeaponSelectClass;
	UPROPERTY(BlueprintReadOnly)												UUserWidget* WeaponSelect;
	
	//TabHoldUI
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI") TSubclassOf<UUserWidget> HoldTabUIClass;
	UPROPERTY(BlueprintReadOnly)												UUserWidget* HoldTabUI;
	//HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HUD") AHUD* HUD;
	
	//SkillTarget
	UFUNCTION(BlueprintCallable)
	void BeginTargeting(int32 SkillSlotIndex);
	UFUNCTION(BlueprintCallable)
	void EndTargeting(bool bApply);
	bool FinalizeAimForSkill();
	bool GetMouseAimLocation(FVector& OutLocation, FHitResult* OutHit = nullptr) const;

	//ItemTarget 
	UFUNCTION(BlueprintCallable)
	void BeginItemTargeting();
	UFUNCTION(BlueprintCallable)
	void EndItemTargeting(bool bApply);
	bool FinalizeAimForItem();
	void UpdateItemTargeting(float DeltaSeconds);
	float GetItemRange() const;
	float GetItemRadius() const;

	//BuildingUI
	bool isOpenBuildingUI;

protected:
	void UpdateTargeting(float DeltaSeconds);
	float GetSkillRangeFromSlot(int32 SlotIndex) const;
	float GetSkillRadiusFromSlot(int32 SlotIndex) const;

private:
	bool  bTargeting = false;
	int32 PendingSkillIndex = INDEX_NONE;
	FVector CurrentAim = FVector::ZeroVector;

	bool  bItemTargeting = false;
	FVector CurrentItemAim = FVector::ZeroVector;

	// 미리보기 데칼
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	UMaterialInterface* TargetDecalMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float DefaultDecalThickness = 64.f; // 데칼 박스의 X(투영 두께)

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TEnumAsByte<ECollisionChannel> TargetTraceChannel = ECC_GameTraceChannel4;

	UPROPERTY()
	UDecalComponent* TargetDecal = nullptr;

	// 범위/반경 클램프 옵션 설정ㅇ
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool bClampToSkillRange = true;

	// 스냅 옵션(땅/표면 노말로 데칼 회전)
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	bool bSnapToSurfaceNormal = true;

	
public:
	AMyGameController();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* IMC_Default;
	
	virtual void OnPossess(APawn* pPawn) override;
	void CloseBuildingUI() { isOpenBuildingUI = false; }

	UFUNCTION(BlueprintCallable)
	void StageStart();

	UPROPERTY(BlueprintReadOnly)
	float HoldTabTickTime{};
	
	UPROPERTY(BlueprintReadOnly)
	float usingTime{};

	UPROPERTY()
	ADirectionalLight* CachedDirLight = nullptr;

	UPROPERTY()
	ASkyLight* CachedSkyLight = nullptr;

	UPROPERTY()
	APostProcessVolume* CachedPostProcessVolume = nullptr;

	bool bisDay = true;
	void SetLight(bool isDay);
	void SetDayLight();

	void CacheLightsAndPostProcess();

	// Timeline
	UPROPERTY()
	UTimelineComponent* DayNightTimeline;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DayNightCurve; // 0 = 낮, 1 = 밤
	// Timeline Callback
	UFUNCTION()
	void HandleDayNightFade(float Alpha);

	void ToggleDayNight();

	float RoundTrigger = 2.0f;
	float CheckDelta;

	class URoundComponent* RoundComp;

	bool CantTime = false;

};
