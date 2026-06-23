// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageComponent.h"
#include "../Character/GameCharacterBase.h"
#include "../Status/StatusComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

UDamageComponent::UDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	
}


// Called when the game starts
void UDamageComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}

void UDamageComponent::CalculateDamage(AActor* pClass, float Amount)
{
	if (Amount <= 0.f) return; //Miss~
	if (b_Invincible && Amount > 0.f) return;
	AActor* CB = Cast<AActor>(pClass);
	if (!CB) return;
	UStatusComponent* StatComp = CB->FindComponentByClass<UStatusComponent>();
	if (!StatComp) return;
	StatComp->TakeDamage(Amount);
	
}

void UDamageComponent::CalculateHeal(AActor* pClass, float Amount)
{
	if (Amount <= 0.f) return; 
	
	AGameCharacterBase* CB = Cast<AGameCharacterBase>(pClass);
	if (!CB) return;
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CB->HealNiagara, CB->GetActorLocation(), {}, FVector(1.f, 1.f, 1.f));
	UStatusComponent* StatComp = CB->FindComponentByClass<UStatusComponent>();
	if (!StatComp) return;
	StatComp->ProgressHPMgr(Amount);
}

void UDamageComponent::ApplyCC(const FSkillCCInfo& Info, AActor* Source)
{
	if (b_Invincible) return;
	if (b_SuperArmor && Info.EffectType == ESkillCCType::KnockBack || Info.EffectType == ESkillCCType::KnockFront) return;

	switch (Info.EffectType)
	{
	case ESkillCCType::Stun:		ApplyStun(Info.Duration); break;
	case ESkillCCType::Invincible:	ApplyInvincible(Info.Duration); break;
	case ESkillCCType::Provoke:		ApplyProvoke(Info.Duration); break;
	case ESkillCCType::SuperArmor:	ApplySuperArmor(Info.Duration); break;
	case ESkillCCType::KnockBack:
		if (Source)
		{
			const FVector FromSrc = GetOwner()->GetActorLocation() - Source->GetActorLocation();
			ApplyKnockBack(Info.KnockDistance, FromSrc);
		}
		break;
	case ESkillCCType::KnockFront:
		ApplyKnockFront(Info.KnockDistance, Source);
		break;
	default: break;
	}

}

void UDamageComponent::AddStatusEffect(const FSkillEffectInfo& pStatusEffect)
{
	if (pStatusEffect.EffectType == ESkillEffectType::None) return;
	int32 Index = ActiveStatusEffects.IndexOfByPredicate([&](const FActiveStatusEffect& E)
		{
			return E.EffectData.SkillEffectName == pStatusEffect.SkillEffectName;
		});

	if (Index != INDEX_NONE)
	{
		FActiveStatusEffect& E = ActiveStatusEffects[Index];

		E.CurrentStack = FMath::Clamp(E.CurrentStack + 1, 1, pStatusEffect.MaxStack > 0 ? pStatusEffect.MaxStack : 99);

		E.RemainingDuration = FMath::Max(E.RemainingDuration, pStatusEffect.Duration);

		if (!E.Icon && pStatusEffect.Icon)
			E.Icon = pStatusEffect.Icon;
	}
	else
	{
		ActiveStatusEffects.Add(FActiveStatusEffect(pStatusEffect, pStatusEffect.Duration));
		
	}
}

void UDamageComponent::AddBuff(const FSkillBuffInfo& BuffInfo)
{
	if (BuffInfo.BuffType == ESkillBuffType::None || BuffInfo.Duration <= 0.f)
		return;

	AGameCharacterBase* OwnerChar = Cast<AGameCharacterBase>(GetOwner());
	if (!OwnerChar) return;

	UStatusComponent* StatComp = OwnerChar->FindComponentByClass<UStatusComponent>();
	if (!StatComp) return;

	const int32 Found = FindBuffIndexByName(BuffInfo.SkillBuffName);

	if (Found != INDEX_NONE)
	{
		
		ActiveBuffs[Found].RemainingDuration = FMath::Max(
			ActiveBuffs[Found].RemainingDuration, BuffInfo.Duration
		);

		if (!ActiveBuffs[Found].Icon && BuffInfo.Icon)
			ActiveBuffs[Found].Icon = BuffInfo.Icon;
		return;
	}

	
	StatComp->AddModifier(BuffInfo.BuffStat);

	ActiveBuffs.Add(FActiveBuff(BuffInfo, BuffInfo.Duration));
}

