// Fill out your copyright notice in the Description page of Project Settings.

#include "PerkStruct.h"
#include "../MyGamePlayerState.h"
#include "../Item/ItemStruct.h"
#include "../Item/ItemComponent.h"
#include "../Skill/SkillComponent.h"
#include "../Status/StatusComponent.h"
#include "../Character/GameCharacterBase.h"
#include "PerkComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UPerk::UPerk()
{
    m_PerkName = TEXT("");
    m_bIsActive = false;
}

void UPerk::SetPerkData(const FPerkData& pData)
{
    this->m_PerkData = pData;
    this->m_PerkName = pData.PerkName;
}

void UPerk::ActivatePerk()
{
    if (!m_bIsActive)
    {
        m_bIsActive = true;
        OnPerkActivated();
    }
}

void UPerk::DeactivatePerk()
{
    if (m_bIsActive)
    {
        m_bIsActive = false;
        OnPerkDeactivated();
    }
}

void UPerk::TogglePerk()
{
    if (m_bIsActive)
    {
        DeactivatePerk();
    }
    else
    {
        ActivatePerk();
    }
}

// 기본 구현 (자식 클래스에서 오버라이드)
void UPerk::OnPerkActivated_Implementation()
{
    // 기본 구현은 비어있음
}

void UPerk::OnPerkDeactivated_Implementation()
{
    // 기본 구현은 비어있음
}

// UItemEffectStrategy 기본 구현
bool UItemEffectStrategy::ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

bool UItemEffectStrategy::RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

// USkillEffectStrategy 기본 구현
bool USkillEffectStrategy::ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

bool USkillEffectStrategy::RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

// UItemQuantityEffect 구현
bool UItemQuantityEffect::ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World || PerkData.ItemID == NAME_None) return false;
    
    // 플레이어 캐릭터 찾기 (PlayerState가 아닌 PlayerCharacter에서 ItemComponent 찾기)
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
    if (!ItemComponent) return false;
    
    int32 QuantityIncrease = FMath::RoundToInt(PerkData.Value);
    bool bSuccess = ItemComponent->AddItemQuantity(PerkData.ItemID, QuantityIncrease);
    
    if (bSuccess)
    {
        
        // 일회성 효과이므로 즉시 비활성화
        AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
        if (MyPS && MyPS->PerkComp)
        {
            MyPS->PerkComp->DeactivatePerkByID(PerkData.PerkID);
        }
    }
    else
    {
    }
    
    return bSuccess;
}

bool UItemQuantityEffect::RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    // 일회성 효과이므로 제거할 필요 없음 (아이템 수량은 그대로 유지)
    return true;
}

// UItemBoostEffect 구현
UItemBoostEffect::UItemBoostEffect()
{
    // 멤버 변수 초기화
    m_OriginalItemData = FItemData();
    m_RoundBasedEffect = FRoundBasedEffect();
    m_CurrentRound = 0;
    m_bIsActivated = false;
    m_bWaitingForNextRound = false;
}

bool UItemBoostEffect::ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World || PerkData.ItemID == NAME_None) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
    if (!ItemComponent) return false;
    
    // 아이템 데이터 가져오기
    UItem* Item = ItemComponent->GetAllItem(PerkData.ItemID);
    if (!Item) return false;
    
    // 원본 아이템 데이터 저장 (복원용)
    m_OriginalItemData = Item->GetItemData();
    
    // 라운드 기반 지속시간 설정
    m_RoundBasedEffect = FRoundBasedEffect(PerkData, m_CurrentRound);
    
    // 지연 활성화 설정
    m_bIsActivated = false;
    m_bWaitingForNextRound = true;
    
    // 원본 아이템 데이터를 미리 저장해두기 (OnRoundStart에서 사용)
    
    if (PerkData.RoundDuration > 0)
    {
    }
    else
    {
    }
    return true;
}

bool UItemBoostEffect::RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World || PerkData.ItemID == NAME_None) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
    if (!ItemComponent) return false;
    
    // 아이템 데이터 가져오기
    UItem* Item = ItemComponent->GetAllItem(PerkData.ItemID);
    if (!Item) return false;
    
    // 원본 아이템 데이터로 복원 (활성화된 상태에서만)
    if (m_bIsActivated && m_OriginalItemData.ItemID != NAME_None)
    {
        // ItemComponent를 통해 아이템 데이터 복원 (UI 알림 포함)
        ItemComponent->UpdateItemData(PerkData.ItemID, m_OriginalItemData);
        
    }
    else if (m_bIsActivated)
    {
    }
    
    // 상태 초기화
    m_bIsActivated = false;
    m_bWaitingForNextRound = false;
    
    return true;
}

void UItemBoostEffect::OnRoundStart()
{
    // 다음 라운드를 기다리는 중이면 실제 효과 적용
    if (m_bWaitingForNextRound && !m_bIsActivated)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 플레이어 캐릭터 찾기
        APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
        if (!Pawn) return;
        
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
        if (!PlayerCharacter) return;
        
        UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
        if (!ItemComponent) return;
        
        // 아이템 데이터 가져오기
        UItem* Item = ItemComponent->GetAllItem(m_RoundBasedEffect.PerkData.ItemID);
        if (!Item) return;
        
        // 원본 아이템 데이터가 유효한지 확인
        if (m_OriginalItemData.ItemID == NAME_None)
        {
            return;
        }
        
        // 새로운 아이템 데이터로 교체 (비어있는 항목은 원본 유지)
        FItemData NewItemData = m_OriginalItemData; // 원본 데이터로 시작
        FItemData OverrideData = m_RoundBasedEffect.PerkData.ItemDataOverride;
        
        // 비어있지 않은 항목만 덮어쓰기
        if (OverrideData.ItemName != TEXT(""))
            NewItemData.ItemName = OverrideData.ItemName;
        if (OverrideData.Description != TEXT(""))
            NewItemData.Description = OverrideData.Description;
        if (OverrideData.Icon != nullptr)
            NewItemData.Icon = OverrideData.Icon;
        if (OverrideData.MaxQuantity > 0)
            NewItemData.MaxQuantity = OverrideData.MaxQuantity;
        if (OverrideData.Cost > 0)
            NewItemData.Cost = OverrideData.Cost;
        
        // ItemID는 항상 원본 유지
        NewItemData.ItemID = m_RoundBasedEffect.PerkData.ItemID;
        
        // ItemComponent를 통해 아이템 데이터 업데이트 (UI 알림 포함)
        ItemComponent->UpdateItemData(m_RoundBasedEffect.PerkData.ItemID, NewItemData);
        
        m_bIsActivated = true;
        m_bWaitingForNextRound = false;
        
    }
    
    // 이미 활성화된 상태에서 라운드 지속시간 관리
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        
        // 남은 라운드가 0이 되면 효과 제거
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveEffect_Implementation(m_RoundBasedEffect.PerkData, World);
            }
        }
    }
}


void UItemBoostEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}

void UItemBoostEffect::SetPerkData(const FPerkData& PerkData)
{
    // 라운드 기반 지속시간 설정
    m_RoundBasedEffect = FRoundBasedEffect(PerkData, m_CurrentRound);
    
    // 지연 활성화 설정
    m_bIsActivated = false;
    m_bWaitingForNextRound = true;
    
}

// USkillBoostEffect 구현
USkillBoostEffect::USkillBoostEffect()
{
    // 멤버 변수 초기화
    m_OriginalSkillData = FSkillData();
    m_RoundBasedEffect = FRoundBasedEffect();
    m_CurrentRound = 0;
    m_bIsActivated = false;
    m_bWaitingForNextRound = false;
}

bool USkillBoostEffect::ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    
    if (!World || PerkData.SkillID == NAME_None) 
    {
        return false;
    }
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) 
    {
        return false;
    }
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) 
    {
        return false;
    }
    
    USkillComponent* SkillComponent = PlayerCharacter->FindComponentByClass<USkillComponent>();
    if (!SkillComponent) 
    {
        return false;
    }
    
    // 스킬 슬롯에서 해당 스킬 찾기
    USkill* TargetSkill = nullptr;
    for (int32 i = 0; i < SkillComponent->GetSkillNum(); i++)
    {
        USkill* Skill = SkillComponent->GetSkillSlot(i);
        if (Skill && Skill->GetSkillData().SkillID == PerkData.SkillID)
        {
            TargetSkill = Skill;
            break;
        }
    }
    
    if (!TargetSkill) 
    {
        return false;
    }
    
    // 원본 스킬 데이터 저장 (복원용)
    m_OriginalSkillData = TargetSkill->GetSkillData();
    
    
    // 라운드 기반 지속시간 설정
    m_RoundBasedEffect = FRoundBasedEffect(PerkData, m_CurrentRound);
    
    // 지연 활성화 설정
    m_bIsActivated = false;
    m_bWaitingForNextRound = true;
    
    
    if (PerkData.RoundDuration > 0)
    {
    }
    else
    {
    }
    return true;
}

