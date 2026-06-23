// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusStruct.h"
#include "GameFramework/Actor.h"
#include "StatusComponent.generated.h"

class ABuildingBase;


UENUM(BlueprintType)
enum class EStatusOwnerType : uint8
{
	Character,
	Building
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDiedSignature, AActor*, OwnerActor);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJ_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	//UPROPERTY(BlueprintReadOnly)
	UPROPERTY() UDataTable* m_StatTable = nullptr;
	UPROPERTY() EClassType  m_ClassType = EClassType::None;
	UPROPERTY() FName       m_ClassID = NAME_None;
	UPROPERTY() FName       m_WeaponID = NAME_None;
	UPROPERTY() int32       m_Level = 1;
	
	UPROPERTY(BlueprintReadOnly)		FBaseStat m_BaseStat;
	UPROPERTY()							FStatModifier m_WeaponStat;
	UPROPERTY()							TArray<FStatModifier> m_StatModifier;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EStatusOwnerType OwnerType = EStatusOwnerType::Character;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* DeadMontage{};


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HPRegenPerSec = 0.f;

public:
	UPROPERTY(BlueprintReadOnly)		FCurrentStat m_CurrentStat;
	UPROPERTY(BlueprintReadOnly)		FBaseStat m_FinalStat;
	
public:	
	UStatusComponent();

	UPROPERTY(BlueprintAssignable)
	FOnDiedSignature OnDied;


	UFUNCTION(BlueprintCallable)
	void Init(const EClassType ClassType, const FClassConfig& pClassSetConfig);

	UFUNCTION(BlueprintCallable)
	void InitFromBaseStat(const FBaseStat& InBase, EStatusOwnerType InOwnerType = EStatusOwnerType::Building, bool bResetHP = true);

	UFUNCTION(BlueprintCallable)
	void SetBaseStat(const FBaseStat& InBase, bool bKeepHpPct = true);

	UFUNCTION(BlueprintCallable)
	FCurrentStat GetCurrentStat() { return m_CurrentStat; }

	UFUNCTION(BlueprintCallable)
	void AddModifier(const FStatModifier& pModi);

	UFUNCTION(BlueprintCallable)
	void RemoveModifier(const FStatModifier& pModi);

	UFUNCTION(BlueprintCallable)
	void ReCalculationStat();

	UFUNCTION(BlueprintCallable)
	void TakeDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable)
	void FullHPUpdate()
	{
		m_CurrentStat.CntHP = m_FinalStat.MaxHP;
	}


	UPROPERTY(EditAnywhere, BlueprintReadOnly)	float m_HPFrontRatio;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	float m_HPBackRatio;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)	bool m_bHPRotationRatio;

	FTimerHandle Timer_StartHPProgress;
	void StartHPProgress();

	UFUNCTION(BlueprintCallable, Category = "Stat")
	void ProgressHPMgr(float Amount);

	//OnlyPlyer
	UFUNCTION(BlueprintCallable)
	void AddExp(int32 ExpAmount);

	UFUNCTION(BlueprintCallable)
	void LevelUp();

	UFUNCTION(BlueprintCallable)
	void OnDeath(bool isDead);

	float destroyStart = 0.f;
	
	UFUNCTION(BlueprintCallable)
	void DestroyChar() { GetOwner()->Destroy(); }

	//OnlyBuilding
	UFUNCTION(BlueprintCallable)
	void ResetHPToMax() { m_CurrentStat.CntHP = m_FinalStat.MaxHP; }

	UFUNCTION(BlueprintCallable)
	void SetHPRegenPerSec(float InValue) { HPRegenPerSec = InValue; }

	UFUNCTION(BlueprintPure) float GetHP() const { return m_CurrentStat.CntHP; }
	UFUNCTION(BlueprintPure) float GetMaxHP() const { return m_FinalStat.MaxHP; }
	UFUNCTION(BlueprintPure) bool  IsDead() const;
	
	UFUNCTION(BlueprintCallable)
	EClassType GetClassType(){ return m_ClassType; }

	UFUNCTION(BlueprintCallable)
	FName GetWeaponID() { return m_WeaponID; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
	UFUNCTION()
	void SpawnGold(int32 Gold);
	UFUNCTION()
	void SpawnExp(int32 Exp);
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
