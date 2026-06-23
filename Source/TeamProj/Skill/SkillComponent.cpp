// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"
#include "SkillTrigger.h"
#include "../Damage/DamageComponent.h"
#include "../Status/StatusComponent.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_Skills.Empty();
	m_Size = 4;



}


void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	InitSlots(m_Size);
	
} 


// Called every frame
void USkillComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for(int32 i = 0; i < m_Skills.Num(); ++i)
	{
		if (USkill* Skill = m_Skills[i])
		{
			const bool IsReadySkill = Skill->IsReady();
			Skill->CoolDown(DeltaTime);
			if (Skill->IsReady() && !IsReadySkill)
				OnSkillReady.Broadcast(i);
		}
	}


}

void USkillComponent::InitSlots(int32 MaxSlots)
{
	m_Size = FMath::Max(1, MaxSlots);
	m_Skills.SetNum(m_Size);
}



//플레이어는 시작전 bIsPlqyer = true
void USkillComponent::InitSkill(const TArray<FSkillConfig>& SkillSetConfig, int32 Level)
{

	UDataTable* SkillDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Skill/DT_Skill.DT_Skill"));
	if (!SkillDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to Load SkillDataTable"));
		return;
	}
	TArray<FSkillData*> AllRows;
	SkillDataTable->GetAllRows(TEXT("SkillInit"), AllRows);

	int32 CurrentIndex = 0;

	for (const FSkillConfig& Config : SkillSetConfig)
	{
		const FString TargetClass = Config.ClassType.ToString(); //"Player","Sword" , "02" -> 플레이어클래스 검무기타입의 2번스킬
		const FString WeaponType = Config.WeaponType.ToString();
		int32 SkillCount = Config.Count; //1

		int32 FoundCount = 0;

		for (FSkillData* Row : AllRows) //1~3
		{
			if (!Row) continue;

			if (!Row->SkillID.ToString().StartsWith(TargetClass)) continue;
			if (!Row->WeaponType.ToString().StartsWith(WeaponType)) continue;
			if (FoundCount >= SkillCount) break; // 0>= 1

			USkill* NewSkill = NewObject<USkill>(this);
			if (!NewSkill) return;
			NewSkill->SetSkillData(*Row);
			NewSkill->SetSkillLevel(Level);

			m_Skills[CurrentIndex++] = NewSkill;
			FoundCount++;
		}
	}

}


bool USkillComponent::TryUseSKill(int32 Index, AActor* OptionalTarget, FVector OptionalLocation)
{
	if (!m_Skills.IsValidIndex(Index)) return false;

	if (isSkillCasting) return false;

	if (AGameCharacterBase* Owner = Cast<AGameCharacterBase>(GetOwner()))
	{
		if (UStatusComponent* StatComp = Owner->FindComponentByClass<UStatusComponent>())
			if (StatComp->IsDead())
				return false;
	}

	bool SkillUse = false;
	if (USkill* Skill = m_Skills[Index])
	{
		if (Skill->GetSkillData().TriggerClass != nullptr)
		{
			
			SkillUse =  Skill->ExecuteSkill(GetOwner(), OptionalTarget, OptionalLocation);
		}
	}

	if (bUseNotifyFlow)
	{
		SkillUse =   BeginCast(Index, OptionalTarget, OptionalLocation);
	}
	else
	{
		if (USkill* Skill = m_Skills[Index])
		{
			SkillUse =  Skill->ExecuteSkill(GetOwner(), OptionalTarget, OptionalLocation);
		}
	}

	USkill* Skill = m_Skills[Index];
	if (!Skill) return false;
	if (Skill->IsReady())
	{
		isSkillCasting = true;
		return SkillUse;
	}
	return false;
}

bool USkillComponent::UpgradeSkill(FName SkillID)
{
	for (USkill* Slot : m_Skills)
		if (Slot && Slot->GetSkillData().SkillID == SkillID)
			return Slot->SkillLevelUp();
	return false;
}

USkill* USkillComponent::GetSkillSlot(int32 Index) const
{
	if (!m_Skills.IsValidIndex(Index)) return nullptr;
	return m_Skills[Index];
}

void USkillComponent::OnCastSkillStart(UAnimMontage* Montage, bool bInterrupted)
{

	isSkillCasting = false;
	UE_LOG(LogTemp, Warning, TEXT("TrySkill? = %s"), isSkillCasting ? TEXT("true") : TEXT("false"));
}

void USkillComponent::UpdateSkillData(FName SkillID, const FSkillData& NewSkillData)
{
	for (USkill* Skill : m_Skills)
	{
		if (Skill && Skill->GetSkillData().SkillID == SkillID)
		{
			Skill->SetSkillData(NewSkillData);
			OnSkillDataChanged.Broadcast(SkillID);
			return;
		}
	}
}