bool USkillBoostEffect::RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World || PerkData.SkillID == NAME_None) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    USkillComponent* SkillComponent = PlayerCharacter->FindComponentByClass<USkillComponent>();
    if (!SkillComponent) return false;
    
    // 스킬 슬롯에서 해당 스킬 찾기
    USkill* TargetSkill = nullptr;
    for (int32 i = 0; i < SkillComponent->GetSkillNum(); i++)
    {
        USkill* Skill = SkillComponent->GetSkillSlot(i);
        if (Skill && Skill->GetSkillData().SkillID == PerkData.SkillID)
        {
            TargetSkill = Skill;
            break;
        }
    }
    
    if (!TargetSkill) return false;
    
    // 원본 스킬 데이터로 복원 (활성화된 상태에서만)
    if (m_bIsActivated && m_OriginalSkillData.SkillID != NAME_None)
    {
        // SkillComponent를 통해 스킬 데이터 복원 (UI 알림 포함)
        SkillComponent->UpdateSkillData(PerkData.SkillID, m_OriginalSkillData);
        
    }
    else if (m_bIsActivated)
    {
    }
    
    // 상태 초기화
    m_bIsActivated = false;
    m_bWaitingForNextRound = false;
    
    return true;
}

void USkillBoostEffect::OnRoundStart()
{
    
    // 다음 라운드를 기다리는 중이면 실제 효과 적용
    if (m_bWaitingForNextRound && !m_bIsActivated)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 플레이어 캐릭터 찾기
        APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
        if (!Pawn) return;
        
        APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
        if (!PlayerCharacter) return;
        
        USkillComponent* SkillComponent = PlayerCharacter->FindComponentByClass<USkillComponent>();
        if (!SkillComponent) return;
        
        // 스킬 슬롯에서 해당 스킬 찾기
        USkill* TargetSkill = nullptr;
        for (int32 i = 0; i < SkillComponent->GetSkillNum(); i++)
        {
            USkill* Skill = SkillComponent->GetSkillSlot(i);
            if (Skill && Skill->GetSkillData().SkillID == m_RoundBasedEffect.PerkData.SkillID)
            {
                TargetSkill = Skill;
                break;
            }
        }
        
        if (!TargetSkill) return;
        
        // 원본 스킬 데이터가 유효한지 확인
        if (m_OriginalSkillData.SkillID == NAME_None)
        {
            return;
        }
        
        // 새로운 스킬 데이터로 교체 (비어있는 항목은 원본 유지)
        FSkillData NewSkillData = m_OriginalSkillData; // 원본 데이터로 시작
        FSkillData OverrideData = m_RoundBasedEffect.PerkData.SkillDataOverride;
        
        // 비어있지 않은 항목만 덮어쓰기
        if (OverrideData.SkillName != TEXT(""))
            NewSkillData.SkillName = OverrideData.SkillName;
        if (OverrideData.WeaponType != TEXT(""))
            NewSkillData.WeaponType = OverrideData.WeaponType;
        if (OverrideData.Description != TEXT(""))
            NewSkillData.Description = OverrideData.Description;
        if (OverrideData.Icon != nullptr)
            NewSkillData.Icon = OverrideData.Icon;
        if (OverrideData.Animation != nullptr)
            NewSkillData.Animation = OverrideData.Animation;
        if (OverrideData.Sound != nullptr)
            NewSkillData.Sound = OverrideData.Sound;
        if (OverrideData.HalfHeight > 0)
            NewSkillData.HalfHeight = OverrideData.HalfHeight;
        if (OverrideData.Range > 0)
            NewSkillData.Range = OverrideData.Range;
        if (OverrideData.Radius > 0)
            NewSkillData.Radius = OverrideData.Radius;
        if (OverrideData.ConeHalfAngle > 0)
            NewSkillData.ConeHalfAngle = OverrideData.ConeHalfAngle;
        if (OverrideData.Cooldown > 0)
            NewSkillData.Cooldown = OverrideData.Cooldown;
        if (OverrideData.CastingTime > 0)
            NewSkillData.CastingTime = OverrideData.CastingTime;
        if (OverrideData.Duration > 0)
            NewSkillData.Duration = OverrideData.Duration;
        
        // 복잡한 멤버들은 처리하지 않음 (ProjectileInfo, SkillLevels, TargetAffiliation, TriggerClass, TargetingClass, Effects)
        
        // SkillID는 항상 원본 유지 (FPerkData의 SkillID 사용)
        NewSkillData.SkillID = m_RoundBasedEffect.PerkData.SkillID;
        
        // SkillComponent를 통해 스킬 데이터 업데이트 (UI 알림 포함)
        SkillComponent->UpdateSkillData(m_RoundBasedEffect.PerkData.SkillID, NewSkillData);
        
        m_bIsActivated = true;
        m_bWaitingForNextRound = false;
        
    }
    
    // 이미 활성화된 상태에서 라운드 지속시간 관리
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        
        // 남은 라운드가 0이 되면 효과 제거
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveEffect_Implementation(m_RoundBasedEffect.PerkData, World);
            }
        }
    }
}

void USkillBoostEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}

// UAllItemsQuantityEffect 구현
bool UAllItemsQuantityEffect::ApplyEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
    if (!ItemComponent) return false;
    
    // 적용할 수량 계산
    int32 QuantityIncrease = FMath::RoundToInt(PerkData.Value);
    
    // 모든 아이템 가져오기
    TArray<UItem*> AllItems = ItemComponent->GetAllItemData();
    
    // 적용된 아이템 ID 초기화
    m_AppliedItemIDs.Empty();
    
    int32 AppliedCount = 0;
    for (UItem* Item : AllItems)
    {
        if (!Item) continue;
        
        FName ItemID = Item->GetItemData().ItemID;
        if (ItemID == NAME_None) continue;
        
        // 아이템 수량 증가
        bool bSuccess = ItemComponent->AddItemQuantity(ItemID, QuantityIncrease);
        if (bSuccess)
        {
            m_AppliedItemIDs.Add(ItemID);
            AppliedCount++;
        }
    }
    
    
    // 일회성 효과이므로 즉시 비활성화
    if (AppliedCount > 0)
    {
        AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
        if (MyPS && MyPS->PerkComp)
        {
            MyPS->PerkComp->DeactivatePerkByID(PerkData.PerkID);
        }
    }
    
    return AppliedCount > 0;
}

bool UAllItemsQuantityEffect::RemoveEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    // 일회성 효과이므로 제거할 필요 없음 (아이템 수량은 그대로 유지)
    return true;
}

// UPlayerEffectStrategy 기본 구현
bool UPlayerEffectStrategy::ApplyPlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

bool UPlayerEffectStrategy::RemovePlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

// URoundEffectStrategy 기본 구현
bool URoundEffectStrategy::ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

bool URoundEffectStrategy::RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

// UGoldIncreaseEffect 구현
bool UGoldIncreaseEffect::ApplyPlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
    if (!MyPS) return false;
    
    int32 GoldIncrease = FMath::RoundToInt(PerkData.Value);
    MyPS->Gold += GoldIncrease;
    
    
    // 일회성 효과이므로 즉시 비활성화
    if (UPerkComponent* PerkComp = MyPS->PerkComp)
    {
        PerkComp->DeactivatePerkByID(PerkData.PerkID);
    }
    
    return true;
}

bool UGoldIncreaseEffect::RemovePlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    // 일회성 효과이므로 제거할 필요 없음 (골드는 그대로 유지)
    return true;
}

