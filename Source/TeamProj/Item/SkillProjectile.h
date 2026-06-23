// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericTeamAgentInterface.h"
#include "../Skill/TeamFilterUtil.h"
#include "../Skill/SkillStruct.h"
#include "SkillProjectile.generated.h"

class UProjectileMovementComponent;
class UNiagaraComponent;
class USkillEffect;

UCLASS()
class TEAMPROJ_API ASkillProjectile : public AActor
{
	GENERATED_BODY()

	public:
	ASkillProjectile();

	UPROPERTY(VisibleAnywhere)  class USphereComponent* Sphere;
	UPROPERTY(VisibleAnywhere) UStaticMeshComponent* Mesh = nullptr;
	UPROPERTY(VisibleAnywhere)	UProjectileMovementComponent* Move;
	UPROPERTY(VisibleAnywhere)	UNiagaraComponent* VFX;

	void Init(const FSkillContext& InCtx,
		ETargetAffiliation InAffiliation,
		const TArray<TSubclassOf<class USkillEffect>>& InEffects,
		float InSpeed, float InRadius,
		bool bHoming, AActor* HomingTarget, float HomingAcceleration,
		bool bInExplodeOnGround = false, float InExplosionRadius = 0.f);


	//관통쪽
	UPROPERTY(EditAnywhere) int32 MaxPenetrations = 0; // 0 -> 소멸 1 -> 관통쓰~
	int32 Penetrations = 0;
	TSet<TWeakObjectPtr<AActor>> HitOnce; //중복방지

	//폭발(지면 히트)
	bool bExplodeOnGround = true;
	float ExplosionRadius = 300.f;

	//폭발시 수집할 타겟들
	TArray<TEnumAsByte<EObjectTypeQuery>> ExplosionObjectTypes;

	//폭발 연출..
	UPROPERTY(EditAnywhere, Category = ExplosionEffect)
	UNiagaraSystem* HitFX = nullptr;
	UPROPERTY(EditAnywhere, Category = ExplosionEffect)
	USoundBase* HitSFX = nullptr;
	UPROPERTY(EditAnywhere, Category = ExplosionEffect)
	UNiagaraSystem* ExplosionFX = nullptr;
	UPROPERTY(EditAnywhere, Category = ExplosionEffect)
	USoundBase* ExplosionSFX = nullptr;

	int32 towerDmg = 0;

protected:
	virtual void BeginPlay() override;

	UFUNCTION() void OnSphereOverlap(UPrimitiveComponent* Overlapped, AActor* Other,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepHit);

	UFUNCTION() void OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ApplyEffectsToActor(AActor* Target);
	void ExplodeAt(const FVector& Center);

	void HitAt(const FVector& Center);


	UPROPERTY() TWeakObjectPtr<AActor> Caster;
	FSkillContext Ctx;
	ETargetAffiliation Affiliation = ETargetAffiliation::HostileOnly;
	UPROPERTY()  TArray<TSubclassOf<class USkillEffect>> Effects;




	};
