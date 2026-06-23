// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkComponent.h"
#include "PerkStruct.h"
#include "../Status/StatusComponent.h"
#include "Engine/DataTable.h"

UPerkComponent::UPerkComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_AllPerks.Empty();
	m_ActivePerks.Empty();
}

void UPerkComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPerkComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UPerkComponent::AddPerk(UPerk* NewPerk)
{
	if (NewPerk && !m_AllPerks.Contains(NewPerk))
	{
		m_AllPerks.Add(NewPerk);
	}
}

void UPerkComponent::RemovePerk(UPerk* PerkToRemove)
{
	if (PerkToRemove && m_AllPerks.Contains(PerkToRemove))
	{
		// 퍼크가 활성화되어 있다면 먼저 비활성화
		if (PerkToRemove->IsActive())
		{
			PerkToRemove->DeactivatePerk();
		}
		
		m_AllPerks.Remove(PerkToRemove);
	}
}

void UPerkComponent::ActivatePerkByID(const FName& PerkID)
{
	UPerk* Perk = FindPerkByID(PerkID);
	if (Perk)
	{
		Perk->ActivatePerk();
		
		// 활성화된 퍼크 목록에 추가 (중복 방지)
		if (!m_ActivePerks.Contains(Perk))
		{
			m_ActivePerks.Add(Perk);
		}
	}
}

void UPerkComponent::DeactivatePerkByID(const FName& PerkID)
{
	UPerk* Perk = FindPerkByID(PerkID);
	if (Perk)
	{
		Perk->DeactivatePerk();
		
		// 활성화된 퍼크 목록에서 제거
		m_ActivePerks.Remove(Perk);
	}
}

UPerk* UPerkComponent::FindPerkByID(const FName& PerkID)
{
	for (UPerk* Perk : m_AllPerks)
	{
		if (Perk && Perk->GetPerkData().PerkID == PerkID)
		{
			return Perk;
		}
	}
	return nullptr;
}


void UPerkComponent::NotifyAllySpawned(UStatusComponent* NewAllyStatusComp)
{
	if (!NewAllyStatusComp) return;
	
	// m_AppliedAllyStatModifiers 사용: 미리 계산된 모디파이어를 새 아군에게 적용
	ApplyAllAllyStatModifiersToNewAlly(NewAllyStatusComp);
}

void UPerkComponent::AddAppliedAllyStatModifier(const FStatModifier& Modifier)
{
	m_AppliedAllyStatModifiers.Add(Modifier);
}

void UPerkComponent::RemoveAppliedAllyStatModifier(const FStatModifier& Modifier)
{
	m_AppliedAllyStatModifiers.Remove(Modifier);
}

void UPerkComponent::ApplyAllAllyStatModifiersToNewAlly(UStatusComponent* NewAllyStatusComp)
{
	if (!NewAllyStatusComp) 
	{
		return;
	}
	
	// 아군인지 확인
	if (NewAllyStatusComp->GetClassType() != EClassType::Ally) 
	{
		return;
	}
	
	
	// 모든 적용된 모디파이어 적용
	for (const FStatModifier& Modifier : m_AppliedAllyStatModifiers)
	{
		NewAllyStatusComp->AddModifier(Modifier);
	}
	
}

void UPerkComponent::AddAppliedEnemyStatModifier(const FStatModifier& Modifier)
{
	m_AppliedEnemyStatModifiers.Add(Modifier);
}

void UPerkComponent::RemoveAppliedEnemyStatModifier(const FStatModifier& Modifier)
{
	m_AppliedEnemyStatModifiers.Remove(Modifier);
}

void UPerkComponent::ApplyAllEnemyStatModifiersToNewEnemy(UStatusComponent* NewEnemyStatusComp)
{
	if (!NewEnemyStatusComp) return;
	
	// 적군인지 확인
	if (NewEnemyStatusComp->GetClassType() != EClassType::Enemy) return;
	
	// 모든 적용된 모디파이어 적용
	for (const FStatModifier& Modifier : m_AppliedEnemyStatModifiers)
	{
		NewEnemyStatusComp->AddModifier(Modifier);
	}
}

