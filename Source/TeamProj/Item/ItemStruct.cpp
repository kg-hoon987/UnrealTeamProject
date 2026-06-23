// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemStruct.h"
#include "../Character/GameCharacterBase.h"
#include "../Skill/SkillTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "../Damage/DamageComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../Item/SkillProjectile.h"

UItem::UItem()
{
    m_ItemName = TEXT("");
    m_CoolRemaining = 0.f;
    m_CurrentQuantity = 0;
}

const FSkillLevelData& UItem::GetItemLevelData() const
{
    return m_ItemData.ItemLevel;
}

void UItem::SetItemData(const FItemData& pData, int32 nQuantity)
{
    this->m_ItemData = pData;
    this->m_ItemName = pData.ItemName;
    this->m_CoolRemaining = 0.f;
    SetItemQuantity(nQuantity);
}

void UItem::SetItemQuantity(int32 nQuantity)
{
    this->m_CurrentQuantity = nQuantity;
}

int32 UItem::AddToQuantity(int32 Amount)
{
    if (m_CurrentQuantity + Amount <= m_ItemData.MaxQuantity)
    {
        m_CurrentQuantity += Amount;
    }
    else {
        m_CurrentQuantity = m_ItemData.MaxQuantity;
    }
    return m_CurrentQuantity;
}

