// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillStruct.h"
#include "SkillTrigger.h"
#include "SkillComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillReady, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillDataChanged, FName, SkillID);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillAnimNotify, FName Name);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TEAMPROJ_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnyWhere)						TArray<USkill*> m_Skills;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	int32 m_Size;
	UPROPERTY(BlueprintAssignable)				FOnSkillReady OnSkillReady;
	UPROPERTY(BlueprintAssignable)				FOnSkillDataChanged OnSkillDataChanged;
	UPROPERTY(BlueprintReadWrite)				bool isSkillCasting = false;

public:	
	USkillComponent();

protected:
	virtual void BeginPlay() override;
	

public:	
	FOnSkillAnimNotify OnSkillAnimNotify;

	void BroadcastAnimNotify(FName Name)
	{
		OnSkillAnimNotify.Broadcast(Name);
	}

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable)
	void InitSlots(int32 MaxSlots);
	
	UFUNCTION(BlueprintCallable)
	void InitSkill(const TArray<FSkillConfig>& SkillSetConfig, int32 Level);

	UFUNCTION(BlueprintCallable)
	bool TryUseSKill(int32 Index, AActor* OptionalTarget = nullptr, FVector OptionalLocation = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable)
	bool UpgradeSkill(FName SkillID);
	
	UFUNCTION(BlueprintPure)
	USkill* GetSkillSlot(int32 Index) const;

	UFUNCTION()
	int32 GetSkillNum() {return m_Skills.Num(); }





	//프로젝타일
public:

	UPROPERTY()
	int32 PendingSkillIndex = INDEX_NONE;

	UPROPERTY()
	FSkillContext PendingCtx; 
	UPROPERTY(EditAnywhere) bool bUseNotifyFlow = true;

	UFUNCTION(BlueprintCallable)
	bool BeginCast(int32 Index, AActor* OptionalTarget, const FVector& OptionalLocation)
	{
		if (!m_Skills.IsValidIndex(Index)) return false;
		USkill* S = m_Skills[Index];
		if (!S || !S->IsReady()) return false;

		PendingSkillIndex = Index;
		PendingCtx = {};
		PendingCtx.Caster = GetOwner();
		PendingCtx.OptionalTarget = OptionalTarget;
		PendingCtx.OptionalLocation = OptionalLocation;
		PendingCtx.SkillLevel = S->GetSkillLevel();
		PendingCtx.Data = S->GetSkillDataPtr(); // ← 앞서 말한대로 ref/ptr 반환으로 변경했어야 함

		// 몽타주 재생만 함
		if (UAnimMontage* M = S->GetSkillData().Animation)
			if (APawn* P = Cast<APawn>(GetOwner()))
				if (USkeletalMeshComponent* Mesh = P->FindComponentByClass<USkeletalMeshComponent>())
					if (UAnimInstance* Anim = Mesh->GetAnimInstance())
					{
						FaceToward2D(P, OptionalLocation);
						Anim->Montage_Play(M, 1.f);
						S->CoolStart();
					}

		UE_LOG(LogTemp, Warning, TEXT("[BeginCast] Index=%d  Skill=%s"),
			Index, *S->GetSkillData().SkillName);

		return true;
	}

	UFUNCTION(BlueprintCallable)
	bool FirePendingSkillNow()
	{
		if (!m_Skills.IsValidIndex(PendingSkillIndex)) return false;
		USkill* S = m_Skills[PendingSkillIndex];
		if (!S) return false;

		
		S->FireOnce_Internal(PendingCtx);
		

		return true;
	}

	UFUNCTION(BlueprintCallable)
	void OnCastSkillStart(UAnimMontage* Montage, bool bInterrupted);
		
	UFUNCTION(BlueprintCallable)
	void UpdateSkillData(FName SkillID, const FSkillData& NewSkillData);
};