void UPerkComponent::AddAppliedWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier)
{
	if (WeaponType == EWeaponType::None) return;
	
	// 해당 무기 타입의 배열이 없으면 생성
	if (!m_AppliedWeaponStatModifiers.Contains(WeaponType))
	{
		m_AppliedWeaponStatModifiers.Add(WeaponType, FStatModifierArray());
	}
	
	// 모디파이어 추가
	m_AppliedWeaponStatModifiers[WeaponType].Modifiers.Add(Modifier);
}

void UPerkComponent::RemoveAppliedWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier)
{
	if (WeaponType == EWeaponType::None) return;
	
	// 해당 무기 타입의 배열이 있으면 모디파이어 제거
	if (m_AppliedWeaponStatModifiers.Contains(WeaponType))
	{
		m_AppliedWeaponStatModifiers[WeaponType].Modifiers.Remove(Modifier);
		
		// 배열이 비어있으면 맵에서 제거
		if (m_AppliedWeaponStatModifiers[WeaponType].Modifiers.Num() == 0)
		{
			m_AppliedWeaponStatModifiers.Remove(WeaponType);
		}
	}
}

void UPerkComponent::ApplyAllWeaponStatModifiersToNewAlly(UStatusComponent* NewAllyStatusComp)
{
	if (!NewAllyStatusComp) return;
	
	// 아군인지 확인
	if (NewAllyStatusComp->GetClassType() != EClassType::Ally) return;
	
	// 무기 타입 확인
	FName WeaponID = NewAllyStatusComp->GetWeaponID();
	EWeaponType CurrentWeaponType = EWeaponType::None;
	
	// WeaponID를 EWeaponType으로 변환 (아군용: Common Weapon + Only Team Weapon)
	if (WeaponID == FName("Bow")) CurrentWeaponType = EWeaponType::Bow;
	else if (WeaponID == FName("GreatSword")) CurrentWeaponType = EWeaponType::GreatSword;
	else if (WeaponID == FName("Staff")) CurrentWeaponType = EWeaponType::Staff;
	else if (WeaponID == FName("Throwing")) CurrentWeaponType = EWeaponType::Throwing;
	else if (WeaponID == FName("Sword")) CurrentWeaponType = EWeaponType::Sword;
	else if (WeaponID == FName("Shield")) CurrentWeaponType = EWeaponType::Shield;
	else if (WeaponID == FName("catapult")) CurrentWeaponType = EWeaponType::catapult;
	
	if (CurrentWeaponType == EWeaponType::None) return;
	
	// 해당 무기 타입의 모디파이어들 적용
	if (m_AppliedWeaponStatModifiers.Contains(CurrentWeaponType))
	{
		int32 AppliedCount = 0;
		for (const FStatModifier& Modifier : m_AppliedWeaponStatModifiers[CurrentWeaponType].Modifiers)
		{
			NewAllyStatusComp->AddModifier(Modifier);
			AppliedCount++;
		}
	}
}

void UPerkComponent::AddAppliedEnemyWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier)
{
	if (WeaponType == EWeaponType::None) return;
	
	// 해당 무기 타입의 배열이 없으면 생성
	if (!m_AppliedEnemyWeaponStatModifiers.Contains(WeaponType))
	{
		m_AppliedEnemyWeaponStatModifiers.Add(WeaponType, FStatModifierArray());
	}
	
	// 모디파이어 추가
	m_AppliedEnemyWeaponStatModifiers[WeaponType].Modifiers.Add(Modifier);
}

void UPerkComponent::RemoveAppliedEnemyWeaponStatModifier(EWeaponType WeaponType, const FStatModifier& Modifier)
{
	if (WeaponType == EWeaponType::None) return;
	
	// 해당 무기 타입의 배열이 있으면 모디파이어 제거
	if (m_AppliedEnemyWeaponStatModifiers.Contains(WeaponType))
	{
		m_AppliedEnemyWeaponStatModifiers[WeaponType].Modifiers.Remove(Modifier);
		
		// 배열이 비어있으면 맵에서 제거
		if (m_AppliedEnemyWeaponStatModifiers[WeaponType].Modifiers.Num() == 0)
		{
			m_AppliedEnemyWeaponStatModifiers.Remove(WeaponType);
		}
	}
}

