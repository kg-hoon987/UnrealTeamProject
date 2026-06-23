// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PerkStruct.h"
#include "PerkComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEAMPROJ_API UPerkComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// 모든 퍼크 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Perk")
	TArray<UPerk*> m_AllPerks;

	// 활성화된 퍼크들만 따로 추적
	UPROPERTY()
	TArray<UPerk*> m_ActivePerks;

	// 현재 적용된 아군 스탯 모디파이어들 (새로 소환되는 아군에게 적용)
	UPROPERTY()
	TArray<FStatModifier> m_AppliedAllyStatModifiers;

	// 현재 적용된 적군 스탯 모디파이어들 (새로 소환되는 적군에게 적용)
	UPROPERTY()
	TArray<FStatModifier> m_AppliedEnemyStatModifiers;

	// 무기 타입별 아군 스탯 모디파이어들 (새로 소환되는 특정 무기 타입 아군에게 적용)
	UPROPERTY()
	TMap<EWeaponType, FStatModifierArray> m_AppliedWeaponStatModifiers;

	// 무기 타입별 적군 스탯 모디파이어들 (새로 소환되는 특정 무기 타입 적군에게 적용)
	UPROPERTY()
	TMap<EWeaponType, FStatModifierArray> m_AppliedEnemyWeaponStatModifiers;


	// 라운드 기반 지속시간을 가진 효과들
	UPROPERTY()
	TArray<FRoundBasedEffect> m_RoundBasedEffects;

	// 현재 라운드 수
	UPROPERTY()
	int32 m_CurrentRound = 0;

public:
	UPerkComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 퍼크 관리 함수들
	UFUNCTION(BlueprintCallable)
	void AddPerk(UPerk* NewPerk);

	UFUNCTION(BlueprintCallable)
	void RemovePerk(UPerk* PerkToRemove);

	UFUNCTION(BlueprintCallable)
	void ActivatePerkByID(const FName& PerkID);

	UFUNCTION(BlueprintCallable)
	void DeactivatePerkByID(const FName& PerkID);

	UFUNCTION(BlueprintCallable)
	UPerk* FindPerkByID(const FName& PerkID);

	UFUNCTION(BlueprintCallable)
	TArray<UPerk*> GetAllPerks() const { return m_AllPerks; }

	// 활성화된 퍼크 목록 반환
	UFUNCTION(BlueprintCallable)
	TArray<UPerk*> GetActivePerks() const { return m_ActivePerks; }

	// 라운드 기반 지속시간을 가진 퍼크 목록 반환 (남은 라운드 정보 포함)
	UFUNCTION(BlueprintCallable)
	TArray<FRoundBasedEffect> GetRemainingPerks() const { return m_RoundBasedEffects; }


	// 아군 소환 시 호출되는 함수 (외부에서 호출)
	UFUNCTION(BlueprintCallable)
	void NotifyAllySpawned(class UStatusComponent* NewAllyStatusComp);

	// 적용된 아군 스탯 모디파이어 관리
	UFUNCTION(BlueprintCallable)
	void AddAppliedAllyStatModifier(const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void RemoveAppliedAllyStatModifier(const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void ApplyAllAllyStatModifiersToNewAlly(class UStatusComponent* NewAllyStatusComp);

	// 적용된 적군 스탯 모디파이어 관리
	UFUNCTION(BlueprintCallable)
	void AddAppliedEnemyStatModifier(const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void RemoveAppliedEnemyStatModifier(const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void ApplyAllEnemyStatModifiersToNewEnemy(class UStatusComponent* NewEnemyStatusComp);

	// 무기 타입별 아군 스탯 모디파이어 관리
	UFUNCTION(BlueprintCallable)
	void AddAppliedWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void RemoveAppliedWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void ApplyAllWeaponStatModifiersToNewAlly(class UStatusComponent* NewAllyStatusComp);

	// 무기 타입별 적군 스탯 모디파이어 관리
	UFUNCTION(BlueprintCallable)
	void AddAppliedEnemyWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void RemoveAppliedEnemyWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable)
	void ApplyAllEnemyWeaponStatModifiersToNewEnemy(class UStatusComponent* NewEnemyStatusComp);


	// 퍼크 팩토리 함수
	UFUNCTION(BlueprintCallable)
	UPerk* CreatePerkByType(EPerkType PerkType);

	// 퍼크 데이터로부터 퍼크 생성
	UFUNCTION(BlueprintCallable)
	UPerk* CreatePerkFromData(const FPerkData& PerkData);

	// 라운드 관련 함수들
	UFUNCTION(BlueprintCallable)
	void OnRoundStart();

	UFUNCTION(BlueprintCallable)
	void OnRoundEnd();

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentRound(int32 NewRound);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentRound() const { return m_CurrentRound; }

	// 라운드 기반 효과 관리
	UFUNCTION(BlueprintCallable)
	void AddRoundBasedEffect(const FRoundBasedEffect& Effect);

	UFUNCTION(BlueprintCallable)
	void RemoveRoundBasedEffect(const FRoundBasedEffect& Effect);

	// 데이터테이블 연동 함수들
	UFUNCTION(BlueprintCallable)
	void LoadPerksFromDataTable();

	UFUNCTION(BlueprintCallable)
	void LoadPerkFromDataTableRow(const FName& RowName);

	UFUNCTION(BlueprintCallable)
	TArray<FPerkData> GetAllPerkDataFromTable() const;

	// 랜덤 퍼크 선택 함수들
	UFUNCTION(BlueprintCallable)
	TArray<FPerkData> GetRandomPerks(int32 Count = 3) const;

	UFUNCTION(BlueprintCallable)
	TArray<FPerkData> GetRandomPerksByRarity(EPerkRarity Rarity, int32 Count = 3) const;

	UFUNCTION(BlueprintCallable)
	TArray<FPerkData> GetRandomPerksByType(EPerkType PerkType, int32 Count = 3) const;

	UFUNCTION(BlueprintCallable)
	TArray<FPerkData> GetRandomPerksExcludingActive(int32 Count = 3) const;

	// UI 표시용 활성화된 퍼크 정보 반환
	UFUNCTION(BlueprintCallable)
	TArray<FPerkDisplayInfo> GetActivePerksForDisplay() const;

	// 특정 퍼크의 남은 라운드 수 반환
	UFUNCTION(BlueprintCallable)
	int32 GetRemainingRoundsForPerk(const FName& PerkID) const;

private:
	// 기본 퍼크 데이터테이블 경로
	static const FString DefaultPerkDataTablePath;
};