// UExpIncreaseEffect 구현
bool UExpIncreaseEffect::ApplyPlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    // 플레이어 캐릭터의 StatusComponent 찾기
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UStatusComponent* StatusComponent = PlayerCharacter->FindComponentByClass<UStatusComponent>();
    if (!StatusComponent) return false;
    
    int32 ExpIncrease = FMath::RoundToInt(PerkData.Value);
    
    // StatusComponent의 AddExp() 함수 사용
    StatusComponent->AddExp(ExpIncrease);
    
    
    // 일회성 효과이므로 즉시 비활성화
    AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
    if (MyPS)
    {
        UPerkComponent* PerkComp = MyPS->PerkComp;
        if (PerkComp)
        {
            PerkComp->DeactivatePerkByID(PerkData.PerkID);
        }
    }
    
    return true;
}

bool UExpIncreaseEffect::RemovePlayerEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    // 일회성 효과이므로 제거할 필요 없음 (경험치는 그대로 유지)
    return true;
}

// URoundGoldIncreaseEffect 구현
bool URoundGoldIncreaseEffect::ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
    if (!MyPS) return false;
    
    int32 GoldIncrease = FMath::RoundToInt(PerkData.Value);
    MyPS->Gold += GoldIncrease;
    
    return true;
}

bool URoundGoldIncreaseEffect::RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
    if (!MyPS) return false;
    
    // ERoundBonusEffectType은 일회성 보상이므로 되돌리지 않음
    // 골드 증가는 영구적으로 유지됨
    return true;
}

// URoundExpIncreaseEffect 구현
bool URoundExpIncreaseEffect::ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    // 플레이어 캐릭터의 StatusComponent 찾기
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UStatusComponent* StatusComponent = PlayerCharacter->FindComponentByClass<UStatusComponent>();
    if (!StatusComponent) return false;
    
    int32 ExpIncrease = FMath::RoundToInt(PerkData.Value);
    
    // StatusComponent의 AddExp() 함수 사용
    StatusComponent->AddExp(ExpIncrease);
    
    // 디버깅 로그 추가
    
    return true;
}

bool URoundExpIncreaseEffect::RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
    if (!MyPS) return false;
    
    // ERoundBonusEffectType은 일회성 보상이므로 되돌리지 않음
    // 경험치 증가는 영구적으로 유지됨
    return true;
}

// URoundItemQuantityEffect 구현
bool URoundItemQuantityEffect::ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World || PerkData.ItemID == NAME_None) return false;
    
    // 플레이어 캐릭터 찾기 (ItemComponent는 PlayerCharacter에 있음)
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
    if (!ItemComponent) return false;
    
    int32 QuantityIncrease = FMath::RoundToInt(PerkData.Value);
    bool bSuccess = ItemComponent->AddItemQuantity(PerkData.ItemID, QuantityIncrease);
    
    if (bSuccess)
    {
    }
    else
    {
    }
    
    return bSuccess;
}

bool URoundItemQuantityEffect::RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World || PerkData.ItemID == NAME_None) return false;
    
    // RoundDuration이 끝나면 더 이상 아이템을 제공하지 않음
    // 이미 제공된 아이템 수량은 되돌리지 않고 그대로 유지
    return true;
}

// URoundAllItemsQuantityEffect 구현
bool URoundAllItemsQuantityEffect::ApplyRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
    if (!PlayerCharacter) return false;
    
    UItemComponent* ItemComponent = PlayerCharacter->FindComponentByClass<UItemComponent>();
    if (!ItemComponent) return false;
    
    // 적용할 수량 계산
    int32 QuantityIncrease = FMath::RoundToInt(PerkData.Value);
    
    // 모든 아이템 가져오기
    TArray<UItem*> AllItems = ItemComponent->GetAllItemData();
    
    // 적용된 아이템 ID 초기화
    m_AppliedItemIDs.Empty();
    
    int32 AppliedCount = 0;
    for (UItem* Item : AllItems)
    {
        if (!Item) continue;
        
        FName ItemID = Item->GetItemData().ItemID;
        if (ItemID == NAME_None) continue;
        
        // 아이템 수량 증가
        bool bSuccess = ItemComponent->AddItemQuantity(ItemID, QuantityIncrease);
        if (bSuccess)
        {
            m_AppliedItemIDs.Add(ItemID);
            AppliedCount++;
        }
    }
    
    return AppliedCount > 0;
}

bool URoundAllItemsQuantityEffect::RemoveRoundEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // RoundDuration이 끝나면 더 이상 아이템을 제공하지 않음
    // 이미 제공된 아이템 수량은 되돌리지 않고 그대로 유지
    return true;
}

// UStatEffectStrategy 기본 구현
bool UStatEffectStrategy::ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

bool UStatEffectStrategy::RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    return false;
}

// UPlayerStatEffect 구현
bool UPlayerStatEffect::ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 현재 라운드 수를 PerkComponent에서 가져오기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                m_CurrentRound = PerkComp->GetCurrentRound();
            }
        }
    }
    
    // 라운드 기반 지속시간 설정 (올바른 방식)
    m_RoundBasedEffect = FRoundBasedEffect(PerkData, m_CurrentRound);
    
    // 라운드 기반 지속시간이 있는 경우 지연 활성화
    if (PerkData.RoundDuration > 0)
    {
        m_bWaitingForNextRound = true;
        m_bIsActivated = false;
        return true;
    }
    
    // 즉시 적용 (영구 지속)
    m_bIsActivated = true;
    return ApplyStatModifier(PerkData, World);
}

bool UPlayerStatEffect::ApplyStatModifier(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    // 플레이어 캐릭터의 StatusComponent 찾기 (Pawn에서 직접 찾기)
    UStatusComponent* StatusComponent = Pawn->FindComponentByClass<UStatusComponent>();
    if (!StatusComponent) return false;
    
    // 스탯 모디파이어 적용
    StatusComponent->AddModifier(PerkData.StatModifier);
    
    return true;
}

bool UPlayerStatEffect::RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 실제로 적용된 효과가 있는 경우에만 제거
    if (m_bIsActivated)
    {
        return RemoveStatModifier(PerkData, World);
    }
    
    // 지연 활성화 상태인 경우 플래그만 리셋
    m_bWaitingForNextRound = false;
    m_bIsActivated = false;
    
    return true;
}

bool UPlayerStatEffect::RemoveStatModifier(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 플레이어 캐릭터 찾기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (!Pawn) return false;
    
    // 플레이어 캐릭터의 StatusComponent 찾기 (Pawn에서 직접 찾기)
    UStatusComponent* StatusComponent = Pawn->FindComponentByClass<UStatusComponent>();
    if (!StatusComponent) return false;
    
    // 스탯 모디파이어 생성 (제거용 - 음수로 변환)
    FStatModifier RemoveModifier;
    RemoveModifier.AddHP = -PerkData.StatModifier.AddHP;
    RemoveModifier.AddAtk = -PerkData.StatModifier.AddAtk;
    RemoveModifier.AddDef = -PerkData.StatModifier.AddDef;
    RemoveModifier.AddAtkSpd = -PerkData.StatModifier.AddAtkSpd;
    RemoveModifier.AddCritR = -PerkData.StatModifier.AddCritR;
    RemoveModifier.AddCritD = -PerkData.StatModifier.AddCritD;
    RemoveModifier.AddAtkRadius = -PerkData.StatModifier.AddAtkRadius;
    RemoveModifier.AddCoolRec = -PerkData.StatModifier.AddCoolRec;
    RemoveModifier.AddSpd = -PerkData.StatModifier.AddSpd;
    
    // AddModifier에 음수값을 전달하여 효과적으로 제거
    StatusComponent->AddModifier(RemoveModifier);
    
    return true;
}

void UPlayerStatEffect::OnRoundStart()
{
    // 이미 만료된 퍼크는 처리하지 않음
    if (!m_bWaitingForNextRound && !m_bIsActivated && m_RoundBasedEffect.RemainingRounds <= 0)
    {
        return;
    }
    
    // 지연 활성화 대기 중인 경우
    if (m_bWaitingForNextRound)
    {
        m_bWaitingForNextRound = false;
        m_bIsActivated = true;
        
        // 실제 효과 적용
        UWorld* World = GetWorld();
        if (World)
        {
            ApplyStatModifier(m_RoundBasedEffect.PerkData, World);
        }
    }
    
    // 이미 활성화된 경우 라운드 수 감소
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        // 지속시간 종료
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveStatModifier(m_RoundBasedEffect.PerkData, World);
                m_bIsActivated = false;
                
                // PerkComponent에서 이 퍼크를 비활성화하여 m_ActivePerks에서 제거
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (UPerkComponent* PerkComp = MyPS->PerkComp)
                        {
                            PerkComp->DeactivatePerkByID(m_RoundBasedEffect.PerkData.PerkID);
                        }
                    }
                }
            }
        }
    }
}

void UPlayerStatEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}

// UAllyStatEffect 구현
bool UAllyStatEffect::ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 현재 라운드 수를 PerkComponent에서 가져오기
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                m_CurrentRound = PerkComp->GetCurrentRound();
            }
        }
    }
    
    // 라운드 기반 지속시간 설정 (올바른 방식)
    m_RoundBasedEffect = FRoundBasedEffect(PerkData, m_CurrentRound);
    
    // 적용할 스탯 모디파이어 저장 (항상 저장)
    m_AppliedModifier = PerkData.StatModifier;
    
    // 라운드 기반 지속시간이 있는 경우 지연 활성화
    if (PerkData.RoundDuration > 0)
    {
        m_bWaitingForNextRound = true;
        m_bIsActivated = false;
        return true;
    }
    
    // 즉시 적용 (영구 지속)
    m_bIsActivated = true;
    return ApplyStatModifierToAllAllies(PerkData, World);
}

bool UAllyStatEffect::ApplyStatModifierToAllAllies(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 기존 적용된 아군들 초기화
    m_AppliedAllies.Empty();
    
    // 첫 번째 적용 시에만 전체 검색 (성능 최적화)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGameCharacterBase::StaticClass(), FoundActors);
    
    int32 AppliedCount = 0;
    for (AActor* Actor : FoundActors)
    {
        AGameCharacterBase* Character = Cast<AGameCharacterBase>(Actor);
        if (!Character) continue;
        
        UStatusComponent* StatusComponent = Character->FindComponentByClass<UStatusComponent>();
        if (!StatusComponent) continue;
        
        // 아군인지 확인 (EClassType::Ally)
        if (StatusComponent->GetClassType() == EClassType::Ally)
            {
                // 스탯 모디파이어 적용
                StatusComponent->AddModifier(m_AppliedModifier);
                
                // 적용된 아군을 캐시에 추가
                m_AppliedAllies.Add(StatusComponent);
                AppliedCount++;
        }
    }
    
    // PerkComponent에 모디파이어 저장 (새로 스폰되는 아군에게 적용용)
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->AddAppliedAllyStatModifier(m_AppliedModifier);
            }
        }
    }
    
    return AppliedCount > 0;
}

bool UAllyStatEffect::RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 실제로 적용된 효과가 있는 경우에만 제거
    if (m_bIsActivated)
    {
        return RemoveStatModifierFromAllAllies(PerkData, World);
    }
    
    // 지연 활성화 상태인 경우 플래그만 리셋
    m_bWaitingForNextRound = false;
    m_bIsActivated = false;
    
    return true;
}

bool UAllyStatEffect::RemoveStatModifierFromAllAllies(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 캐시된 아군들에게만 효과 제거 (성능 최적화)
    int32 RemovedCount = 0;
    for (UStatusComponent* StatusComponent : m_AppliedAllies)
    {
        if (!StatusComponent) continue;
        
        // 스탯 모디파이어 제거 (음수로 제거)
        FStatModifier RemoveModifier;
        RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
        RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
        RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
        RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
        
        StatusComponent->RemoveModifier(RemoveModifier);
        RemovedCount++;
    }
    
    // PerkComponent에서 모디파이어 제거
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->RemoveAppliedAllyStatModifier(m_AppliedModifier);
            }
        }
    }
    
    // 캐시 초기화
    m_AppliedAllies.Empty();
    m_AppliedModifier = FStatModifier();
    
    return RemovedCount > 0;
}

void UAllyStatEffect::ApplyToNewAlly(UStatusComponent* NewAllyStatusComp)
{
    if (!NewAllyStatusComp) return;
    
    // 아군인지 확인
    if (NewAllyStatusComp->GetClassType() != EClassType::Ally) return;
    
    // m_AppliedAllyStatModifiers 방식 사용 시에는 중복 체크 불필요
    // PlayerState PerkComponent에서 이미 중앙 집중식으로 관리하므로
    
    // 새 아군에게 스탯 모디파이어 적용
    NewAllyStatusComp->AddModifier(m_AppliedModifier);
    
    // 캐시에 추가 (디버깅용)
    m_AppliedAllies.Add(NewAllyStatusComp);
}

void UAllyStatEffect::RemoveFromAlly(UStatusComponent* AllyStatusComp)
{
    if (!AllyStatusComp) return;
    
    // 캐시에서 제거
    m_AppliedAllies.Remove(AllyStatusComp);
    
    // 스탯 모디파이어 제거 (모든 변수 음수 변환)
    FStatModifier RemoveModifier;
    RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
    RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
    RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
    RemoveModifier.AddAtkSpd = -m_AppliedModifier.AddAtkSpd;
    RemoveModifier.AddCritR = -m_AppliedModifier.AddCritR;
    RemoveModifier.AddCritD = -m_AppliedModifier.AddCritD;
    RemoveModifier.AddAtkRadius = -m_AppliedModifier.AddAtkRadius;
    RemoveModifier.AddCoolRec = -m_AppliedModifier.AddCoolRec;
    RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
    
    // AddModifier에 음수값을 전달하여 효과적으로 제거
    AllyStatusComp->AddModifier(RemoveModifier);
}

void UAllyStatEffect::OnRoundStart()
{
    // 이미 만료된 퍼크는 처리하지 않음
    if (!m_bWaitingForNextRound && !m_bIsActivated && m_RoundBasedEffect.RemainingRounds <= 0)
    {
        return;
    }
    
    // 지연 활성화 대기 중인 경우
    if (m_bWaitingForNextRound)
    {
        m_bWaitingForNextRound = false;
        m_bIsActivated = true;
        
        // 실제 효과 적용
        UWorld* World = GetWorld();
        if (World)
        {
            ApplyStatModifierToAllAllies(m_RoundBasedEffect.PerkData, World);
        }
    }
    
    // 이미 활성화된 경우 라운드 수 감소
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        // 지속시간 종료
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveStatModifierFromAllAllies(m_RoundBasedEffect.PerkData, World);
                m_bIsActivated = false;
                
                // PerkComponent에서 이 퍼크를 비활성화하여 m_ActivePerks에서 제거
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (UPerkComponent* PerkComp = MyPS->PerkComp)
                        {
                            PerkComp->DeactivatePerkByID(m_RoundBasedEffect.PerkData.PerkID);
                        }
                    }
                }
            }
        }
    }
}

void UAllyStatEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}

// UEnemyStatEffect 구현
bool UEnemyStatEffect::ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 라운드 기반 지속시간 설정
    m_RoundBasedEffect.PerkData = PerkData;
    m_RoundBasedEffect.RemainingRounds = PerkData.RoundDuration;
    
    // 적용할 스탯 모디파이어 저장
    m_AppliedModifier = PerkData.StatModifier;
    
    // 라운드 기반 지속시간이 있는 경우 지연 활성화
    if (PerkData.RoundDuration > 0)
    {
        m_bWaitingForNextRound = true;
        m_bIsActivated = false;
        return true;
    }
    
    // 즉시 적용 (영구 지속)
    return ApplyStatModifierToAllEnemies(PerkData, World);
}

bool UEnemyStatEffect::ApplyStatModifierToAllEnemies(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 기존 적용된 적군들 초기화
    m_AppliedEnemies.Empty();
    
    // 첫 번째 적용 시에만 전체 검색 (성능 최적화)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGameCharacterBase::StaticClass(), FoundActors);
    
    int32 AppliedCount = 0;
    for (AActor* Actor : FoundActors)
    {
        AGameCharacterBase* Character = Cast<AGameCharacterBase>(Actor);
        if (!Character) continue;
        
        UStatusComponent* StatusComponent = Character->FindComponentByClass<UStatusComponent>();
        if (!StatusComponent) continue;
        
        // 적군인지 확인 (EClassType::Enemy)
        if (StatusComponent->GetClassType() == EClassType::Enemy)
        {
            // 스탯 모디파이어 적용
            StatusComponent->AddModifier(m_AppliedModifier);
            
            // 적용된 적군을 캐시에 추가
            m_AppliedEnemies.Add(StatusComponent);
            AppliedCount++;
        }
    }
    
    // PerkComponent에 모디파이어 저장 (새로 스폰되는 적군에게 적용용)
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->AddAppliedEnemyStatModifier(m_AppliedModifier);
            }
        }
    }
    
    return AppliedCount > 0;
}

