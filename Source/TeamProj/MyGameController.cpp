// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Skill/SkillComponent.h" 
#include "Skill/SkillStruct.h"
#include "Character/GameCharacterBase.h"
#include "Stage/RoundComponent.h"
#include "Widget/GameHUD.h"
#include "MyGamePlayerState.h"
#include "Item/ItemComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Math/Color.h"    
#include "Math/Vector4.h"   
#include "EngineUtils.h"          // TActorIterator
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"                  // ASkyLight
#include "Components/SkyLightComponent.h"     // USkyLightComponent
#include "Components/DirectionalLightComponent.h" 
#include "Curves/CurveFloat.h"
#include "Components/TimelineComponent.h"
#include "Building/BuildingBase.h"
#include "Landscape.h"
#include "EngineUtils.h"


AMyGameController::AMyGameController()
{
	IA_LMB = nullptr;
	IA_RMB = nullptr;
	IA_SPACEBAR = nullptr;
	IA_Wheel = nullptr;
	IA_WheelClick = nullptr;
	IMC_Default = nullptr;
	MainWidget = nullptr;
	MainWidgetClass = nullptr;
	IsLMBPress = false;
	HUD = nullptr;
	IsDetachCamera = false;
	isOpenBuildingUI = false;

	PrimaryActorTick.bCanEverTick = true;
}

void AMyGameController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController()) return;

	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (auto* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(IMC_Default, 0);
		}
	}
	bShowMouseCursor = true;

	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UUserWidget>(this, MainWidgetClass);
		if (MainWidget)
		{
			MainWidget->AddToViewport();

		}
	}


	HUD = GetHUD();
	SetIgnoreLookInput(true);
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	Mode.SetHideCursorDuringCapture(false);
	SetInputMode(Mode);

	CacheLightsAndPostProcess();

	DayNightTimeline = NewObject<UTimelineComponent>(this, TEXT("DayNightTimeline"));
	DayNightTimeline->RegisterComponent();

	DayNightTimeline = NewObject<UTimelineComponent>(this, TEXT("DayNightTimeline"));
	if (DayNightTimeline)
	{
		DayNightTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		DayNightTimeline->RegisterComponent();
		DayNightTimeline->SetLooping(false);

		if (DayNightCurve)
		{
			FOnTimelineFloat ProgressFunction;
			ProgressFunction.BindUFunction(this, FName("HandleDayNightFade"));
			DayNightTimeline->AddInterpFloat(DayNightCurve, ProgressFunction);

			// Timeline 완료 시 처리 (옵션)
			FOnTimelineEvent FinishedFunction;
			FinishedFunction.BindUFunction(this, FName("OnDayNightTimelineFinished"));
			DayNightTimeline->SetTimelineFinishedFunc(FinishedFunction);
		}
	}

	if (CachedPostProcessVolume)
	{
		CachedPostProcessVolume->BlendWeight = 0.0f;
	}


	APawn* P = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (P)
		RoundComp = Cast<AMyGamePlayerState>(P->GetPlayerState())->RoundCom;
	SetDayLight();
	
	if (WeaponSelectClass)
	{
		WeaponSelect = CreateWidget<UUserWidget>(this, WeaponSelectClass);
		if (WeaponSelect)
		{
			WeaponSelect->AddToViewport();
		}
	}

	if (HoldTabUIClass)
	{
		HoldTabUI = CreateWidget<UUserWidget>(this, HoldTabUIClass);
		
		if(APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
		{
			PC->SetHoldTabWidget(HoldTabUI);
			PC->UnLookHoldTabWidget();
			
		}
	}
}

