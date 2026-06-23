// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillTrigger.h"
#include "../Character/GameCharacterBase.h"
#include "../Skill/SkillComponent.h"


UAnimInstance* USkillTriggerStrategy::GetAnim(const FSkillContext& Ctx) const
{
	if (!Ctx.Caster) return nullptr;
	AGameCharacterBase* CB = Cast<AGameCharacterBase>(Ctx.Caster);
	if (!CB) return nullptr;

	UAnimInstance* GetAnim = Cast<UAnimInstance>(CB->GetMesh()->GetAnimInstance());
	if (!GetAnim) return nullptr;

	return GetAnim;
}

float USkillTriggerStrategy::PlayMontageIfAny(const FSkillContext& Ctx, UAnimMontage* Montage, float InPlayRate, FName StartSection) const
{
	if (!Montage) return 0.f;
	UAnimInstance* Anim = GetAnim(Ctx);
	if (!Anim) return 0.f;

	const float Len = Anim->Montage_Play(Montage, InPlayRate);
	if (Len > 0.f && StartSection != NAME_None)
		Anim->Montage_JumpToSection(StartSection, Montage);
	/*
	AGameCharacterBase* CB = Cast<AGameCharacterBase>(Ctx.Caster);
	if (!CB) return 0.f;
	if (USkillComponent* SkillComp = CB->FindComponentByClass<USkillComponent>())
	{
		Anim->OnMontageEnded.AddDynamic(SkillComp, &USkillComponent::OnCastSkillStart);
	}
	*/
	return Len;

}

void USkillTriggerStrategy::StopMontageIfAny(const FSkillContext& Ctx, UAnimMontage* Montage, float BlendOut) const
{
	if (!Montage) return;
	

	AGameCharacterBase* CB = Cast<AGameCharacterBase>(Ctx.Caster);
	if (!CB) return;
	if (USkillComponent* SkillComp = CB->FindComponentByClass<USkillComponent>())
	{
		if (UAnimInstance* Anim = GetAnim(Ctx))
		{
			Anim->Montage_Stop(BlendOut, Montage);
			//Anim->OnMontageEnded.AddDynamic(SkillComp, &USkillComponent::OnCastSkillStart);
		}
	}
	
}





void USkillTriggerStrategy::ExecuteTrigger(const FSkillContext& Ctx,  FOnFireWithCtx* OnFire)
{
	//디폴트는 스킬 즉발
	if (OnFire->IsBound())
		OnFire->Execute(Ctx);
}

void USkillTriggerStrategy::CancelTrigger_Implementation()
{
}


void USkillEffect::Apply(const FSkillContext& Ctx, AActor* Target) 
{
	
}