bool UEnemyStatEffect::RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 실제로 적용된 효과가 있는 경우에만 제거
    if (m_bIsActivated)
    {
        return RemoveStatModifierFromAllEnemies(PerkData, World);
    }
    
    // 지연 활성화 상태인 경우 플래그만 리셋
    m_bWaitingForNextRound = false;
    m_bIsActivated = false;
    
    return true;
}

bool UEnemyStatEffect::RemoveStatModifierFromAllEnemies(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 캐시된 적군들에게만 효과 제거 (성능 최적화)
    int32 RemovedCount = 0;
    for (UStatusComponent* StatusComponent : m_AppliedEnemies)
    {
        if (!StatusComponent) continue;
        
        // 스탯 모디파이어 제거 (모든 변수 음수 변환)
        FStatModifier RemoveModifier;
        RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
        RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
        RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
        RemoveModifier.AddAtkSpd = -m_AppliedModifier.AddAtkSpd;
        RemoveModifier.AddCritR = -m_AppliedModifier.AddCritR;
        RemoveModifier.AddCritD = -m_AppliedModifier.AddCritD;
        RemoveModifier.AddAtkRadius = -m_AppliedModifier.AddAtkRadius;
        RemoveModifier.AddCoolRec = -m_AppliedModifier.AddCoolRec;
        RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
        
        StatusComponent->AddModifier(RemoveModifier);
        RemovedCount++;
    }
    
    // PerkComponent에서 모디파이어 제거
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->RemoveAppliedEnemyStatModifier(m_AppliedModifier);
            }
        }
    }
    
    // 캐시 초기화
    m_AppliedEnemies.Empty();
    m_AppliedModifier = FStatModifier();
    
    return RemovedCount > 0;
}

void UEnemyStatEffect::ApplyToNewEnemy(UStatusComponent* NewEnemyStatusComp)
{
    if (!NewEnemyStatusComp) return;
    
    // 적군인지 확인
    if (NewEnemyStatusComp->GetClassType() != EClassType::Enemy) return;
    
    // m_AppliedEnemyStatModifiers 방식 사용 시에는 중복 체크 불필요
    // PlayerState PerkComponent에서 이미 중앙 집중식으로 관리하므로
    
    // 새 적군에게 스탯 모디파이어 적용
    NewEnemyStatusComp->AddModifier(m_AppliedModifier);
    
    // 캐시에 추가 (디버깅용)
    m_AppliedEnemies.Add(NewEnemyStatusComp);
}

void UEnemyStatEffect::RemoveFromEnemy(UStatusComponent* EnemyStatusComp)
{
    if (!EnemyStatusComp) return;
    
    // 캐시에서 제거
    m_AppliedEnemies.Remove(EnemyStatusComp);
    
    // 스탯 모디파이어 제거
    FStatModifier RemoveModifier;
    RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
    RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
    RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
    RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
    
    EnemyStatusComp->RemoveModifier(RemoveModifier);
}

void UEnemyStatEffect::OnRoundStart()
{
    // 지연 활성화 대기 중인 경우
    if (m_bWaitingForNextRound)
    {
        m_bWaitingForNextRound = false;
        m_bIsActivated = true;
        
        // 실제 효과 적용
        UWorld* World = GetWorld();
        if (World)
        {
            ApplyStatModifierToAllEnemies(m_RoundBasedEffect.PerkData, World);
        }
    }
    
    // 이미 활성화된 경우 라운드 수 감소
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        // 지속시간 종료
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveStatModifierFromAllEnemies(m_RoundBasedEffect.PerkData, World);
                m_bIsActivated = false;
            }
        }
    }
}

void UEnemyStatEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}

// UAllyUnitStatEffect 구현
bool UAllyUnitStatEffect::ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 라운드 기반 지속시간 설정
    m_RoundBasedEffect.PerkData = PerkData;
    m_RoundBasedEffect.RemainingRounds = PerkData.RoundDuration;
    
    // 적용할 스탯 모디파이어와 타겟 무기 타입 저장
    m_AppliedModifier = PerkData.StatModifier;
    m_TargetWeaponType = PerkData.TargetWeaponType;
    
    // 라운드 기반 지속시간이 있는 경우 지연 활성화
    if (PerkData.RoundDuration > 0)
    {
        m_bWaitingForNextRound = true;
        m_bIsActivated = false;
        return true;
    }
    
    // 즉시 적용 (영구 지속)
    return ApplyStatModifierToTargetUnits(PerkData, World);
}

bool UAllyUnitStatEffect::ApplyStatModifierToTargetUnits(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 기존 적용된 유닛들 초기화
    m_AppliedUnits.Empty();
    
    // 첫 번째 적용 시에만 전체 검색 (성능 최적화)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGameCharacterBase::StaticClass(), FoundActors);
    
    int32 AppliedCount = 0;
    for (AActor* Actor : FoundActors)
    {
        AGameCharacterBase* Character = Cast<AGameCharacterBase>(Actor);
        if (!Character) continue;
        
        UStatusComponent* StatusComponent = Character->FindComponentByClass<UStatusComponent>();
        if (!StatusComponent) continue;
        
        // 아군이면서 플레이어가 아닌지 확인
        if (StatusComponent->GetClassType() == EClassType::Ally)
        {
            // 무기 타입 확인
            FName WeaponID = StatusComponent->GetWeaponID();
            EWeaponType CurrentWeaponType = EWeaponType::None;
            
            // WeaponID를 EWeaponType으로 변환 (아군용: Common Weapon + Only Team Weapon)
            if (WeaponID == FName("Bow")) CurrentWeaponType = EWeaponType::Bow;
            else if (WeaponID == FName("GreatSword")) CurrentWeaponType = EWeaponType::GreatSword;
            else if (WeaponID == FName("Staff")) CurrentWeaponType = EWeaponType::Staff;
            else if (WeaponID == FName("Throwing")) CurrentWeaponType = EWeaponType::Throwing;
            else if (WeaponID == FName("Sword")) CurrentWeaponType = EWeaponType::Sword;
            else if (WeaponID == FName("Shield")) CurrentWeaponType = EWeaponType::Shield;
            else if (WeaponID == FName("catapult")) CurrentWeaponType = EWeaponType::catapult;
            
            // 타겟 무기 타입과 일치하는지 확인
            if (CurrentWeaponType == m_TargetWeaponType)
            {
                // 스탯 모디파이어 적용
                StatusComponent->AddModifier(m_AppliedModifier);
                
                // 적용된 유닛을 캐시에 추가
                m_AppliedUnits.Add(StatusComponent);
                AppliedCount++;
            }
        }
    }
    
    // PerkComponent에 모디파이어 저장 (새로 스폰되는 특정 무기 타입 아군에게 적용용)
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->AddAppliedWeaponStatModifier(m_TargetWeaponType, m_AppliedModifier);
            }
        }
    }
    
    return AppliedCount > 0;
}

bool UAllyUnitStatEffect::RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 실제로 적용된 효과가 있는 경우에만 제거
    if (m_bIsActivated)
    {
        return RemoveStatModifierFromTargetUnits(PerkData, World);
    }
    
    // 지연 활성화 상태인 경우 플래그만 리셋
    m_bWaitingForNextRound = false;
    m_bIsActivated = false;
    
    return true;
}

bool UAllyUnitStatEffect::RemoveStatModifierFromTargetUnits(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 캐시된 유닛들에게만 효과 제거 (성능 최적화)
    int32 RemovedCount = 0;
    for (UStatusComponent* StatusComponent : m_AppliedUnits)
    {
        if (!StatusComponent) continue;
        
        // 스탯 모디파이어 제거 (모든 변수 음수 변환)
        FStatModifier RemoveModifier;
        RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
        RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
        RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
        RemoveModifier.AddAtkSpd = -m_AppliedModifier.AddAtkSpd;
        RemoveModifier.AddCritR = -m_AppliedModifier.AddCritR;
        RemoveModifier.AddCritD = -m_AppliedModifier.AddCritD;
        RemoveModifier.AddAtkRadius = -m_AppliedModifier.AddAtkRadius;
        RemoveModifier.AddCoolRec = -m_AppliedModifier.AddCoolRec;
        RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
        
        StatusComponent->AddModifier(RemoveModifier);
        RemovedCount++;
    }
    
    // PerkComponent에서 모디파이어 제거
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->RemoveAppliedWeaponStatModifier(m_TargetWeaponType, m_AppliedModifier);
            }
        }
    }
    
    // 캐시 초기화
    m_AppliedUnits.Empty();
    m_AppliedModifier = FStatModifier();
    m_TargetWeaponType = EWeaponType::None;
    
    return RemovedCount > 0;
}