void AMyGameController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (auto* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(IA_LMB, ETriggerEvent::Started, this, &AMyGameController::OnLMB_Pressed);
		EIC->BindAction(IA_LMB, ETriggerEvent::Completed, this, &AMyGameController::OnLMB_Released);
		EIC->BindAction(IA_RMB, ETriggerEvent::Started, this, &AMyGameController::OnRMB_Pressed);
		EIC->BindAction(IA_RMB, ETriggerEvent::Completed, this, &AMyGameController::OnRMB_Released);
		EIC->BindAction(IA_Wheel, ETriggerEvent::Triggered, this, &AMyGameController::OnWheelAxis_Triggered);
		EIC->BindAction(IA_WheelClick, ETriggerEvent::Started, this, &AMyGameController::OnWheelClick_Pressed);
		EIC->BindAction(IA_WheelClick, ETriggerEvent::Completed, this, &AMyGameController::OnWheelClick_Released);
		EIC->BindAction(IA_SPACEBAR, ETriggerEvent::Started, this, &AMyGameController::OnSPACEBAR_Pressed);
		EIC->BindAction(IA_Q, ETriggerEvent::Started, this, &AMyGameController::OnQ_Pressed);
		EIC->BindAction(IA_E, ETriggerEvent::Started, this, &AMyGameController::OnE_Pressed);
		EIC->BindAction(IA_R, ETriggerEvent::Started, this, &AMyGameController::OnR_Pressed);
		EIC->BindAction(IA_F, ETriggerEvent::Started, this, &AMyGameController::OnF_Pressed);
		EIC->BindAction(IA_Tab, ETriggerEvent::Started, this, &AMyGameController::OnTab_Pressed);
		EIC->BindAction(IA_Tab, ETriggerEvent::Triggered, this, &AMyGameController::OnTab_Treggered);
		EIC->BindAction(IA_Tab, ETriggerEvent::Completed, this, &AMyGameController::OnTab_Released);
		EIC->BindAction(IA_Alt, ETriggerEvent::Started, this, &AMyGameController::OnAlt_Pressed);
		EIC->BindAction(IA_Alt, ETriggerEvent::Completed, this, &AMyGameController::OnAlt_Released);
	}


}



void AMyGameController::OnLMB_Pressed()
{
	IsLMBPress = true;
	if (IWidgetInterface* IF = Cast<IWidgetInterface>(GetHUD()))
		IF->DragPressed_Implementation();

	if (bTargeting)
	{
		if (FinalizeAimForSkill())
			EndTargeting(true);
		else
			EndTargeting(false);
	}
	else if (bItemTargeting)
	{
		if (FinalizeAimForItem())
			EndItemTargeting(true);
		else
			EndItemTargeting(false);
	}
}



void AMyGameController::OnLMB_Released()
{
	IsLMBPress = false;
	if (IWidgetInterface* IF = Cast<IWidgetInterface>(GetHUD()))
		IF->DragReleased_Implementation();
}


void AMyGameController::OnRMB_Pressed()
{
	
	if (IWidgetInterface* IF = Cast<IWidgetInterface>(GetHUD()))
	{
		FHitResult Point;
		GetHitResultUnderCursorByChannel(TraceTypeQuery1, true, Point);
		IF->Move_Implementation(FVector2D(Point.ImpactPoint.X,Point.ImpactPoint.Y));
	}

	if (bTargeting)
		EndTargeting(false);

	if (bItemTargeting)
		EndItemTargeting(false);
}


void AMyGameController::OnRMB_Released()
{
}

void AMyGameController::OnQ_Pressed()
{
	BeginTargeting(0);
}

void AMyGameController::OnE_Pressed()
{
	BeginTargeting(1);
}

void AMyGameController::OnR_Pressed()
{
	BeginTargeting(2);
}

void AMyGameController::OnF_Pressed()
{
	BeginItemTargeting();
}

void AMyGameController::OnTab_Pressed()
{
	if (isOpenBuildingUI) return;

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
	{
		if (!PC->GetBuilding())  return;
		if (RoundComp->RoundState != ERoundState::RoundStart) return;

		PC->GetBuilding()->OpenUI();
		isOpenBuildingUI = true;
	}
}

void AMyGameController::OnTab_Released()
{
	HoldTabTickTime = 0.f;
	usingTime = 0.f;
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
	{
		PC->UnLookHoldTabWidget();
	}
}


