// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Skill/SkillStruct.h"
#include "DamageComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJ_API UDamageComponent : public UActorComponent
{
	GENERATED_BODY()
	
private:
	int32 FindBuffIndexByName(FName BuffName) const;
	void  RemoveBuffInternal(int32 Index);


	bool b_Stunned = false;			//스턴
	bool b_Invincible = false;		//무적
	bool b_Provoke = false;			//도발
	bool b_Provoked = false;		//도발당함
	bool b_SuperArmor = false;		//강직(면역)

	FTimerHandle Timer_Stun;
	FTimerHandle Timer_Invincible;
	FTimerHandle Timer_Provoke;
	FTimerHandle Timer_Provoked;
	FTimerHandle Timer_SuperArmor;


	void ApplyStun(float Duration);
	void ClearStun();
	void ApplyInvincible(float Duration);
	void ClearInvincible();
	void ApplyProvoke(float Duration);
	void ClearProvoke();
	void ApplySuperArmor(float Duration);
	void ClearSuperArmor();
	
	void ApplyKnockBack(float Distance, const FVector& Dir);
	void ApplyKnockFront(float Distance, AActor* Source);


protected:
	TArray<FActiveBuff> ActiveBuffs;					//디버프
	TArray<FActiveStatusEffect> ActiveStatusEffects;	//상태이상


public:	
	UDamageComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



protected:
	virtual void BeginPlay() override;

public:	
	// CC & StatusEffect
	UFUNCTION(BlueprintCallable)
	void CalculateDamage(AActor* pClass, float Amount);

	UFUNCTION(BlueprintCallable)
	void CalculateHeal(AActor* pClass, float Amount);

	//CC,StatusEffect 적용
	UFUNCTION(BlueprintCallable)
	void ApplyCC(const FSkillCCInfo& Info, AActor* Source);
	UFUNCTION(BlueprintCallable)
	void AddStatusEffect(const FSkillEffectInfo& pStatusEffect);

	//버프디버프 적용
	UFUNCTION(BlueprintCallable)
	void AddBuff(const FSkillBuffInfo& BuffInfo);      

	UFUNCTION(BlueprintCallable)
	void AddBuffFromLevel(const FSkillLevelData& Lvl); 

	UFUNCTION(BlueprintCallable)
	void RemoveBuffByInfo(const FSkillBuffInfo& BuffInfo);

	UFUNCTION(BlueprintCallable)
	void RemoveBuffByName(FName BuffName);

	UFUNCTION(BlueprintPure, Category = "Damage|Buff")
	TArray<FActiveBuff> GetActiveBuffs() const { return ActiveBuffs; }

	UFUNCTION(BlueprintPure, Category = "Damage|StatusEffect")
	TArray<FActiveStatusEffect> GetActiveStatusEffects() const { return ActiveStatusEffects; }


	UFUNCTION(BlueprintCallable)
	void HandleExpiredStatusEffect(const FActiveStatusEffect& ExpiredEffect);
	UFUNCTION(BlueprintCallable)
	void ApplyDotDamage(const FActiveStatusEffect& Effect);

		
};