void UPerkComponent::ApplyAllEnemyWeaponStatModifiersToNewEnemy(UStatusComponent* NewEnemyStatusComp)
{
	if (!NewEnemyStatusComp) return;
	
	// 적군인지 확인
	if (NewEnemyStatusComp->GetClassType() != EClassType::Enemy) return;
	
	// 무기 타입 확인
	FName WeaponID = NewEnemyStatusComp->GetWeaponID();
	EWeaponType CurrentWeaponType = EWeaponType::None;
	
	// WeaponID를 EWeaponType으로 변환 (적군용: Common Weapon + Only Enemy Weapon)
	if (WeaponID == FName("Bow")) CurrentWeaponType = EWeaponType::Bow;
	else if (WeaponID == FName("GreatSword")) CurrentWeaponType = EWeaponType::GreatSword;
	else if (WeaponID == FName("Staff")) CurrentWeaponType = EWeaponType::Staff;
	else if (WeaponID == FName("Throwing")) CurrentWeaponType = EWeaponType::Throwing;
	else if (WeaponID == FName("Fly")) CurrentWeaponType = EWeaponType::Fly;
	else if (WeaponID == FName("Dagger")) CurrentWeaponType = EWeaponType::Dagger;
	else if (WeaponID == FName("Gantlet")) CurrentWeaponType = EWeaponType::Gantlet;
	else if (WeaponID == FName("Bomb")) CurrentWeaponType = EWeaponType::Bomb;
	else if (WeaponID == FName("Destroyer")) CurrentWeaponType = EWeaponType::Destroyer;
	
	if (CurrentWeaponType == EWeaponType::None) return;
	
	// 해당 무기 타입의 모디파이어들 적용
	if (m_AppliedEnemyWeaponStatModifiers.Contains(CurrentWeaponType))
	{
		int32 AppliedCount = 0;
		for (const FStatModifier& Modifier : m_AppliedEnemyWeaponStatModifiers[CurrentWeaponType].Modifiers)
		{
			NewEnemyStatusComp->AddModifier(Modifier);
			AppliedCount++;
		}
	}
}


UPerk* UPerkComponent::CreatePerkByType(EPerkType PerkType)
{
	UPerk* NewPerk = nullptr;
	
	switch (PerkType)
	{
		case EPerkType::Player:
			NewPerk = NewObject<UPlayerPerk>(this);
			break;
			
		case EPerkType::Item:
			NewPerk = NewObject<UItemPerk>(this);
			break;
			
		case EPerkType::Skill:
			NewPerk = NewObject<USkillPerk>(this);
			break;
			
		case EPerkType::Stat:
			NewPerk = NewObject<UStatPerk>(this);
			break;
			
			
		case EPerkType::RoundBonus:
			NewPerk = NewObject<URoundBonusPerk>(this);
			break;
			
		default:
			break;
	}
	
	return NewPerk;
}

UPerk* UPerkComponent::CreatePerkFromData(const FPerkData& PerkData)
{
	UPerk* NewPerk = CreatePerkByType(PerkData.PerkType);
	if (NewPerk)
	{
		NewPerk->SetPerkData(PerkData);
	}
	
	return NewPerk;
}