void AMyGameController::OnTab_Treggered(const FInputActionInstance& Instance)
{
	HoldTabTickTime = Instance.GetTriggeredTime();

	

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
	{
		if (PC->GetBuilding())
		{
			usingTime = HoldTabTickTime;
		}
	
		if (RoundComp->RoundState != ERoundState::RoundStart)
		{
			HoldTabTickTime = 0.f;
		}

		if (HoldTabTickTime - usingTime >= 0.2f)
			PC->LookHoldTabWidget();
		else
			PC->UnLookHoldTabWidget();

		if (HoldTabTickTime-usingTime >= RoundTrigger)
		{
			RoundComp->InitWarStart();
			ToggleDayNight();
			HoldTabTickTime = 0.f;
			usingTime = 0.f;
		}
	}
}

void AMyGameController::OnAlt_Pressed(const FInputActionInstance& inst)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
		if (RoundComp->RoundState != ERoundState::RoundStart) return;

	for (TActorIterator<ABuildingBase> It(GetWorld()); It; ++It)
	{
		ABuildingBase* b = *It;
		if (!b->bIsBuilt)
		{
			b->Preview(true);
		}
	}
}

void AMyGameController::OnAlt_Released(const FInputActionInstance& inst)
{
	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetPawn()))
		if (RoundComp->RoundState != ERoundState::RoundStart) return;

	for (TActorIterator<ABuildingBase> It(GetWorld()); It; ++It)
	{
		ABuildingBase* b = *It;
		if (!b->bIsBuilt)
		{
			b->Preview(false);
		}
	}
}

void AMyGameController::OnWheelAxis_Triggered(const FInputActionInstance& Instance)
{
	float Value = Instance.GetValue().Get<float>();
	if (APlayerCharacter* CH = Cast<APlayerCharacter>(GetPawn()))
	{
		CH->ZoomInCamera(Value);
	}
}


void AMyGameController::OnWheelClick_Pressed()
{
}

void AMyGameController::OnWheelClick_Released()
{
}

void AMyGameController::OnSPACEBAR_Pressed()
{
	IsDetachCamera = !IsDetachCamera;
	
	if (APlayerCharacter* CH = Cast<APlayerCharacter>(GetPawn()))
	{
		if (IsDetachCamera)
			CH->DetachCamera();
		else
			CH->AttachCamera();
	}

}



void AMyGameController::Tick(float DeltaTime)
{
	if (IsLMBPress)
	{
		if (IWidgetInterface* IF = Cast<IWidgetInterface>(GetHUD()))
		{
			IF->DragHold_Implementation();
		}
	}

	if (bTargeting)
	{
		UpdateTargeting(DeltaTime);
	}

	if (bItemTargeting)
	{
		UpdateItemTargeting(DeltaTime);
	}

	if (DayNightTimeline)
	{
		DayNightTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}
}


void AMyGameController::OnPossess(APawn* pPawn)
{
	Super::OnPossess(pPawn);
}

void AMyGameController::StageStart()
{
	AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	if (!PS) return;

	URoundComponent* RC = PS->FindComponentByClass<URoundComponent>();
	if (!RC) return;

	if (MainWidget)
	{
		UGameHUD* GH = Cast<UGameHUD>(MainWidget);
		if (!GH) return;
		GH->SetSkillSlotVisibility();
	}

	RC->SetState(ERoundState::RoundStart);

	


}


bool AMyGameController::GetMouseAimLocation(FVector& OutLocation, FHitResult* OutHit) const
{
	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(TargetTraceChannel, true, Hit);

	if (bHit)
	{
		OutLocation = Hit.ImpactPoint;
		if (OutHit) *OutHit = Hit;
		return true;
	}
	return false;
}

void AMyGameController::BeginTargeting(int32 SkillSlotIndex)
{
	PendingSkillIndex = SkillSlotIndex;
	bTargeting = true;

	if (!TargetDecal && TargetDecalMaterial)
	{
		const FVector SpawnAt = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
		TargetDecal = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			TargetDecalMaterial,
			FVector(DefaultDecalThickness, 128.f, 128.f),
			SpawnAt,
			FRotator(-90.f, 0.f, 0.f),
			0.f
		);
		if (TargetDecal)
			TargetDecal->SetFadeScreenSize(0.f);
	}
	else if (TargetDecal)
	{
		TargetDecal->SetHiddenInGame(false);
	}
}