void UAllyUnitStatEffect::RemoveFromUnit(UStatusComponent* UnitStatusComp)
{
    if (!UnitStatusComp) return;
    
    // 캐시에서 제거
    m_AppliedUnits.Remove(UnitStatusComp);
    
    // 스탯 모디파이어 제거
    FStatModifier RemoveModifier;
    RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
    RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
    RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
    RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
    
    UnitStatusComp->RemoveModifier(RemoveModifier);
}

void UAllyUnitStatEffect::OnRoundStart()
{
    // 지연 활성화 대기 중인 경우
    if (m_bWaitingForNextRound)
    {
        m_bWaitingForNextRound = false;
        m_bIsActivated = true;
        
        // 실제 효과 적용
        UWorld* World = GetWorld();
        if (World)
        {
            ApplyStatModifierToTargetUnits(m_RoundBasedEffect.PerkData, World);
        }
    }
    
    // 이미 활성화된 경우 라운드 수 감소
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        // 지속시간 종료
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveStatModifierFromTargetUnits(m_RoundBasedEffect.PerkData, World);
                m_bIsActivated = false;
            }
        }
    }
}

void UAllyUnitStatEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}

// UEnemyUnitStatEffect 구현
bool UEnemyUnitStatEffect::ApplyStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 라운드 기반 지속시간 설정
    m_RoundBasedEffect.PerkData = PerkData;
    m_RoundBasedEffect.RemainingRounds = PerkData.RoundDuration;
    
    // 적용할 스탯 모디파이어와 타겟 무기 타입 저장
    m_AppliedModifier = PerkData.StatModifier;
    m_TargetWeaponType = PerkData.TargetWeaponType;
    
    // 라운드 기반 지속시간이 있는 경우 지연 활성화
    if (PerkData.RoundDuration > 0)
    {
        m_bWaitingForNextRound = true;
        m_bIsActivated = false;
        return true;
    }
    
    // 즉시 적용 (영구 지속)
    return ApplyStatModifierToTargetUnits(PerkData, World);
}

bool UEnemyUnitStatEffect::ApplyStatModifierToTargetUnits(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 기존 적용된 유닛들 초기화
    m_AppliedUnits.Empty();
    
    // 첫 번째 적용 시에만 전체 검색 (성능 최적화)
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(World, AGameCharacterBase::StaticClass(), FoundActors);
    
    int32 AppliedCount = 0;
    for (AActor* Actor : FoundActors)
    {
        AGameCharacterBase* Character = Cast<AGameCharacterBase>(Actor);
        if (!Character) continue;
        
        UStatusComponent* StatusComponent = Character->FindComponentByClass<UStatusComponent>();
        if (!StatusComponent) continue;
        
        // 적군이면서 플레이어가 아닌지 확인
        if (StatusComponent->GetClassType() == EClassType::Enemy)
        {
            // 무기 타입 확인
            FName WeaponID = StatusComponent->GetWeaponID();
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
            
            // 타겟 무기 타입과 일치하는지 확인
            if (CurrentWeaponType == m_TargetWeaponType)
            {
                // 스탯 모디파이어 적용
                StatusComponent->AddModifier(m_AppliedModifier);
                
                // 적용된 유닛을 캐시에 추가
                m_AppliedUnits.Add(StatusComponent);
                AppliedCount++;
            }
        }
    }
    
    // PerkComponent에 모디파이어 저장 (새로 스폰되는 특정 무기 타입 적군에게 적용용)
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->AddAppliedEnemyWeaponStatModifier(m_TargetWeaponType, m_AppliedModifier);
            }
        }
    }
    
    return AppliedCount > 0;
}

bool UEnemyUnitStatEffect::RemoveStatEffect_Implementation(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 실제로 적용된 효과가 있는 경우에만 제거
    if (m_bIsActivated)
    {
        return RemoveStatModifierFromTargetUnits(PerkData, World);
    }
    
    // 지연 활성화 상태인 경우 플래그만 리셋
    m_bWaitingForNextRound = false;
    m_bIsActivated = false;
    
    return true;
}

bool UEnemyUnitStatEffect::RemoveStatModifierFromTargetUnits(const FPerkData& PerkData, UWorld* World)
{
    if (!World) return false;
    
    // 캐시된 유닛들에게만 효과 제거 (성능 최적화)
    int32 RemovedCount = 0;
    for (UStatusComponent* StatusComponent : m_AppliedUnits)
    {
        if (!StatusComponent) continue;
        
        // 스탯 모디파이어 제거 (모든 변수 음수 변환)
        FStatModifier RemoveModifier;
        RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
        RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
        RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
        RemoveModifier.AddAtkSpd = -m_AppliedModifier.AddAtkSpd;
        RemoveModifier.AddCritR = -m_AppliedModifier.AddCritR;
        RemoveModifier.AddCritD = -m_AppliedModifier.AddCritD;
        RemoveModifier.AddAtkRadius = -m_AppliedModifier.AddAtkRadius;
        RemoveModifier.AddCoolRec = -m_AppliedModifier.AddCoolRec;
        RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
        
        StatusComponent->AddModifier(RemoveModifier);
        RemovedCount++;
    }
    
    // PerkComponent에서 모디파이어 제거
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
    if (Pawn)
    {
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
        {
            if (UPerkComponent* PerkComp = MyPS->PerkComp)
            {
                PerkComp->RemoveAppliedEnemyWeaponStatModifier(m_TargetWeaponType, m_AppliedModifier);
            }
        }
    }
    
    // 캐시 초기화
    m_AppliedUnits.Empty();
    m_AppliedModifier = FStatModifier();
    m_TargetWeaponType = EWeaponType::None;
    
    return RemovedCount > 0;
}

void UEnemyUnitStatEffect::RemoveFromUnit(UStatusComponent* UnitStatusComp)
{
    if (!UnitStatusComp) return;
    
    // 캐시에서 제거
    m_AppliedUnits.Remove(UnitStatusComp);
    
    // 스탯 모디파이어 제거
    FStatModifier RemoveModifier;
    RemoveModifier.AddHP = -m_AppliedModifier.AddHP;
    RemoveModifier.AddAtk = -m_AppliedModifier.AddAtk;
    RemoveModifier.AddDef = -m_AppliedModifier.AddDef;
    RemoveModifier.AddSpd = -m_AppliedModifier.AddSpd;
    
    UnitStatusComp->RemoveModifier(RemoveModifier);
}

void UEnemyUnitStatEffect::OnRoundStart()
{
    // 지연 활성화 대기 중인 경우
    if (m_bWaitingForNextRound)
    {
        m_bWaitingForNextRound = false;
        m_bIsActivated = true;
        
        // 실제 효과 적용
        UWorld* World = GetWorld();
        if (World)
        {
            ApplyStatModifierToTargetUnits(m_RoundBasedEffect.PerkData, World);
        }
    }
    
    // 이미 활성화된 경우 라운드 수 감소
    if (m_bIsActivated && m_RoundBasedEffect.RemainingRounds > 0)
    {
        m_RoundBasedEffect.RemainingRounds--;
        
        // 지속시간 종료
        if (m_RoundBasedEffect.RemainingRounds <= 0)
        {
            UWorld* World = GetWorld();
            if (World)
            {
                RemoveStatModifierFromTargetUnits(m_RoundBasedEffect.PerkData, World);
                m_bIsActivated = false;
            }
        }
    }
}

void UEnemyUnitStatEffect::UpdateCurrentRound(int32 NewRound)
{
    m_CurrentRound = NewRound;
}


// UStatPerk 구현
UStatPerk::UStatPerk()
{
}

