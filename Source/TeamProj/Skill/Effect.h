// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTrigger.h"
#include "../Damage/DamageComponent.h"
#include "../Status/StatusComponent.h"
#include "../Character/GameCharacterBase.h"
#include "Effect.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API UEffect_Damage : public USkillEffect
{
	GENERATED_BODY()
	
public:
	virtual void Apply(const FSkillContext& Ctx, AActor* Target) override
	{
		if (!Ctx.Data || !Target) return;

		if (!TeamFilterUtil::PassAffiliation(Ctx.Caster, Target, ETargetAffiliation::HostileOnly))
			return;

		const auto TA = TeamFilterUtil::GetTeamIdOf(Ctx.Caster);
		const auto TB = TeamFilterUtil::GetTeamIdOf(Target);
		const auto ATT = TeamFilterUtil::QueryAttitudeController(Ctx.Caster, Target);

		if (ATT != ETeamAttitude::Hostile)
		{
			UE_LOG(LogTemp, Warning, TEXT("[DMG-GUARD] blocked (not hostile)"));
			return;
		}

		const float AddDmg = (Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddDamage : 0.f); 

		if (UDamageComponent* DamageComp = Target->FindComponentByClass<UDamageComponent>())
		{
			AGameCharacterBase* pCaster = Cast<AGameCharacterBase>(Ctx.Caster);
			if (!pCaster) return;

			UStatusComponent* CasterStatComp = pCaster->FindComponentByClass<UStatusComponent>();
			if (!CasterStatComp) return;

			AActor* pTarget = Cast<AActor>(Target);
			if (!pTarget) return;

			UStatusComponent* TargetStatComp = pTarget->FindComponentByClass<UStatusComponent>();
			if (!TargetStatComp) return;


			float CritRValue = FMath::FRand();

			const bool bIsCritR = CritRValue < CasterStatComp->m_FinalStat.CritRate;
			
			const float BeforeCritDmg = CasterStatComp->m_FinalStat.Attack + AddDmg;

			float AfterCritDmg;
			if (bIsCritR) AfterCritDmg = BeforeCritDmg * (1.f + CasterStatComp->m_FinalStat.CritDamage / 100.f);
			else          AfterCritDmg = BeforeCritDmg;
		
			float TargetDef = (100 - TargetStatComp->m_FinalStat.Defense) / 100;
			const float FinalDmg = AfterCritDmg*TargetDef;

			DamageComp->CalculateDamage(Target,FinalDmg); 
		}
	}
};



UCLASS()
class TEAMPROJ_API UEffect_Heal : public USkillEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal")
	float AttackScale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heal")
	bool bClampToMaxHP = true;

	virtual void Apply(const FSkillContext& Ctx, AActor* Target) override
	{
		if (!Ctx.Caster || !Target || !Ctx.Data) return;

		if (!Ctx.Caster->HasAuthority()) return;

		if (!TeamFilterUtil::PassAffiliation(Ctx.Caster, Target, ETargetAffiliation::FriendlyOnly))
		{
			return;
		}

		const float BaseHeal = (Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddDamage : 0.f);

		float FinalHeal = BaseHeal;

		if (const AGameCharacterBase* P = Cast<AGameCharacterBase>(Ctx.Caster))
		{
			if (const UStatusComponent* StatComp = P->FindComponentByClass<UStatusComponent>())
				AttackScale += StatComp->m_FinalStat.Attack * AttackScale;
		}

		if (FinalHeal < 0.f) return;

		if (AGameCharacterBase* T = Cast<AGameCharacterBase>(Target))
			if (UDamageComponent* DmgComp = T->FindComponentByClass<UDamageComponent>())
			{
				DmgComp->CalculateHeal(T, FinalHeal);
			}
	}
};



UCLASS()
class TEAMPROJ_API UEffect_AddBuff : public USkillEffect
{
	GENERATED_BODY()
	
public:
	virtual void Apply(const FSkillContext& Ctx, AActor* Target)  override 
	{
		if (!Ctx.Data || !Target) return;
		if (!Ctx.Caster || !Ctx.Caster->HasAuthority()) return;

		
		if (!TeamFilterUtil::PassAffiliation(Ctx.Caster, Target, ETargetAffiliation::FriendlyOnly))
			return;

		const int32 SkillLevel = Ctx.SkillLevel;
		if (!Ctx.Data->SkillLevels.IsValidIndex(SkillLevel)) return;

		const FSkillLevelData& L = Ctx.Data->SkillLevels[SkillLevel];

		
		const float FinalDur = FMath::Max(0.f, Ctx.Data->Duration + L.AddDuration);
		if (FinalDur <= 0.f) return;

		
		FSkillBuffInfo Info = L.BuffStat;
		Info.Duration = FinalDur;

		
		if (Info.BuffType == ESkillBuffType::None)
			Info.BuffType = ESkillBuffType::Buff;


		if (UDamageComponent* DmgComp = Target->FindComponentByClass<UDamageComponent>())
		{
			DmgComp->AddBuff(Info); 
		}
	}
};

UCLASS()
class TEAMPROJ_API UEffect_AddDeBuff : public USkillEffect
{
	GENERATED_BODY()

	public:
	virtual void Apply(const FSkillContext& Ctx, AActor* Target)  override
	{
		if (!Ctx.Data || !Target) return;
		if (!Ctx.Caster || !Ctx.Caster->HasAuthority()) return;

		
		if (!TeamFilterUtil::PassAffiliation(Ctx.Caster, Target, ETargetAffiliation::HostileOnly))
			return;

		const int32 SkillLevel = Ctx.SkillLevel;
		if (!Ctx.Data->SkillLevels.IsValidIndex(SkillLevel)) return;

		const FSkillLevelData& L = Ctx.Data->SkillLevels[SkillLevel];

		const float FinalDur = FMath::Max(0.f, Ctx.Data->Duration + L.AddDuration);
		if (FinalDur <= 0.f) return;

		FSkillBuffInfo Info = L.BuffStat;
		Info.Duration = FinalDur;

		
		Info.BuffType = ESkillBuffType::DeBuff;

		if (UDamageComponent* DmgComp = Target->FindComponentByClass<UDamageComponent>())
		{
			DmgComp->AddBuff(Info); 
		}
	}
};