void AMyGameController::EndTargeting(bool bApply)
{
	if (bApply && GetPawn())
	{
		if (USkillComponent* Skills = GetPawn()->FindComponentByClass<USkillComponent>())
		{
			Skills->TryUseSKill(PendingSkillIndex, nullptr, CurrentAim);
		}
	}
	bTargeting = false;
	PendingSkillIndex = INDEX_NONE;
	CurrentAim = FVector::ZeroVector;

	if (TargetDecal)
		TargetDecal->SetHiddenInGame(true);
}

bool AMyGameController::FinalizeAimForSkill()
{
	if (!bTargeting || !GetPawn()) return false;

	FVector Aim; 
	FHitResult Hit;
	if (!GetMouseAimLocation(Aim, &Hit)) return false;

	const FVector O = GetPawn()->GetActorLocation();
	const float Range = GetSkillRangeFromSlot(PendingSkillIndex);
	{
		FHitResult G;
		FVector S = Aim + FVector(0, 0, 3000), E = Aim - FVector(0, 0, 3000);
		if (GetWorld()->LineTraceSingleByChannel(G, S, E, ECC_Visibility))
			Aim = G.ImpactPoint;
	}

	const bool bClamp = bClampToSkillRange;
	const FVector V = Aim - O;
	const float   D = V.Size2D();

	if (Range > 0.f && D > Range)
	{
		if (bClamp)
			Aim = O + V.GetSafeNormal2D() * Range;        
		else
			return false;                                 
	}

	CurrentAim = Aim; 
	return true;

}

void AMyGameController::UpdateTargeting(float DeltaSeconds)
{
	FHitResult Hit;
	if (!GetMouseAimLocation(CurrentAim, &Hit))
		return;

	const FVector O = FVector(GetPawn()->GetActorLocation().X, GetPawn()->GetActorLocation().Y, 0.f);
	const FVector V = FVector(CurrentAim.X - O.X, CurrentAim.Y - O.Y, 0.f);
	if (bClampToSkillRange && GetPawn())
	{
		const float Range = GetSkillRangeFromSlot(PendingSkillIndex);
		if (Range > 0.f)
		{
			const float D = V.Size();
			if (D > Range)
				CurrentAim = O + V / D * Range;
		}
	}

	if (TargetDecal)
	{
		const float Range = GetSkillRangeFromSlot(PendingSkillIndex);
		const float Radius = GetSkillRadiusFromSlot(PendingSkillIndex);
		float dist = V.Length();
		if (dist > Range)
		{
			UE_LOG(LogTemp, Warning, TEXT("[asdfasfds] Dist>Range : %f,%f"), dist, Range);
			TargetDecal->SetWorldLocation(O + (V.GetSafeNormal() * Range) + FVector(0, 0, 2.f));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[asdfasfds] Dist>Range : %f,%f"), dist, Range);
			TargetDecal->SetWorldLocation(CurrentAim + FVector(0, 0, 2.f));
		}

		TargetDecal->DecalSize = FVector(DefaultDecalThickness, 100.f, 100.f);
	}
}


float AMyGameController::GetSkillRangeFromSlot(int32 SlotIndex) const
{
	if (const APawn* P = GetPawn())
		if (const USkillComponent* SkillComp = P->FindComponentByClass<USkillComponent>())
		{
			USkill* S = SkillComp->GetSkillSlot(SlotIndex);
			const float Range = S->GetSkillData().Range + S->GetSkillLevelData().AddRange;
			return Range;
		}
	return 800.f;
}

float AMyGameController::GetSkillRadiusFromSlot(int32 SlotIndex) const
{
	if (const APawn* P = GetPawn())
		if (const USkillComponent* SkillComp = P->FindComponentByClass<USkillComponent>())
		{
			USkill* S = SkillComp->GetSkillSlot(SlotIndex);
			const float Radius = S->GetSkillData().Radius + S->GetSkillLevelData().AddRadius;
			return Radius;
		}
	return 200.f;
}

///////////////////////////////////////////////////////////// 아이템

