// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillStruct.h"
#include "../Character/GameCharacterBase.h"
#include "SkillTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "../Damage/DamageComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../Item/SkillProjectile.h"

void USkill::FireOnce_Internal(const FSkillContext& TriggeredCtx)
{
    TArray<AActor*> Targets;
    ETargetAffiliation UsedAffiliation = m_SkillData.TargetAffiliation;

    auto PickFacingPoint = [&]() -> FVector
        {
            const float DefaultRange = (TriggeredCtx.Data ? TriggeredCtx.Data->Range : 800.f);
            const FVector O = TriggeredCtx.Caster->GetActorLocation();
            const FVector Fwd = TriggeredCtx.Caster->GetActorForwardVector();

            if (Targets.Num() > 0 && IsValid(Targets[0]))               return Targets[0]->GetActorLocation();
            if (IsValid(TriggeredCtx.OptionalTarget))                   return TriggeredCtx.OptionalTarget->GetActorLocation();
            if (!TriggeredCtx.OptionalLocation.IsNearlyZero())          return TriggeredCtx.OptionalLocation;

            if (const APawn* P = Cast<APawn>(TriggeredCtx.Caster))
                if (const AController* C = P->GetController())
                    return O + C->GetControlRotation().Vector() * DefaultRange;

            return O + Fwd * DefaultRange;
        };

    // Targeting
    if (TriggeredCtx.Data && TriggeredCtx.Data->TargetingClass)
    {
        if (UTargetingTask* Tgt = NewObject<UTargetingTask>(this, m_SkillData.TargetingClass))
        {
            Tgt->Affiliation = m_SkillData.TargetAffiliation;
            if (UsedAffiliation != ETargetAffiliation::SelfOnly)
                FaceToward2D(TriggeredCtx.Caster, PickFacingPoint());

            Tgt->ResolveTargets(TriggeredCtx, Targets);
            UsedAffiliation = Tgt->Affiliation;
        }
    }

    if (Targets.Num() == 0 && UsedAffiliation == ETargetAffiliation::SelfOnly && IsValid(TriggeredCtx.Caster))
    {
        Targets.AddUnique(TriggeredCtx.Caster);
    }

    if (m_SkillData.ProjectileInfo.ProjectileClass)
    {
        UWorld* W = TriggeredCtx.Caster ? TriggeredCtx.Caster->GetWorld() : nullptr;
        if (W)
        {
            const float DefaultRange = (TriggeredCtx.Data ? TriggeredCtx.Data->Range : 800.f);
            const FVector CasterLoc = TriggeredCtx.Caster->GetActorLocation();
            const FVector Fwd = TriggeredCtx.Caster->GetActorForwardVector();

            auto PickImpactPoint = [&]() -> FVector
                {
                    if (Targets.Num() > 0 && IsValid(Targets[0])) return Targets[0]->GetActorLocation();
                    if (IsValid(TriggeredCtx.OptionalTarget))      return TriggeredCtx.OptionalTarget->GetActorLocation();
                    if (!TriggeredCtx.OptionalLocation.IsNearlyZero()) return TriggeredCtx.OptionalLocation;

                    if (const APawn* P = Cast<APawn>(TriggeredCtx.Caster))
                        if (const AController* C = P->GetController())
                            return CasterLoc + C->GetControlRotation().Vector() * DefaultRange;

                    return CasterLoc + Fwd * DefaultRange;
                };

            FVector ImpactPoint = PickImpactPoint();
            FaceToward2D(TriggeredCtx.Caster, ImpactPoint);

            if (m_SkillData.ProjectileInfo.bSpawnFromSky)
            {
                FHitResult Hit;
                const FVector TraceStart = ImpactPoint + FVector(0, 0, 5000.f);
                const FVector TraceEnd = ImpactPoint - FVector(0, 0, 5000.f);
                FCollisionQueryParams P(SCENE_QUERY_STAT(MeteorTrace), false);
                P.AddIgnoredActor(TriggeredCtx.Caster);

                if (W->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, P))
                {
                    ImpactPoint = Hit.ImpactPoint;
                    FaceToward2D(TriggeredCtx.Caster, ImpactPoint);
                }
            }

            FVector  SpawnLoc = CasterLoc + Fwd * 50.f;
            FRotator SpawnRot = (ImpactPoint - SpawnLoc).Rotation();

            if (m_SkillData.ProjectileInfo.bSpawnFromSky)
            {
                const float H = FMath::Max(200.f, m_SkillData.ProjectileInfo.SkyHeight);
                SpawnLoc = ImpactPoint + FVector(0, 0, H);
                SpawnRot = FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator();
            }

            const FTransform SpawnTM(SpawnRot, SpawnLoc);

            ASkillProjectile* Proj = W->SpawnActorDeferred<ASkillProjectile>(
                m_SkillData.ProjectileInfo.ProjectileClass,
                SpawnTM,
                TriggeredCtx.Caster,
                Cast<APawn>(TriggeredCtx.Caster),
                ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

            if (Proj)
            {
                const bool bHomingWanted =
                    (!m_SkillData.ProjectileInfo.bSpawnFromSky && m_SkillData.ProjectileInfo.bProjectileHoming);

                AActor* HomingTgt = (bHomingWanted ? TriggeredCtx.OptionalTarget : nullptr);
                if (bHomingWanted && !IsValid(HomingTgt))
                    HomingTgt = (Targets.Num() > 0 ? Targets[0] : nullptr);

                Proj->Init(
                    TriggeredCtx,
                    /*Affiliation*/ UsedAffiliation,
                    /*Effects    */ m_SkillData.Effects,
                    /*Speed      */ m_SkillData.ProjectileInfo.ProjectileSpeed,
                    /*Radius     */ m_SkillData.ProjectileInfo.ProjectileRadius,
                    /*bHoming    */ bHomingWanted,
                    /*HomingTgt  */ HomingTgt,
                    /*HomingAcc  */ m_SkillData.ProjectileInfo.HomingAccel,
                    /*ExplodeOnGround*/ m_SkillData.ProjectileInfo.bExplodeOnGround,
                    /*ExplosionRad  */ m_SkillData.ProjectileInfo.ExplosionRadius
                );

                if (auto* Move = Proj->FindComponentByClass<UProjectileMovementComponent>())
                {
                    Move->bInitialVelocityInLocalSpace = false;
                    Move->bRotationFollowsVelocity = true;

                    const FVector Dir = (ImpactPoint - SpawnLoc).GetSafeNormal();
                    Move->ProjectileGravityScale = 0.f;
                    Move->Velocity = Dir * m_SkillData.ProjectileInfo.ProjectileSpeed;
                }

                UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
                //DrawDebugSphere(W, SpawnLoc, 12.f, 16, FColor::Green, false, 2.f);
                //DrawDebugLine(W, SpawnLoc, ImpactPoint, FColor::Orange, false, 2.f, 0, 2.f);
#endif
            }
        }

       
        return;
    }

    if (UsedAffiliation != ETargetAffiliation::SelfOnly)
        FaceToward2D(TriggeredCtx.Caster, PickFacingPoint());

    // Effects + CC
    for (auto Efs : m_SkillData.Effects)
    {
        if (!Efs) continue;

        if (USkillEffect* Effect = NewObject<USkillEffect>(this, Efs))
        {
            for (AActor* T : Targets)
            {
                if (!IsValid(T)) continue;
                if (!TeamFilterUtil::PassAffiliation(TriggeredCtx.Caster, T, m_SkillData.TargetAffiliation)) continue;

                Effect->Apply(TriggeredCtx, T);

                const FSkillLevelData& ExtraData =
                    TriggeredCtx.Data->SkillLevels.IsValidIndex(TriggeredCtx.SkillLevel)
                    ? TriggeredCtx.Data->SkillLevels[TriggeredCtx.SkillLevel]
                    : FSkillLevelData{};

                if (UDamageComponent* DmgComp = T->FindComponentByClass<UDamageComponent>())
                {
                    if (ExtraData.ExtraEffect.EffectType != ESkillEffectType::None)
                        DmgComp->AddStatusEffect(ExtraData.ExtraEffect);

                    if (ExtraData.ExtraCC.EffectType != ESkillCCType::None)
                        DmgComp->ApplyCC(ExtraData.ExtraCC, TriggeredCtx.Caster);
                }
            }
        }
    }
}