void UDamageComponent::AddBuffFromLevel(const FSkillLevelData& Lvl)
{
	FSkillBuffInfo Info = Lvl.BuffStat;
	if (Lvl.AddDuration > 0.f)
		Info.Duration += Lvl.AddDuration;

	AddBuff(Info);
}

void UDamageComponent::RemoveBuffByInfo(const FSkillBuffInfo& BuffInfo)
{
	const int32 Idx = FindBuffIndexByName(BuffInfo.SkillBuffName);
	if (Idx != INDEX_NONE) RemoveBuffInternal(Idx);
}

void UDamageComponent::RemoveBuffByName(FName BuffName)
{
	const int32 Idx = FindBuffIndexByName(BuffName);
	if (Idx != INDEX_NONE) RemoveBuffInternal(Idx);
}


void UDamageComponent::HandleExpiredStatusEffect(const FActiveStatusEffect& ExpiredEffect)
{
	//종료 이펙트 처리?
}

void UDamageComponent::ApplyDotDamage(const FActiveStatusEffect& Effect)
{
	const float PerTick = 10.f * Effect.CurrentStack; //일단 틱당 10으로 
	AGameCharacterBase* CB = Cast<AGameCharacterBase>(GetOwner());
	if (!CB) return;
	switch (Effect.EffectData.EffectType)
	{
	case ESkillEffectType::None: break;
	case ESkillEffectType::Hurt: CalculateDamage(GetOwner(), PerTick); 
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CB->BloodNiagara, CB->GetActorLocation(), {}, FVector(0.5f, 0.5f, 0.5f));
		break;
	case ESkillEffectType::Iced: CalculateDamage(GetOwner(), PerTick); 
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CB->IcedNiagara, CB->GetActorLocation(), FRotator(180.f, 0.f, 0.f), FVector(0.5f, 0.5f, 0.5f));
		break;
	case ESkillEffectType::Poison: CalculateDamage(GetOwner(), PerTick); 
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CB->PoisonNiagara, CB->GetActorLocation(), {}, FVector(0.5f, 0.5f, 0.5f));
		break;
	case ESkillEffectType::TickHeal: CalculateHeal(GetOwner(), PerTick); 
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CB->HealNiagara, CB->GetActorLocation(), {}, FVector(0.5f, 0.5f, 0.5f));
		break;
	default: break;
	}
}


// Called every frame
void UDamageComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//Buff
	for (int32 i = ActiveBuffs.Num() - 1; i >= 0; --i)
	{
		ActiveBuffs[i].RemainingDuration -= DeltaTime;
		if (ActiveBuffs[i].RemainingDuration <= 0.f)
			RemoveBuffInternal(i);
	}

	for (int32 i = ActiveStatusEffects.Num() - 1; i >= 0; --i)
	{
		FActiveStatusEffect& Effect = ActiveStatusEffects[i];

		Effect.RemainingDuration -= DeltaTime;
		Effect.TickDmgTime += DeltaTime;

		if (Effect.TickDmgTime >= 1.0f)
		{
			ApplyDotDamage(Effect);
			Effect.TickDmgTime = 0.f;
		}

		if (Effect.RemainingDuration <= 0.f)
		{
			HandleExpiredStatusEffect(Effect);
			ActiveStatusEffects.RemoveAt(i);
		}
	}
}