// 아이템 타겟팅 함수들
void AMyGameController::BeginItemTargeting()
{
	// 아이템 수량 확인
	if (const APawn* P = GetPawn())
	{
		if (const UItemComponent* ItemComp = P->FindComponentByClass<UItemComponent>())
		{
			if (UItem* EquippedItem = ItemComp->GetEquippedItem())
			{
				// 수량이 0이면 타겟팅 시작하지 않음
				if (!EquippedItem->HasQuantity())
				{
					return;
				}
			}
			else
			{
				// 장착된 아이템이 없으면 타겟팅 시작하지 않음
				return;
			}
		}
		else
		{
			// ItemComponent가 없으면 타겟팅 시작하지 않음
			return;
		}
	}

	bItemTargeting = true;

	// 데칼 생성 또는 표시
	if (!TargetDecal)
	{

		if (TargetDecalMaterial)
		{
			const FVector SpawnAt = GetPawn() ? GetPawn()->GetActorLocation() : FVector::ZeroVector;
			TargetDecal = UGameplayStatics::SpawnDecalAtLocation(
				GetWorld(),
				TargetDecalMaterial,
				FVector(DefaultDecalThickness, 100.f, 100.f),
				SpawnAt,
				FRotator(-90.f, 0.f, 0.f),
				0.f
			);
			if (TargetDecal)
			{
				TargetDecal->SetFadeScreenSize(0.f);
			}
		}
	}
	else
	{
		TargetDecal->SetHiddenInGame(false);
	}
}

void AMyGameController::EndItemTargeting(bool bApply)
{
	if (bApply && GetPawn())
	{
		if (UItemComponent* ItemComp = GetPawn()->FindComponentByClass<UItemComponent>())
		{
			ItemComp->TryUseItem(nullptr, CurrentItemAim);
		}
	}
	bItemTargeting = false;
	CurrentItemAim = FVector::ZeroVector;

	if (TargetDecal)
		TargetDecal->SetHiddenInGame(true);
}

bool AMyGameController::FinalizeAimForItem()
{
	if (!bItemTargeting || !GetPawn()) return false;

	FVector Aim;
	FHitResult Hit;
	if (!GetMouseAimLocation(Aim, &Hit)) return false;

	const FVector O = GetPawn()->GetActorLocation();
	const float Range = GetItemRange();
	{
		FHitResult G;
		FVector S = Aim + FVector(0, 0, 3000), E = Aim - FVector(0, 0, 3000);
		if (GetWorld()->LineTraceSingleByChannel(G, S, E, ECC_Visibility))
			Aim = G.ImpactPoint;
	}

	const bool bClamp = bClampToSkillRange;
	const FVector V = Aim - O;
	const float   D = V.Size2D();

	if (Range > 0.f && D > Range)
	{
		if (bClamp)
			Aim = O + V.GetSafeNormal2D() * Range;
		else
			return false;
	}

	CurrentItemAim = Aim;
	return true;
}

void AMyGameController::UpdateItemTargeting(float DeltaSeconds)
{
	FHitResult Hit;
	if (!GetMouseAimLocation(CurrentItemAim, &Hit))
		return;
	if (bClampToSkillRange && GetPawn())
	{
		const float Range = GetItemRange();
		if (Range > 0.f)
		{
			const FVector O = GetPawn()->GetActorLocation();
			const FVector V = CurrentItemAim - O;
			const float D = V.Size();
			if (D > Range)
				CurrentItemAim = O + V / D * Range;
		}
	}

	if (TargetDecal)
	{
		const float Range = GetItemRange();
		const float Radius = GetItemRadius();
		TargetDecal->SetWorldLocation(CurrentItemAim + FVector(0, 0, 2.f));

		TargetDecal->DecalSize = FVector(DefaultDecalThickness, 100.f, 100.f);
	}
}

float AMyGameController::GetItemRange() const
{
	if (const APawn* P = GetPawn())
		if (const UItemComponent* ItemComp = P->FindComponentByClass<UItemComponent>())
		{
			if (UItem* EquippedItem = ItemComp->GetEquippedItem())
			{
				const float Range = EquippedItem->GetItemData().Range + EquippedItem->GetItemLevelData().AddRange;
				return Range;
			}
		}
	return 800.f;
}

