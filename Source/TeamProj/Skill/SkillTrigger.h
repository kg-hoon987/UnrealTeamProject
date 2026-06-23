// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/Function.h"
#include "UObject/NoExportTypes.h"
#include "SkillStruct.h"
#include "TeamFilterUtil.h"
#include "../Character/GameCharacterBase.h"
#include "SkillTrigger.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnFireWithCtx, const FSkillContext&);

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class TEAMPROJ_API USkillTriggerStrategy : public UObject   //-> 트리거실행  몽타주, 캐스팅시간, 발사체 스폰, 노티파이 타이밍 실행담당ㅇ
{
	GENERATED_BODY()
	
protected:
	UAnimInstance* GetAnim(const FSkillContext& Ctx) const;
	float PlayMontageIfAny(const FSkillContext& Ctx, UAnimMontage* Montage, float InPlayRate = 1.f, FName StartSection = NAME_None) const;
	void StopMontageIfAny(const FSkillContext& Ctx, UAnimMontage* Montage, float BlendOut = 0.2f) const;
	
public:
	// OnFire 타이밍에서 타게팅 + 이펙트 실행하면될듯 노티파이, 발사체 힛ㅎ
	virtual void ExecuteTrigger(const FSkillContext& Ctx,  FOnFireWithCtx* OnFire);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill|Trigger")
	void CancelTrigger();
	virtual void CancelTrigger_Implementation(); 

	
	
};


UCLASS(Abstract)
class TEAMPROJ_API UTargetingTask : public UObject							//-> 자신, 자신 주변, 콘범위, 타겟 포인트, 트레이스로 인한 대상 수집
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Targeting")
	ETargetAffiliation Affiliation = ETargetAffiliation::HostileOnly;

	virtual void ResolveTargets(const FSkillContext& Ctx, TArray<AActor*>& OutTargets) PURE_VIRTUAL(UTargetingTask::ResolveTargets, );

};


UCLASS(Abstract)
class TEAMPROJ_API USkillEffect : public UObject						//-> 효과적용 CC기, 상태이상을 타겟에 적용
{
	GENERATED_BODY()
	
public:
	virtual void Apply(const FSkillContext& Ctx, AActor* Target);
};

inline void FaceToward2D(AActor* Caster, const FVector& TargetLoc)
{
	if (!Caster) return;

	FVector O = Caster->GetActorLocation();
	FVector To = TargetLoc - O;
	To.Z = 0.f;

	if (To.IsNearlyZero()) return;

	const FRotator NewYaw(0.f, To.Rotation().Yaw, 0.f);
	Caster->SetActorRotation(NewYaw);

}