int32 UDamageComponent::FindBuffIndexByName(FName BuffName) const
{
	return ActiveBuffs.IndexOfByPredicate(
		[&](const FActiveBuff& B) { return B.BuffInfo.SkillBuffName == BuffName; }
	);
}

void UDamageComponent::RemoveBuffInternal(int32 Index)
{
	if (!ActiveBuffs.IsValidIndex(Index)) return;

	if (AGameCharacterBase* OwnerChar = Cast<AGameCharacterBase>(GetOwner()))
	{
		if (UStatusComponent* StatComp = OwnerChar->FindComponentByClass<UStatusComponent>())
		{
			
			StatComp->RemoveModifier(ActiveBuffs[Index].BuffInfo.BuffStat);
		}
	}
	ActiveBuffs.RemoveAt(Index);
}

void UDamageComponent::ApplyStun(float Duration)
{
	if (b_Stunned)
		GetWorld()->GetTimerManager().ClearTimer(Timer_Stun);
	b_Stunned = true;

	if (AGameCharacterBase* Char = Cast<AGameCharacterBase>(GetOwner()))
	{
		if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
		{
			MoveComp->DisableMovement();
			//TODO : 스킬시전도 안되게 넣어야할듯
		}
	}
	GetWorld()->GetTimerManager().SetTimer(Timer_Stun, this, &UDamageComponent::ClearStun, Duration, false);
}

void UDamageComponent::ClearStun()
{
	b_Stunned = false;
	if (AGameCharacterBase* Char = Cast<AGameCharacterBase>(GetOwner()))
	{
		{
			if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
			{
				MoveComp->SetMovementMode(MOVE_Walking);
				//TODO : 스킬시전도 가능하게 넣어야할듯
			}
		}
	}
}

void UDamageComponent::ApplyInvincible(float Duration)
{
	if(b_Invincible)
		GetWorld()->GetTimerManager().ClearTimer(Timer_Invincible);
	b_Invincible = true;
	GetWorld()->GetTimerManager().SetTimer(Timer_Invincible, this, &UDamageComponent::ClearInvincible, Duration, false);
}

void UDamageComponent::ClearInvincible()
{
	b_Invincible = false;
	//추후작업
}

void UDamageComponent::ApplyProvoke(float Duration)
{
	if (b_Provoke)
		GetWorld()->GetTimerManager().ClearTimer(Timer_Provoke);
	b_Provoke = true;
	//TODO : 도발 작업 -> 추후
	GetWorld()->GetTimerManager().SetTimer(Timer_Provoke, this, &UDamageComponent::ClearProvoke, Duration, false);
}

void UDamageComponent::ClearProvoke()
{
	b_Provoke = false;
	//추후작업
}

void UDamageComponent::ApplySuperArmor(float Duration)
{
	if (b_SuperArmor)
		GetWorld()->GetTimerManager().ClearTimer(Timer_SuperArmor);
	b_SuperArmor = true;
	//TODO : 강직 작업 -> 추후
	GetWorld()->GetTimerManager().SetTimer(Timer_SuperArmor, this, &UDamageComponent::ClearSuperArmor, Duration, false);
}

void UDamageComponent::ClearSuperArmor()
{
	b_SuperArmor = false;
	//추후작업
}

void UDamageComponent::ApplyKnockBack(float Distance, const FVector& Dir)
{
	if (AGameCharacterBase* Char = Cast<AGameCharacterBase>(GetOwner()))
	{
		const float KnockTime = 0.2f;
		const FVector Vel = Dir.GetSafeNormal() * (Distance / KnockTime);
		Char->LaunchCharacter(Vel, true, false);
		
	}
}

void UDamageComponent::ApplyKnockFront(float Distance, AActor* Source)
{
	if (!Source) return;
	const FVector ToSource = Source->GetActorLocation() - GetOwner()->GetActorLocation();
	const FVector Dir = ToSource.GetSafeNormal();
	ApplyKnockBack(Distance, Dir);
}