void UStatPerk::OnPerkActivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Stat)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 스탯 효과 적용
        UStatEffectStrategy* StatStrategy = CreateStatEffectStrategy(m_PerkData.StatEffectType);
        if (StatStrategy)
        {
            StatStrategy->ApplyStatEffect(m_PerkData, World);
            
            // 라운드 기반 지속시간이 설정된 경우 PerkComponent에 등록
            if (m_PerkData.RoundDuration > 0)
            {
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (MyPS->PerkComp)
                        {
                            FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                            MyPS->PerkComp->AddRoundBasedEffect(RoundEffect);
                        }
                    }
                }
            }
        }
    }
}

void UStatPerk::OnPerkDeactivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Stat)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 스탯 효과 제거
        UStatEffectStrategy* StatStrategy = CreateStatEffectStrategy(m_PerkData.StatEffectType);
        if (StatStrategy)
        {
            StatStrategy->RemoveStatEffect(m_PerkData, World);
            
            // 라운드 기반 지속시간이 설정된 경우 PerkComponent에서 제거
            if (m_PerkData.RoundDuration > 0)
            {
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (MyPS->PerkComp)
                        {
                            FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                            MyPS->PerkComp->RemoveRoundBasedEffect(RoundEffect);
                        }
                    }
                }
            }
        }
    }
}

void UStatPerk::OnRoundStart()
{
    if (m_PerkData.PerkType != EPerkType::Stat) return;
    
    // 캐시된 인스턴스를 사용하여 OnRoundStart 호출 (상태 정보 유지)
    switch (m_PerkData.StatEffectType)
    {
        case EStatEffectType::PlayerStat:
            if (m_PlayerStatEffect)
            {
                m_PlayerStatEffect->OnRoundStart();
            }
            break;
            
        case EStatEffectType::AllyStat:
            if (m_AllyStatEffect)
            {
                m_AllyStatEffect->OnRoundStart();
            }
            break;
            
        case EStatEffectType::EnemyStat:
            if (m_EnemyStatEffect)
            {
                m_EnemyStatEffect->OnRoundStart();
            }
            break;
            
        case EStatEffectType::AllyUnitStat:
            if (m_AllyUnitStatEffect)
            {
                m_AllyUnitStatEffect->OnRoundStart();
            }
            break;
            
        case EStatEffectType::EnemyUnitStat:
            if (m_EnemyUnitStatEffect)
            {
                m_EnemyUnitStatEffect->OnRoundStart();
            }
            break;
            
        default:
            break;
    }
}


// UPlayerPerk 구현
UPlayerPerk::UPlayerPerk()
{
}

void UPlayerPerk::OnPerkActivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Player)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 플레이어 효과 적용
        UPlayerEffectStrategy* PlayerStrategy = CreatePlayerEffectStrategy(m_PerkData.PlayerEffectType);
        if (PlayerStrategy)
        {
            PlayerStrategy->ApplyPlayerEffect(m_PerkData, World);
        }
    }
}

void UPlayerPerk::OnPerkDeactivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Player)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 플레이어 효과 제거
        UPlayerEffectStrategy* PlayerStrategy = CreatePlayerEffectStrategy(m_PerkData.PlayerEffectType);
        if (PlayerStrategy)
        {
            PlayerStrategy->RemovePlayerEffect(m_PerkData, World);
        }
    }
}

// UItemPerk 구현
UItemPerk::UItemPerk()
{
}

void UItemPerk::OnPerkActivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Item)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 효과 적용
        m_CurrentEffectStrategy = CreateEffectStrategy(m_PerkData.ItemEffectType);
        if (m_CurrentEffectStrategy)
        {
            m_CurrentEffectStrategy->ApplyEffect(m_PerkData, World);
            
            // 라운드 기반 지속시간이 설정된 경우 PerkComponent에 등록
            if (m_PerkData.RoundDuration > 0)
            {
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (MyPS->PerkComp)
                        {
                            FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                            MyPS->PerkComp->AddRoundBasedEffect(RoundEffect);
                        }
                    }
                }
            }
        }
    }
}

void UItemPerk::OnPerkDeactivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Item)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 효과 제거
        UItemEffectStrategy* EffectStrategy = CreateEffectStrategy(m_PerkData.ItemEffectType);
        if (EffectStrategy)
        {
            EffectStrategy->RemoveEffect(m_PerkData, World);
            
            // 라운드 기반 지속시간이 설정된 경우 PerkComponent에서 제거
            if (m_PerkData.RoundDuration > 0)
            {
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (MyPS->PerkComp)
                        {
                            FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                            MyPS->PerkComp->RemoveRoundBasedEffect(RoundEffect);
                        }
                    }
                }
            }
        }
    }
}

UStatEffectStrategy* UStatPerk::CreateStatEffectStrategy(EStatEffectType EffectType)
{
    switch (EffectType)
    {
        case EStatEffectType::PlayerStat:
            // 캐시된 인스턴스 재사용 (라운드 기반 지속시간용)
            if (!m_PlayerStatEffect)
            {
                m_PlayerStatEffect = NewObject<UPlayerStatEffect>(this);
            }
            return m_PlayerStatEffect;
            
        case EStatEffectType::AllyStat:
            // 캐시된 인스턴스 재사용 (새 아군 생성 시 캐시된 효과 적용 가능)
            if (!m_AllyStatEffect)
            {
                m_AllyStatEffect = NewObject<UAllyStatEffect>(this);
            }
            return m_AllyStatEffect;
            
        case EStatEffectType::EnemyStat:
            // 캐시된 인스턴스 재사용 (새 적군 생성 시 캐시된 효과 적용 가능)
            if (!m_EnemyStatEffect)
            {
                m_EnemyStatEffect = NewObject<UEnemyStatEffect>(this);
            }
            return m_EnemyStatEffect;
            
        case EStatEffectType::AllyUnitStat:
            // 캐시된 인스턴스 재사용 (새 아군 유닛 생성 시 캐시된 효과 적용 가능)
            if (!m_AllyUnitStatEffect)
            {
                m_AllyUnitStatEffect = NewObject<UAllyUnitStatEffect>(this);
            }
            return m_AllyUnitStatEffect;
            
        case EStatEffectType::EnemyUnitStat:
            // 캐시된 인스턴스 재사용 (새 적군 유닛 생성 시 캐시된 효과 적용 가능)
            if (!m_EnemyUnitStatEffect)
            {
                m_EnemyUnitStatEffect = NewObject<UEnemyUnitStatEffect>(this);
            }
            return m_EnemyUnitStatEffect;
            
        default:
            return nullptr;
    }
}

UPlayerEffectStrategy* UPlayerPerk::CreatePlayerEffectStrategy(EPlayerEffectType EffectType)
{
    switch (EffectType)
    {
        case EPlayerEffectType::GoldIncrease:
            return NewObject<UGoldIncreaseEffect>(this);
            
        case EPlayerEffectType::ExpIncrease:
            return NewObject<UExpIncreaseEffect>(this);
            
        default:
            return nullptr;
    }
}

void UItemPerk::OnRoundStart()
{
    // ItemBoost 효과인 경우 OnRoundStart 호출
    if (m_PerkData.ItemEffectType == EItemEffectType::ItemBoost)
    {
        // 현재 전략이 있으면 사용, 없으면 새로 생성
        UItemBoostEffect* ItemBoostEffect = nullptr;
        if (m_CurrentEffectStrategy)
        {
            ItemBoostEffect = Cast<UItemBoostEffect>(m_CurrentEffectStrategy);
        }
        else
        {
            // 전략이 없으면 새로 생성
            ItemBoostEffect = NewObject<UItemBoostEffect>(this);
            if (ItemBoostEffect)
            {
                m_CurrentEffectStrategy = ItemBoostEffect;
                // PerkData 설정
                ItemBoostEffect->SetPerkData(m_PerkData);
            }
        }
        
        if (ItemBoostEffect)
        {
            ItemBoostEffect->OnRoundStart();
        }
    }
}


UItemEffectStrategy* UItemPerk::CreateEffectStrategy(EItemEffectType EffectType)
{
    switch (EffectType)
    {
        case EItemEffectType::ItemQuantity:
            return NewObject<UItemQuantityEffect>(this);
            
        case EItemEffectType::ItemBoost:
            return NewObject<UItemBoostEffect>(this);
            
        case EItemEffectType::AllItemsQuantity:
            return NewObject<UAllItemsQuantityEffect>(this);
            
        default:
            return nullptr;
    }
}