// 라운드 관련 함수들 구현
void UPerkComponent::OnRoundStart()
{
    // 활성화된 모든 퍼크들의 OnRoundStart 호출 (라운드 기반 지속시간용)
    // 배열이 비어있으면 처리하지 않음
    if (m_ActivePerks.Num() == 0)
    {
        return;
    }
    
    // RoundExpIncrease 퍼크 개수 확인
    int32 RoundExpIncreaseCount = 0;
    for (UPerk* ActivePerk : m_ActivePerks)
    {
        if (ActivePerk && ActivePerk->GetPerkData().PerkType == EPerkType::RoundBonus)
        {
            if (URoundBonusPerk* RoundBonusPerk = Cast<URoundBonusPerk>(ActivePerk))
            {
                if (RoundBonusPerk->GetPerkData().RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
                {
                    RoundExpIncreaseCount++;
                }
            }
        }
    }
    
    if (RoundExpIncreaseCount > 0)
    {
    }
    
    for (UPerk* ActivePerk : m_ActivePerks)
    {
        if (!ActivePerk) continue;
        
        // 각 퍼크 타입별로 OnRoundStart 호출
        switch (ActivePerk->GetPerkData().PerkType)
        {
            case EPerkType::RoundBonus:
                if (URoundBonusPerk* RoundBonusPerk = Cast<URoundBonusPerk>(ActivePerk))
                {
                    RoundBonusPerk->OnRoundStart();
                }
                break;
                
            case EPerkType::Item:
                if (UItemPerk* ItemPerk = Cast<UItemPerk>(ActivePerk))
                {
                    ItemPerk->OnRoundStart();
                }
                break;
                
            case EPerkType::Skill:
                if (USkillPerk* SkillPerk = Cast<USkillPerk>(ActivePerk))
                {
                    SkillPerk->OnRoundStart();
                }
                break;
                
            case EPerkType::Stat:
                if (UStatPerk* StatPerk = Cast<UStatPerk>(ActivePerk))
                {
                    StatPerk->OnRoundStart();
                }
                break;
                
            default:
                break;
        }
    }
}

void UPerkComponent::OnRoundEnd()
{
	m_CurrentRound++;
	
	// 라운드 기반 효과들 업데이트 (기존 ItemBoost 등)
	// 배열이 비어있으면 처리하지 않음
	if (m_RoundBasedEffects.Num() == 0)
	{
		return;
	}
	
	for (int32 i = m_RoundBasedEffects.Num() - 1; i >= 0; i--)
	{
		// 배열 인덱스 유효성 검증
		if (i < 0 || i >= m_RoundBasedEffects.Num())
		{
			continue;
		}
		
		FRoundBasedEffect& Effect = m_RoundBasedEffects[i];
		
		// 제한된 지속시간이 있는 경우
		if (Effect.RemainingRounds > 0)
		{
			Effect.RemainingRounds--;
			
			// 남은 라운드가 0이 되면 효과 제거
			if (Effect.RemainingRounds <= 0)
			{
				// 해당 퍼크 비활성화
				UPerk* Perk = FindPerkByID(Effect.PerkData.PerkID);
				if (Perk && Perk->IsActive())
				{
					Perk->DeactivatePerk();
				}
				
				// 효과 목록에서 제거 (안전성 검증)
				if (i >= 0 && i < m_RoundBasedEffects.Num())
				{
					m_RoundBasedEffects.RemoveAt(i);
				}
			}
		}
	}
}

void UPerkComponent::UpdateCurrentRound(int32 NewRound)
{
	m_CurrentRound = NewRound;
}

void UPerkComponent::AddRoundBasedEffect(const FRoundBasedEffect& Effect)
{
	if (Effect.PerkData.RoundDuration > 0)
	{
		m_RoundBasedEffects.Add(Effect);
	}
}

void UPerkComponent::RemoveRoundBasedEffect(const FRoundBasedEffect& Effect)
{
	m_RoundBasedEffects.RemoveAll([&Effect](const FRoundBasedEffect& E) {
		return E.PerkData.PerkID == Effect.PerkData.PerkID;
	});
}

// 기본 퍼크 데이터테이블 경로 정의
const FString UPerkComponent::DefaultPerkDataTablePath = TEXT("/Game/Data/Perk/DT_Perk.DT_Perk");

// 데이터테이블 연동 함수들 구현
void UPerkComponent::LoadPerksFromDataTable()
{
	// 기본 데이터테이블 로드
	UDataTable* PerkDataTable = LoadObject<UDataTable>(nullptr, *DefaultPerkDataTablePath);
	if (!PerkDataTable)
	{
		return;
	}

	// 데이터테이블의 모든 행을 순회
	TArray<FPerkData*> AllPerkData;
	PerkDataTable->GetAllRows<FPerkData>(TEXT("LoadPerksFromDataTable"), AllPerkData);
	
	int32 LoadedCount = 0;
	for (FPerkData* PerkData : AllPerkData)
	{
		if (PerkData && PerkData->PerkID != NAME_None)
		{
			// 데이터로부터 퍼크 생성
			UPerk* NewPerk = CreatePerkFromData(*PerkData);
			if (NewPerk)
			{
				AddPerk(NewPerk);
				LoadedCount++;
			}
		}
	}
}

void UPerkComponent::LoadPerkFromDataTableRow(const FName& RowName)
{
	// 기본 데이터테이블 로드
	UDataTable* PerkDataTable = LoadObject<UDataTable>(nullptr, *DefaultPerkDataTablePath);
	if (!PerkDataTable)
	{
		return;
	}

	// 특정 행의 데이터 가져오기
	FPerkData* PerkData = PerkDataTable->FindRow<FPerkData>(RowName, TEXT("LoadPerkFromDataTableRow"));
	if (PerkData && PerkData->PerkID != NAME_None)
	{
		// 데이터로부터 퍼크 생성
		UPerk* NewPerk = CreatePerkFromData(*PerkData);
		if (NewPerk)
		{
			AddPerk(NewPerk);
		}
	}
}

TArray<FPerkData> UPerkComponent::GetAllPerkDataFromTable() const
{
	TArray<FPerkData> Result;
	
	// 기본 데이터테이블 로드
	UDataTable* PerkDataTable = LoadObject<UDataTable>(nullptr, *DefaultPerkDataTablePath);
	if (!PerkDataTable)
	{
		return Result;
	}

	// 데이터테이블의 모든 행을 가져오기
	TArray<FPerkData*> AllPerkData;
	PerkDataTable->GetAllRows<FPerkData>(TEXT("GetAllPerkDataFromTable"), AllPerkData);
	
	for (FPerkData* PerkData : AllPerkData)
	{
		if (PerkData && PerkData->PerkID != NAME_None)
		{
			Result.Add(*PerkData);
		}
	}

	return Result;
}

// 랜덤 퍼크 선택 함수들 구현
TArray<FPerkData> UPerkComponent::GetRandomPerks(int32 Count) const
{
	TArray<FPerkData> Result;
	TArray<FPerkData> AllPerkData = GetAllPerkDataFromTable();
	TArray<FPerkData> FilteredPerks;

	// EnemyStat과 EnemyUnitStat을 제외한 퍼크들만 필터링
	for (const FPerkData& PerkData : AllPerkData)
	{
		if (PerkData.StatEffectType != EStatEffectType::EnemyStat &&
			PerkData.StatEffectType != EStatEffectType::EnemyUnitStat)
		{
			FilteredPerks.Add(PerkData);
		}
	}

	if (FilteredPerks.Num() == 0 || Count <= 0)
	{
		return Result;
	}

	// 요청된 개수가 필터링된 퍼크 개수보다 많으면 모든 필터링된 퍼크 반환
	if (Count >= FilteredPerks.Num())
	{
		return FilteredPerks;
	}

	// 중복없이 랜덤하게 선택
	TArray<int32> SelectedIndices;
	int32 MaxIndex = FilteredPerks.Num() - 1;

	for (int32 i = 0; i < Count; i++)
	{
		int32 RandomIndex;
		do
		{
			RandomIndex = FMath::RandRange(0, MaxIndex);
		} while (SelectedIndices.Contains(RandomIndex));

		SelectedIndices.Add(RandomIndex);
		Result.Add(FilteredPerks[RandomIndex]);
	}

	return Result;
}

TArray<FPerkData> UPerkComponent::GetRandomPerksByRarity(EPerkRarity Rarity, int32 Count) const
{
	TArray<FPerkData> Result;
	TArray<FPerkData> AllPerkData = GetAllPerkDataFromTable();
	TArray<FPerkData> FilteredPerks;
	
	// 지정된 희귀도에 해당하는 퍼크들만 필터링
	for (const FPerkData& PerkData : AllPerkData)
	{
		if (PerkData.Rarity == Rarity)
		{
			FilteredPerks.Add(PerkData);
		}
	}
	
	if (FilteredPerks.Num() == 0 || Count <= 0)
	{
		return Result;
	}
	
	// 요청된 개수가 필터링된 퍼크 개수보다 많으면 모든 필터링된 퍼크 반환
	if (Count >= FilteredPerks.Num())
	{
		return FilteredPerks;
	}
	
	// 중복없이 랜덤하게 선택
	TArray<int32> SelectedIndices;
	int32 MaxIndex = FilteredPerks.Num() - 1;
	
	for (int32 i = 0; i < Count; i++)
	{
		int32 RandomIndex;
		do 
		{
			RandomIndex = FMath::RandRange(0, MaxIndex);
		} 
		while (SelectedIndices.Contains(RandomIndex));
		
		SelectedIndices.Add(RandomIndex);
		Result.Add(FilteredPerks[RandomIndex]);
	}
	
	return Result;
}

TArray<FPerkData> UPerkComponent::GetRandomPerksByType(EPerkType PerkType, int32 Count) const
{
	TArray<FPerkData> Result;
	TArray<FPerkData> AllPerkData = GetAllPerkDataFromTable();
	TArray<FPerkData> FilteredPerks;
	
	// 지정된 타입에 해당하는 퍼크들만 필터링
	for (const FPerkData& PerkData : AllPerkData)
	{
		if (PerkData.PerkType == PerkType)
		{
			FilteredPerks.Add(PerkData);
		}
	}
	
	if (FilteredPerks.Num() == 0 || Count <= 0)
	{
		return Result;
	}
	
	// 요청된 개수가 필터링된 퍼크 개수보다 많으면 모든 필터링된 퍼크 반환
	if (Count >= FilteredPerks.Num())
	{
		return FilteredPerks;
	}
	
	// 중복없이 랜덤하게 선택
	TArray<int32> SelectedIndices;
	int32 MaxIndex = FilteredPerks.Num() - 1;
	
	for (int32 i = 0; i < Count; i++)
	{
		int32 RandomIndex;
		do 
		{
			RandomIndex = FMath::RandRange(0, MaxIndex);
		} 
		while (SelectedIndices.Contains(RandomIndex));
		
		SelectedIndices.Add(RandomIndex);
		Result.Add(FilteredPerks[RandomIndex]);
	}
	
	return Result;
}

TArray<FPerkData> UPerkComponent::GetRandomPerksExcludingActive(int32 Count) const
{
	TArray<FPerkData> Result;
	TArray<FPerkData> AllPerkData = GetAllPerkDataFromTable();
	TArray<FPerkData> AvailablePerks;
	
	// 활성화된 퍼크들의 ID 수집
	TArray<FName> ActivePerkIDs;
	for (UPerk* ActivePerk : m_ActivePerks)
	{
		if (ActivePerk && ActivePerk->IsActive())
		{
			ActivePerkIDs.Add(ActivePerk->GetPerkData().PerkID);
		}
	}
	
	// 활성화되지 않은 퍼크들만 필터링
	for (const FPerkData& PerkData : AllPerkData)
	{
		if (!ActivePerkIDs.Contains(PerkData.PerkID))
		{
			AvailablePerks.Add(PerkData);
		}
	}
	
	if (AvailablePerks.Num() == 0 || Count <= 0)
	{
		return Result;
	}
	
	// 요청된 개수가 사용 가능한 퍼크 개수보다 많으면 모든 사용 가능한 퍼크 반환
	if (Count >= AvailablePerks.Num())
	{
		return AvailablePerks;
	}
	
	// 중복없이 랜덤하게 선택
	TArray<int32> SelectedIndices;
	int32 MaxIndex = AvailablePerks.Num() - 1;
	
	for (int32 i = 0; i < Count; i++)
	{
		int32 RandomIndex;
		do 
		{
			RandomIndex = FMath::RandRange(0, MaxIndex);
		} 
		while (SelectedIndices.Contains(RandomIndex));
		
		SelectedIndices.Add(RandomIndex);
		Result.Add(AvailablePerks[RandomIndex]);
	}
	
	return Result;
}

// UI 표시용 활성화된 퍼크 정보 반환
TArray<FPerkDisplayInfo> UPerkComponent::GetActivePerksForDisplay() const
{
	TArray<FPerkDisplayInfo> Result;
	
	// 활성화된 모든 퍼크를 순회
	for (UPerk* ActivePerk : m_ActivePerks)
	{
		if (!ActivePerk || !ActivePerk->IsActive()) continue;
		
		FPerkData PerkData = ActivePerk->GetPerkData();
		if (PerkData.PerkID == NAME_None) continue;
		
		// 해당 퍼크의 남은 라운드 수 찾기
		int32 RemainingRounds = GetRemainingRoundsForPerk(PerkData.PerkID);
		
		// FPerkDisplayInfo 생성 및 추가
		FPerkDisplayInfo DisplayInfo(PerkData, RemainingRounds);
		Result.Add(DisplayInfo);
	}
	
	return Result;
}

// 특정 퍼크의 남은 라운드 수 반환
int32 UPerkComponent::GetRemainingRoundsForPerk(const FName& PerkID) const
{
	// m_RoundBasedEffects에서 해당 퍼크 찾기
	for (const FRoundBasedEffect& Effect : m_RoundBasedEffects)
	{
		if (Effect.PerkData.PerkID == PerkID)
		{
			return Effect.RemainingRounds - 1;
		}
	}
	
	// 라운드 기반 효과가 아니면 영구 지속 (100 반환)
	return 100;
}