USkill::USkill()
{
	m_SkillName = TEXT("");
	m_CurrentLevel = 0;
	m_CoolRemaining = 0.f;

}



const FSkillLevelData& USkill::GetSkillLevelData() const
{
	static FSkillLevelData SData;
	return m_SkillData.SkillLevels.IsValidIndex(m_CurrentLevel) ? m_SkillData.SkillLevels[m_CurrentLevel] : SData;
}

void USkill::SetSkillData(const FSkillData& pData)
{
	this->m_SkillData = pData;
	this->m_CoolRemaining = 0.f;
}

void USkill::SetSkillLevel(int32 nLevel)
{
	this->m_CurrentLevel = nLevel;
}

bool USkill::SkillLevelUp()
{
	if (m_SkillData.SkillLevels.IsValidIndex(m_CurrentLevel + 1))
	{
		m_CurrentLevel++;
		return true;
	}
	return false;
}

bool USkill::ExecuteSkill(AActor* Caster, AActor* OptionalTarget, const FVector& OptionalLocation)
{
    if (!IsReady() || !Caster) return false;

    FSkillContext Ctx;
    Ctx.Caster = Caster;
    Ctx.OptionalTarget = OptionalTarget;
    Ctx.OptionalLocation = OptionalLocation;
    Ctx.SkillLevel = m_CurrentLevel;
    Ctx.Data = &m_SkillData;

    USkillTriggerStrategy* Trigger = m_SkillData.TriggerClass ? NewObject<USkillTriggerStrategy>(this, m_SkillData.TriggerClass) : nullptr;
    if (!Trigger) return false;


    FOnFireWithCtx Delegate = FOnFireWithCtx::CreateLambda([this](const FSkillContext& TriggeredCtx)
        {
            this->FireOnce_Internal(TriggeredCtx);
        });

    Trigger->ExecuteTrigger(Ctx, &Delegate);
    return true;
}

bool USkill::CoolStart()
{
	if (m_SkillData.SkillLevels.IsValidIndex(m_CurrentLevel))
	{
		const float BaseCoolDown = m_SkillData.Cooldown - GetSkillLevelData().CoolRec; //캐릭터 스탯 추가가속까지 더해야함
		m_CoolRemaining = FMath::Max(BaseCoolDown, 0.f);
		return true;
	}
	return false;
}

void USkill::CoolDown(float DeltaTime)
{
	if (m_CoolRemaining > 0.f)
	{
		m_CoolRemaining -= DeltaTime;
		if (m_CoolRemaining < 0.f)
		{
			m_CoolRemaining = 0.f;
		}
	}
}

float USkill::GetCurrentSkillRadius()
{
    float Radius = GetSkillLevelData().AddRadius + m_SkillData.Radius;
    return Radius;
}
