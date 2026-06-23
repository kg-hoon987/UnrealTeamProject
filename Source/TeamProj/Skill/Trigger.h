// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TimerManager.h"
#include "SkillTrigger.h"
#include "SkillStruct.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../MyGameAIController.h"
#include "GameFramework/PlayerController.h"
#include "Trigger.generated.h"

/**
 *
 */
UCLASS()
class TEAMPROJ_API UTrigger_Immediate : public USkillTriggerStrategy
{
	GENERATED_BODY()

	public:
	virtual void ExecuteTrigger(const FSkillContext& Ctx, FOnFireWithCtx* OnFire) override
	{
		if (Ctx.Data && Ctx.Data->Animation) PlayMontageIfAny(Ctx, Ctx.Data->Animation, 1.f);

		if (OnFire && OnFire->IsBound()) OnFire->Execute(Ctx);

		// 즉발은 곧바로 종료 = 쿨다운 시작
		if (USkill* OwnerSkill = Cast<USkill>(GetOuter()))
		{
			OwnerSkill->CoolStart();
		}
	}
};



UCLASS()
class TEAMPROJ_API UTrigger_CastTime : public USkillTriggerStrategy
{
	GENERATED_BODY()

	protected:
	FSkillContext   CurrentCtx;
	FOnFireWithCtx  CurrentOnFire;
	FTimerHandle    CastTimeHandle;
	bool            bCasting = false;

	// 시작 때 바라볼 방향 계산 (타깃 > 지정위치 > 현재 전방 우선)
	static FRotator CalcDesiredYaw(const FSkillContext& Ctx)
	{
		check(Ctx.Caster);
		const FVector From = Ctx.Caster->GetActorLocation();

		FVector AimAt = From + Ctx.Caster->GetActorForwardVector() * 100.f;
		if (Ctx.OptionalTarget)
			AimAt = Ctx.OptionalTarget->GetActorLocation();
		else if (!Ctx.OptionalLocation.IsNearlyZero())
			AimAt = Ctx.OptionalLocation;

		FVector To = AimAt - From; To.Z = 0.f;
		if (To.IsNearlyZero())
			return FRotator(0.f, Ctx.Caster->GetActorRotation().Yaw, 0.f);

		return FRotator(0.f, To.Rotation().Yaw, 0.f);
	}

public:
	// 원하는 동작을 기본값으로 셋업
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool  bUseSkillDataCasting = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ExtraCastTime = 0.f;

	// 발사 시 몽타주를 강제로 멈출지 (기본: false = 끊김 방지)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cast")
	bool bStopMontageOnFire = false;

	UFUNCTION(BlueprintCallable)
	void CancelCast()
	{
		if (CurrentCtx.Caster && CurrentCtx.Caster->GetWorld())
			CurrentCtx.Caster->GetWorld()->GetTimerManager().ClearTimer(CastTimeHandle);

		// 취소 시에는 몽타주 정리
		if (CurrentCtx.Data)
			StopMontageIfAny(CurrentCtx, CurrentCtx.Data->Animation);

		bCasting = false;
	}

	virtual void ExecuteTrigger(const FSkillContext& Ctx, FOnFireWithCtx* OnFire) override
	{
		if (!Ctx.Caster) { if (OnFire && OnFire->IsBound()) OnFire->Execute(Ctx); return; }

		// 기존 설계 유지: 트리거 시작 시 쿨다운 시작
		if (USkill* OwnerSkill = Cast<USkill>(GetOuter()))
			OwnerSkill->CoolStart();

		// ★ 1) 시작 순간 한 번만 돌려놓기 (플레이어도 ActorRotation만 스냅)
		if (ACharacter* Char = Cast<ACharacter>(Ctx.Caster))
		{
			const FRotator DesiredYaw = CalcDesiredYaw(Ctx);
			Char->SetActorRotation(DesiredYaw);
		}

		// ★ 2) 루트모션 몽타주 재생 (그 뒤로는 회전/입력/무브먼트 건드리지 않음)
		if (Ctx.Data && Ctx.Data->Animation)
			PlayMontageIfAny(Ctx, Ctx.Data->Animation);

		// 캐스팅 시간 계산
		float BaseCast = 0.f;
		if (bUseSkillDataCasting && Ctx.Data)
		{
			const int32 LV = Ctx.SkillLevel;
			const float LevelRec = (Ctx.Data->SkillLevels.IsValidIndex(LV) ? Ctx.Data->SkillLevels[LV].CastingTimeRec : 0.f);
			BaseCast = Ctx.Data->CastingTime - LevelRec;
		}
		const float CastTime = FMath::Max(0.f, BaseCast + ExtraCastTime);

		// ★ 3) 즉시발동이면 바로 OnFire
		if (CastTime <= KINDA_SMALL_NUMBER)
		{
			if (OnFire && OnFire->IsBound())
				OnFire->Execute(Ctx);
			return;
		}

		// 타이머 세팅
		CurrentCtx = Ctx;
		CurrentOnFire = (OnFire ? *OnFire : FOnFireWithCtx());
		bCasting = true;

		if (UWorld* World = Ctx.Caster->GetWorld())
		{
			World->GetTimerManager().SetTimer(
				CastTimeHandle,
				FTimerDelegate::CreateWeakLambda(this, [this]()
					{
						if (bCasting && CurrentOnFire.IsBound())
							CurrentOnFire.Execute(CurrentCtx);

						bCasting = false;

						// ★ 4) 발사 시점에 몽타주를 멈추지 않음(옵션) → 끊김 방지
						if (bStopMontageOnFire && CurrentCtx.Data)
							StopMontageIfAny(CurrentCtx, CurrentCtx.Data->Animation);
					}),
				CastTime,
				false
			);
		}
	}
};