float AMyGameController::GetItemRadius() const
{
	if (const APawn* P = GetPawn())
		if (const UItemComponent* ItemComp = P->FindComponentByClass<UItemComponent>())
		{
			if (UItem* EquippedItem = ItemComp->GetEquippedItem())
			{
				const float Radius = EquippedItem->GetItemData().Radius + EquippedItem->GetItemLevelData().AddRadius;
				return Radius;
			}
		}
	return 200.f;
}


void AMyGameController::SetLight(bool isDay)
{
	// 레벨에 배치된 DirectionalLight 액터를 찾음
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		ADirectionalLight* DirLight = *It;
		if (DirLight)
		{
			UDirectionalLightComponent* LightComp = Cast<UDirectionalLightComponent>(DirLight->GetLightComponent());
			if (LightComp)
			{
				LightComp->SetIntensity(10000.0f);   // 낮 느낌 (lux 단위 아님, Unreal 단위)
				LightComp->SetLightColor(FLinearColor::White); // 5500K 정도 느낌
				// 색온도 사용하려면 Temperature 모드
				LightComp->SetUseTemperature(true);
				LightComp->SetTemperature(3000.0f);   // 태양광
			}
		}
	}
	// SkyLight도 같이 업데이트
	for (TActorIterator<ASkyLight> It(GetWorld()); It; ++It)
	{
		ASkyLight* Sky = *It;
		if (Sky && Sky->GetLightComponent())
		{
			USkyLightComponent* SkyComp =
				Cast<USkyLightComponent>(Sky->GetLightComponent());
			if (SkyComp)
			{
				SkyComp->Intensity = 1.0f;
				SkyComp->RecaptureSky();
			}
		}
	}
}

void AMyGameController::SetDayLight()
{
	// Directional Light
	if (CachedDirLight)
	{
		if (UDirectionalLightComponent* DirComp = Cast<UDirectionalLightComponent>(CachedDirLight->GetLightComponent()))
		{
			DirComp->SetIntensity(10.0f);
			DirComp->SetUseTemperature(true);
			DirComp->SetTemperature(7000.0f);
		}
	}

	// Sky Light
	if (CachedSkyLight)
	{
		if (USkyLightComponent* SkyComp = Cast<USkyLightComponent>(CachedSkyLight->GetLightComponent()))
		{
			SkyComp->Intensity = 1.0f;
			SkyComp->RecaptureSky();
		}
	}

	// PostProcessVolume
	if (CachedPostProcessVolume)
	{
		FPostProcessSettings& S = CachedPostProcessVolume->Settings;
		S.BloomIntensity = 1.2f;
		S.AutoExposureBias = 1.2f;
		S.VignetteIntensity = 0.2f;
		S.ColorSaturation = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		S.WhiteTemp = 6000.f;
		S.WhiteTint = 0.f;
	}

	bisDay = true; // 상태도 낮으로 초기화
}

void AMyGameController::CacheLightsAndPostProcess()
{
	// DirectionalLight
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		CachedDirLight = *It;
		break;
	}

	// SkyLight
	for (TActorIterator<ASkyLight> It(GetWorld()); It; ++It)
	{
		CachedSkyLight = *It;
		break;
	}

	// PostProcessVolume
	for (TActorIterator<APostProcessVolume> It(GetWorld()); It; ++It)
	{
		CachedPostProcessVolume = *It;
		break;
	}
}