int32 UItem::RemoveFromQuantity(int32 Amount)
{
    if (m_CurrentQuantity >= Amount)
    {
        m_CurrentQuantity -= Amount;
    }
    else {
        m_CurrentQuantity = 0;
    }
    return m_CurrentQuantity;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UItem::UseItem(AActor* User, AActor* OptionalTarget, const FVector& OptionalLocation)
{
    if (!IsReady() || !User) return false;

    // FItemContext를 FSkillContext로 변환하여 스킬 시스템 활용
    FSkillContext Ctx;
    Ctx.Caster = User;  // User를 Caster로 변환
    Ctx.OptionalTarget = OptionalTarget;
    Ctx.OptionalLocation = OptionalLocation;
    Ctx.SkillLevel = 0;

    // FSkillData 객체 생성 및 초기화
    FSkillData SkillData;
    SkillData.SkillID = m_ItemData.ItemID;
    SkillData.WeaponType = TEXT("");
    SkillData.SkillName = m_ItemData.ItemName;
    SkillData.Description = m_ItemData.Description;
    SkillData.Icon = m_ItemData.Icon;
    SkillData.Animation = m_ItemData.Animation;
    SkillData.Sound = m_ItemData.Sound;
    SkillData.HalfHeight = m_ItemData.HalfHeight;
    SkillData.Range = m_ItemData.Range;
    SkillData.Radius = m_ItemData.Radius;
    SkillData.ConeHalfAngle = m_ItemData.ConeHalfAngle;
    SkillData.Cooldown = m_ItemData.Cooldown;
    SkillData.CastingTime = m_ItemData.CastingTime;
    SkillData.Duration = m_ItemData.Duration;
    SkillData.ProjectileInfo = m_ItemData.ProjectileInfo;
    SkillData.SkillLevels.Add(m_ItemData.ItemLevel);
    SkillData.TargetAffiliation = m_ItemData.TargetAffiliation;
    SkillData.TriggerClass = m_ItemData.TriggerClass;
    SkillData.TargetingClass = m_ItemData.TargetingClass;
    SkillData.Effects = m_ItemData.Effects;

    Ctx.Data = &SkillData;

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    USkillTriggerStrategy* Trigger = m_ItemData.TriggerClass ? NewObject<USkillTriggerStrategy>(this, m_ItemData.TriggerClass) : nullptr;
    if (!Trigger) return false;

    FOnFireWithCtx Delegate = FOnFireWithCtx::CreateLambda([this](const FSkillContext& TriggeredCtx)
        {
            TArray<AActor*> Targets;
            ETargetAffiliation UsedAffiliation = m_ItemData.TargetAffiliation;

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

            if (TriggeredCtx.Data && TriggeredCtx.Data->TargetingClass)
            {
                if (UTargetingTask* Tgt = NewObject<UTargetingTask>(this, m_ItemData.TargetingClass))
                {
                    Tgt->Affiliation = m_ItemData.TargetAffiliation;
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

            if (m_ItemData.ProjectileInfo.ProjectileClass)
            {
                UWorld* W = TriggeredCtx.Caster ? TriggeredCtx.Caster->GetWorld() : nullptr;
                if (W && TriggeredCtx.Caster->HasAuthority())
                {
                    const float DefaultRange = (TriggeredCtx.Data ? TriggeredCtx.Data->Range : 800.f);
                    const FVector CasterLoc = TriggeredCtx.Caster->GetActorLocation();
                    const FVector Fwd = TriggeredCtx.Caster->GetActorForwardVector();

                    auto PickImpactPoint = [&]() -> FVector
                        {
                            if (Targets.Num() > 0 && IsValid(Targets[0]))
                                return Targets[0]->GetActorLocation();

                            if (IsValid(TriggeredCtx.OptionalTarget))
                                return TriggeredCtx.OptionalTarget->GetActorLocation();

                            if (!TriggeredCtx.OptionalLocation.IsNearlyZero())
                                return TriggeredCtx.OptionalLocation;

                            if (const APawn* P = Cast<APawn>(TriggeredCtx.Caster))
                                if (const AController* C = P->GetController())
                                    return CasterLoc + C->GetControlRotation().Vector() * DefaultRange;

                            return CasterLoc + Fwd * DefaultRange;
                        };

                    FVector ImpactPoint = PickImpactPoint();
                    FaceToward2D(TriggeredCtx.Caster, ImpactPoint);

                    if (m_ItemData.ProjectileInfo.bSpawnFromSky)
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

                    if (m_ItemData.ProjectileInfo.bSpawnFromSky)
                    {
                        const float H = FMath::Max(200.f, m_ItemData.ProjectileInfo.SkyHeight);
                        SpawnLoc = ImpactPoint + FVector(0, 0, H);
                        SpawnRot = FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator();
                    }

                    const FTransform SpawnTM(SpawnRot, SpawnLoc);

                    ASkillProjectile* Proj = W->SpawnActorDeferred<ASkillProjectile>(
                        m_ItemData.ProjectileInfo.ProjectileClass,
                        SpawnTM,
                        TriggeredCtx.Caster,
                        Cast<APawn>(TriggeredCtx.Caster),
                        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

                    if (Proj)
                    {
                        const bool bHomingWanted =
                            (!m_ItemData.ProjectileInfo.bSpawnFromSky && m_ItemData.ProjectileInfo.bProjectileHoming);

                        AActor* HomingTgt = (bHomingWanted ? TriggeredCtx.OptionalTarget : nullptr);

                        if (bHomingWanted && !IsValid(HomingTgt))
                            HomingTgt = (Targets.Num() > 0 ? Targets[0] : nullptr);

                        Proj->Init(
                            TriggeredCtx,
                            UsedAffiliation,
                            m_ItemData.Effects,
                            m_ItemData.ProjectileInfo.ProjectileSpeed,
                            m_ItemData.ProjectileInfo.ProjectileRadius,
                            bHomingWanted,
                            HomingTgt,
                            m_ItemData.ProjectileInfo.HomingAccel,
                            m_ItemData.ProjectileInfo.bExplodeOnGround,
                            m_ItemData.ProjectileInfo.ExplosionRadius
                        );

                        if (auto* Move = Proj->FindComponentByClass<UProjectileMovementComponent>())
                        {
                            Move->bInitialVelocityInLocalSpace = false;
                            Move->bRotationFollowsVelocity = true;

                            if (m_ItemData.ProjectileInfo.bSpawnFromSky)
                            {
                                const FVector Dir = (ImpactPoint - SpawnLoc).GetSafeNormal();
                                Move->ProjectileGravityScale = 0.f;
                                Move->Velocity = Dir * m_ItemData.ProjectileInfo.ProjectileSpeed;
                            }
                            else
                            {
                                const FVector Dir = (ImpactPoint - SpawnLoc).GetSafeNormal();
                                Move->ProjectileGravityScale = 0.f;
                                Move->Velocity = Dir * m_ItemData.ProjectileInfo.ProjectileSpeed;
                            }
                        }

                        const float TravelDist = (ImpactPoint - SpawnLoc).Size();
                        const float Life = (m_ItemData.ProjectileInfo.ProjectileSpeed > KINDA_SMALL_NUMBER)
                            ? (TravelDist / m_ItemData.ProjectileInfo.ProjectileSpeed) + 0.25f
                            : 5.f;
                        Proj->SetLifeSpan(10.f);

                        UGameplayStatics::FinishSpawningActor(Proj, SpawnTM);

                        DrawDebugSphere(W, SpawnLoc, 12.f, 16, FColor::Green, false, 2.f);
                        DrawDebugLine(W, SpawnLoc, ImpactPoint, FColor::Orange, false, 2.f, 0, 2.f);
                    }
                }
                CoolStart();
                return;
            }

            if (UsedAffiliation != ETargetAffiliation::SelfOnly)
                FaceToward2D(TriggeredCtx.Caster, PickFacingPoint());

            for (auto Efs : m_ItemData.Effects)
            {
                if (!Efs) continue;

                if (USkillEffect* Effect = NewObject<USkillEffect>(this, Efs))
                {
                    for (AActor* T : Targets)
                    {
                        if (!IsValid(T)) continue;

                        if (!TeamFilterUtil::PassAffiliation(TriggeredCtx.Caster, T, m_ItemData.TargetAffiliation)) continue;

                        Effect->Apply(TriggeredCtx, T);
                        const FSkillLevelData& ExtraData = TriggeredCtx.Data->SkillLevels.IsValidIndex(0) ? TriggeredCtx.Data->SkillLevels[0] : FSkillLevelData{};

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

            CoolStart();
        });

    Trigger->ExecuteTrigger(Ctx, &Delegate);

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool UItem::CoolStart()
{
    if (m_CurrentQuantity > 0)
    {
        m_CoolRemaining = FMath::Max(m_ItemData.Cooldown, 0.f);
        return true;
    }
    return false;
}

void UItem::CoolDown(float DeltaTime)
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