// USkillPerk 구현
USkillPerk::USkillPerk()
{
}

void USkillPerk::OnPerkActivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Skill)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 효과 적용
        m_CurrentEffectStrategy = CreateEffectStrategy(m_PerkData.SkillEffectType);
        if (m_CurrentEffectStrategy)
        {
            m_CurrentEffectStrategy->ApplyEffect(m_PerkData, World);
            
            // 라운드 기반 지속시간이 설정된 경우 PerkComponent에 등록
            if (m_PerkData.RoundDuration > 0)
            {
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (MyPS->PerkComp)
                        {
                            FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                            MyPS->PerkComp->AddRoundBasedEffect(RoundEffect);
                        }
                    }

                }
            }
        }
    }
}

void USkillPerk::OnPerkDeactivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::Skill)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // 전략 패턴을 사용하여 효과 제거
        USkillEffectStrategy* EffectStrategy = CreateEffectStrategy(m_PerkData.SkillEffectType);
        if (EffectStrategy)
        {
            EffectStrategy->RemoveEffect(m_PerkData, World);
            
            // 라운드 기반 지속시간이 설정된 경우 PerkComponent에서 제거
            if (m_PerkData.RoundDuration > 0)
            {
                APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
                if (Pawn)
                {
                    if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                    {
                        if (MyPS->PerkComp)
                        {
                            FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                            MyPS->PerkComp->RemoveRoundBasedEffect(RoundEffect);
                        }
                    }

                }
            }
        }
    }
}

void USkillPerk::OnRoundStart()
{
    // SkillBoost 효과인 경우 OnRoundStart 호출
    if (m_CurrentEffectStrategy && m_PerkData.SkillEffectType == EPerkSkillEffectType::SkillBoost)
    {
        if (USkillBoostEffect* SkillBoostEffect = Cast<USkillBoostEffect>(m_CurrentEffectStrategy))
        {
            SkillBoostEffect->OnRoundStart();
        }
    }
}

USkillEffectStrategy* USkillPerk::CreateEffectStrategy(EPerkSkillEffectType EffectType)
{
    switch (EffectType)
    {
        case EPerkSkillEffectType::SkillBoost:
            return NewObject<USkillBoostEffect>(this);
            
        default:
            return nullptr;
    }
}

// URoundBonusPerk 구현
URoundBonusPerk::URoundBonusPerk()
{
}

void URoundBonusPerk::OnPerkActivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::RoundBonus)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // RoundExpIncrease인 경우 초기화 로그
        if (m_PerkData.RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
        {
        }
        
        // RoundDuration 설정 (0이면 영구 지속, 아니면 정확히 RoundDuration만큼)
        if (m_PerkData.RoundDuration == 0)
        {
            m_RemainingRounds = 0; // 영구 지속
        }
        else
        {
            m_RemainingRounds = m_PerkData.RoundDuration; // RoundDuration만큼 (다음 라운드부터 적용)
        }
        m_bIsActive = true;
        
        // 다음 라운드를 기다리도록 설정 (즉시 적용하지 않음)
        m_bWaitingForNextRound = true;
        m_bIsActivated = false;
        
        // 라운드 기반 지속시간이 설정된 경우 PerkComponent에 등록
        if (m_PerkData.RoundDuration > 0)
        {
            APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
            if (Pawn)
            {
                if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                {
                    if (MyPS->PerkComp)
                    {
                        FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                        MyPS->PerkComp->AddRoundBasedEffect(RoundEffect);
                    }
                }
            }
        }
        
        // RoundExpIncrease인 경우 초기화 완료 로그
        if (m_PerkData.RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
        {
        }
    }
}

void URoundBonusPerk::OnPerkDeactivated_Implementation()
{
    if (m_PerkData.PerkType == EPerkType::RoundBonus)
    {
        UWorld* World = GetWorld();
        if (!World) return;
        
        // RoundExpIncrease인 경우 비활성화 로그
        if (m_PerkData.RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
        {
        }
        
        // 라운드 기반 지속시간이 설정된 경우 PerkComponent에서 제거
        if (m_PerkData.RoundDuration > 0)
        {
            APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
            if (Pawn)
            {
                if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(Pawn->GetPlayerState()))
                {
                    if (MyPS->PerkComp)
                    {
                        FRoundBasedEffect RoundEffect(m_PerkData, MyPS->PerkComp->GetCurrentRound());
                        MyPS->PerkComp->RemoveRoundBasedEffect(RoundEffect);
                    }
                }
            }
        }
        
        // ERoundBonusEffectType은 일회성 보상이므로 RemoveRoundEffect 호출하지 않음
        // 골드, 경험치, 아이템 수량 증가는 되돌리지 않음
        
        // 상태 초기화
        m_bIsActive = false;
        m_bIsActivated = false;
        m_bWaitingForNextRound = false;
        m_RemainingRounds = 0;
    }
}

void URoundBonusPerk::OnRoundStart()
{
    if (!m_bIsActive || m_PerkData.PerkType != EPerkType::RoundBonus)
    {
        return;
    }
    
    UWorld* World = GetWorld();
    if (!World) return;
    
    // RoundExpIncrease인 경우 남은 라운드 로그
    if (m_PerkData.RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
    {
    }
    
    // 다음 라운드를 기다리는 중이면 실제 효과 적용
    if (m_bWaitingForNextRound && !m_bIsActivated)
    {
        URoundEffectStrategy* RoundStrategy = CreateRoundEffectStrategy(m_PerkData.RoundEffectType);
        if (RoundStrategy)
        {
            RoundStrategy->ApplyRoundEffect(m_PerkData, World);
            m_bIsActivated = true;
            m_bWaitingForNextRound = false;
        }
        
        // 라운드 감소
        m_RemainingRounds--;
        
        // RoundExpIncrease인 경우 남은 라운드 로그
        if (m_PerkData.RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
        {
        }
        
        // 남은 라운드가 0이 되면 퍼크 비활성화
        if (m_RemainingRounds <= 0)
        {
            DeactivatePerk();
        }
        
        // 첫 번째 라운드에서는 여기서 끝 (중복 적용 방지)
        return;
    }
    
    // 이미 활성화된 상태에서 라운드 지속시간 관리
    if (m_bIsActivated && m_RemainingRounds > 0)
    {
        // 먼저 현재 라운드에서 효과 적용
        URoundEffectStrategy* RoundStrategy = CreateRoundEffectStrategy(m_PerkData.RoundEffectType);
        if (RoundStrategy)
        {
            RoundStrategy->ApplyRoundEffect(m_PerkData, World);
        }
        
        // 그 다음 남은 라운드 감소
        m_RemainingRounds--;
        
        // RoundExpIncrease인 경우 남은 라운드 로그
        if (m_PerkData.RoundEffectType == ERoundBonusEffectType::RoundExpIncrease)
        {
        }
        
        // 남은 라운드가 0이 되면 퍼크 비활성화 (효과는 되돌리지 않음)
        if (m_RemainingRounds <= 0)
        {
            // 퍼크 비활성화 (일회성 보상이므로 되돌리지 않음)
            DeactivatePerk();
        }
    }
    // 영구 지속인 경우
    else if (m_bIsActivated && m_RemainingRounds == 0)
    {
        // 매 라운드마다 효과 적용
        URoundEffectStrategy* RoundStrategy = CreateRoundEffectStrategy(m_PerkData.RoundEffectType);
        if (RoundStrategy)
        {
            RoundStrategy->ApplyRoundEffect(m_PerkData, World);
        }
    }
}


URoundEffectStrategy* URoundBonusPerk::CreateRoundEffectStrategy(ERoundBonusEffectType EffectType)
{
    switch (EffectType)
    {
        case ERoundBonusEffectType::RoundGoldIncrease:
            return NewObject<URoundGoldIncreaseEffect>(this);
            
        case ERoundBonusEffectType::RoundExpIncrease:
            return NewObject<URoundExpIncreaseEffect>(this);
            
        case ERoundBonusEffectType::RoundItemQuantity:
            return NewObject<URoundItemQuantityEffect>(this);
            
        case ERoundBonusEffectType::RoundAllItemsQuantity:
            return NewObject<URoundAllItemsQuantityEffect>(this);
            
        default:
            return nullptr;
    }
}