void AMyGameController::HandleDayNightFade(float Alpha)
{
	// Alpha: 0 = 낮, 1 = 밤
   // Directional Light
	if (CachedDirLight)
	{
		if (UDirectionalLightComponent* DirComp = Cast<UDirectionalLightComponent>(CachedDirLight->GetLightComponent()))
		{
			DirComp->SetIntensity(FMath::Lerp(10.f, 0.1f, Alpha));
			DirComp->SetUseTemperature(true);
			DirComp->SetTemperature(FMath::Lerp(5500.f, 10000.f, Alpha));
			float Pitch = FMath::Lerp(-64.f, -212.f, Alpha);
			if (FMath::Abs(Pitch) > 180)
				CachedDirLight->SetActorRotation(FRotator(Pitch + 180, -15.f, 26.f));
			else
				CachedDirLight->SetActorRotation(FRotator(Pitch, -15.f, 26.f));
		}
	}

	// Sky Light
	if (CachedSkyLight)
	{
		if (USkyLightComponent* SkyComp = Cast<USkyLightComponent>(CachedSkyLight->GetLightComponent()))
		{
			SkyComp->Intensity = FMath::Lerp(1.0f, 0.2f, Alpha);
			SkyComp->RecaptureSky();
		}
	}

	// PostProcessVolume
	if (CachedPostProcessVolume)
	{
		FPostProcessSettings& S = CachedPostProcessVolume->Settings;

		// Bloom
		S.bOverride_BloomIntensity = true;
		S.BloomIntensity = FMath::Lerp(1.2f, 2.5f, Alpha);

		// Exposure
		S.bOverride_AutoExposureBias = true;
		S.AutoExposureBias = FMath::Lerp(1.2f, -2.5f, Alpha);

		// Vignette
		S.bOverride_VignetteIntensity = true;
		S.VignetteIntensity = FMath::Lerp(0.2f, 0.7f, Alpha);

		// Color Saturation
		S.bOverride_ColorSaturation = true;
		S.ColorSaturation = FMath::Lerp(FVector4(1.f, 1.f, 1.f, 1.f), FVector4(0.5f, 0.6f, 1.f, 1.f), Alpha);

		// White Temp & Tint
		S.bOverride_WhiteTemp = true;
		S.WhiteTemp = FMath::Lerp(6000.f, 3000.f, Alpha);

		S.bOverride_WhiteTint = true;
		S.WhiteTint = FMath::Lerp(0.f, -0.1f, Alpha);

		// SceneColorTint
		S.bOverride_SceneColorTint = true;
		S.SceneColorTint = FMath::Lerp(FLinearColor(1.02f, 1.02f, 1.f), FLinearColor(0.7f, 0.7f, 0.8f), Alpha);

		// Ambient Cubemap
		S.bOverride_AmbientCubemapIntensity = true;
		S.AmbientCubemapIntensity = FMath::Lerp(0.8f, 0.3f, Alpha);

		// Indirect Lighting
		S.bOverride_IndirectLightingColor = true;
		S.IndirectLightingColor = FMath::Lerp(FLinearColor(0.95f, 0.98f, 1.05f), FLinearColor(0.6f, 0.6f, 0.7f), Alpha);

		S.bOverride_IndirectLightingIntensity = true;
		S.IndirectLightingIntensity = FMath::Lerp(1.3f, 0.5f, Alpha);

		// Lens Flare
		S.bOverride_LensFlareIntensity = true;
		S.LensFlareIntensity = FMath::Lerp(0.4f, 0.f, Alpha);

		// Bloom Dirt Mask
		S.bOverride_BloomDirtMaskIntensity = true;
		S.BloomDirtMaskIntensity = FMath::Lerp(0.1f, 0.f, Alpha);

		// Depth of Field
		S.bOverride_DepthOfFieldFstop = true;
		S.DepthOfFieldFstop = FMath::Lerp(22.f, 5.f, Alpha);

		// Color Gain
		S.bOverride_ColorGain = true;
		S.ColorGain = FMath::Lerp(FVector4(1.05f, 1.05f, 1.f, 1.f), FVector4(0.9f, 0.9f, 1.f, 1.f), Alpha);
	}
}

void AMyGameController::ToggleDayNight()
{
	bisDay = !bisDay;
	if (!DayNightTimeline || !DayNightCurve) return;


	if (DayNightTimeline->IsPlaying())
	{
		DayNightTimeline->Reverse();
	}
	else
	{
		if (bisDay)  // 낮으로 바꾸기
			DayNightTimeline->ReverseFromEnd();
		else         // 밤으로 바꾸기
			DayNightTimeline->PlayFromStart();
	}
}

