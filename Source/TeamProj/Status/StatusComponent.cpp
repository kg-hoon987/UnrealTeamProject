// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusComponent.h"
#include "../Character/GameCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Building/BuildingBase.h"
#include "../MyGamePlayerState.h"
#include "../Currency/Gold.h"
#include "../Currency/Exp.h"
#include "../Stage/RoundComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"  //추가3

UStatusComponent::UStatusComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	m_CurrentStat.CntLevel = 0;
	m_CurrentStat.CntHP = 0.f;
	m_CurrentStat.CntEXP = 0;
	m_HPBackRatio = 0.f;
	m_HPFrontRatio = 0.f;
	m_bHPRotationRatio = false;
}

void UStatusComponent::Init(const EClassType ClassType, const FClassConfig& pClassSetConfig)
{
	m_ClassType = ClassType;
	m_StatTable = nullptr;
	m_Level = 1;

	switch (m_ClassType)
	{
	case EClassType::None: 
		break;
	case EClassType::Player: 
		m_StatTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Status/DT_PlayerStat.DT_PlayerStat"));
		if (!m_StatTable) return;
		break;
	case EClassType::Ally:
		m_StatTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Status/DT_AllyStat.DT_AllyStat"));
		if (!m_StatTable) return;
		break;
	case EClassType::Enemy:
		m_StatTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/Status/DT_EnemyStat.DT_EnemyStat"));
		if (!m_StatTable) return;
		break;
	}


	m_ClassID = pClassSetConfig.ClassType;
	m_WeaponID = pClassSetConfig.WeaponType;

	
	TArray<FBaseStat*> AllRows;
	m_StatTable->GetAllRows(TEXT("StatInit"), AllRows);

	FBaseStat* Found = nullptr;
	for (FBaseStat* Row : AllRows)
	{
		if (!Row) continue;
		
		if (Row->ClassID == m_ClassID && Row->WeaponID == m_WeaponID && Row->Level == pClassSetConfig.Level)
		{
			Found = Row;
			break;
		}
	}

	if (!Found)
		return;

	m_BaseStat = *Found;
	m_WeaponStat = Found->WeaponStat;

	
	m_CurrentStat.CntLevel = Found->Level;
	m_CurrentStat.CntHP = Found->MaxHP;  
	AddModifier(m_WeaponStat); 
	ProgressHPMgr(0);
}


void UStatusComponent::InitFromBaseStat(const FBaseStat& InBase, EStatusOwnerType InOwnerType, bool bResetHP)
{
	OwnerType = InOwnerType;
	m_StatTable = nullptr;        // ĳ���� ���̺� �̻��
	m_ClassType = EClassType::None;
	m_ClassID = NAME_None;
	m_WeaponID = NAME_None;
	m_Level = 1;

	m_BaseStat = InBase;
	m_WeaponStat = FStatModifier{};    // �ǹ��� ���� ������̾� ����
	m_StatModifier.Reset();

	ReCalculationStat();

	m_CurrentStat.CntLevel = 1;
	if (bResetHP) m_CurrentStat.CntHP = m_FinalStat.MaxHP;
}

void UStatusComponent::SetBaseStat(const FBaseStat& InBase, bool bKeepHpPct)
{
	const float PrevMax = m_FinalStat.MaxHP;
	const float HpPct = (PrevMax > 0.f) ? (m_CurrentStat.CntHP / PrevMax) : 0.f;

	m_BaseStat = InBase;
	ReCalculationStat();

	if (bKeepHpPct)
		ProgressHPMgr(HpPct);
}

void UStatusComponent::TakeDamage(float DamageAmount)
{
	if (m_CurrentStat.CntHP <= 0.f) return;

	AActor* pOwner = Cast<AActor>(GetOwner());

	const float TotalDmg = DamageAmount * -1;
	ProgressHPMgr(TotalDmg);
	const bool bDead = (m_CurrentStat.CntHP <= 0.f);
	OnDeath(bDead);
	if (bDead)
	{
		if (AActor* Owner = GetOwner())
		{
			OnDied.Broadcast(Owner);
		}
	}
}

void UStatusComponent::StartHPProgress()
{
	m_bHPRotationRatio = true;
}


void UStatusComponent::ProgressHPMgr(float Amount)
{
	m_CurrentStat.CntHP = FMath::Clamp( m_CurrentStat.CntHP + Amount, 0.f, m_FinalStat.MaxHP);
	m_HPFrontRatio = m_CurrentStat.CntHP / m_FinalStat.MaxHP;

	if (m_HPFrontRatio >= m_HPBackRatio)
		m_HPBackRatio = m_HPFrontRatio;
	else
		GetOwner()->GetWorldTimerManager().SetTimer(Timer_StartHPProgress, this, &UStatusComponent::StartHPProgress, 1.f, false, 0.5f);
}





void UStatusComponent::AddModifier(const FStatModifier& pModi)
{
	m_StatModifier.Add(pModi);
	ReCalculationStat();
}

void UStatusComponent::RemoveModifier(const FStatModifier& pModi)
{
	m_StatModifier.Remove(pModi);
	ReCalculationStat();
}

void UStatusComponent::ReCalculationStat()
{
	FStatModifier TotalMod = {};

	for (const auto& Mod : m_StatModifier)
	{
		TotalMod.AddHP += Mod.AddHP;
		TotalMod.AddAtk += Mod.AddAtk;
		TotalMod.AddDef += Mod.AddDef;
		TotalMod.AddCritR += Mod.AddCritR;
		TotalMod.AddCritD += Mod.AddCritD;
		TotalMod.AddAtkSpd += Mod.AddAtkSpd;
		TotalMod.AddAtkRadius += Mod.AddAtkRadius;
		TotalMod.AddSpd += Mod.AddSpd;
		TotalMod.AddCoolRec += Mod.AddCoolRec;
	}

	m_FinalStat.MaxHP				= m_BaseStat.MaxHP				+ TotalMod.AddHP;
	m_FinalStat.Attack				= m_BaseStat.Attack				+ TotalMod.AddAtk;
	m_FinalStat.Defense				= m_BaseStat.Defense			+ TotalMod.AddDef;
	m_FinalStat.CritRate			= m_BaseStat.CritRate			+ TotalMod.AddCritR;
	m_FinalStat.CritDamage			= m_BaseStat.CritDamage			+ TotalMod.AddCritD;
	m_FinalStat.AttackSpeed			= m_BaseStat.AttackSpeed		+ TotalMod.AddAtkSpd;
	m_FinalStat.AttackRadius		= m_BaseStat.AttackRadius		+ TotalMod.AddAtkRadius;
	m_FinalStat.Speed				= m_BaseStat.Speed				+ TotalMod.AddSpd;
	m_FinalStat.CooldownReduction	= m_BaseStat.CooldownReduction	+ TotalMod.AddCoolRec;
	m_FinalStat.ReqEXPToNextLevel	= m_BaseStat.ReqEXPToNextLevel;

	if (AGameCharacterBase* CB = Cast<AGameCharacterBase>(GetOwner()))
		if (UCharacterMovementComponent* CM = Cast<UCharacterMovementComponent>(CB->GetCharacterMovement())) 
			CM->MaxWalkSpeed = m_FinalStat.Speed;
}


void UStatusComponent::AddExp(int32 ExpAmount)
{
	m_CurrentStat.CntEXP += ExpAmount;
	while (m_CurrentStat.CntEXP >= m_FinalStat.ReqEXPToNextLevel)
	{
		m_CurrentStat.CntEXP -= m_FinalStat.ReqEXPToNextLevel;
		LevelUp();
	}
}

void UStatusComponent::LevelUp()
{
	if (!m_StatTable) return;

	const int32 NewLevel = m_CurrentStat.CntLevel + 1;
	TArray<FBaseStat*> AllRows;
	m_StatTable->GetAllRows(TEXT("LevelUp"), AllRows);

	FBaseStat* Next = nullptr;
	for (FBaseStat* Row : AllRows)
	{
		if (!Row) continue;
		if (Row->ClassID == m_ClassID && Row->WeaponID == m_WeaponID && Row->Level == NewLevel)
		{
			Next = Row;
			break;
		}
	}

	if (!Next) return;

	// 레벨업 선택 위젯 띄우기
	if (AGameCharacterBase* Character = Cast<AGameCharacterBase>(GetOwner())) //추가3
	{
		if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
		{
			// 플레이어 캐릭터인지 확인
			if (Cast<APlayerCharacter>(Character))
			{
				// 레벨업 선택 위젯 클래스 로드
				TSubclassOf<UUserWidget> LevelUpSelectWidgetClass = LoadClass<UUserWidget>(nullptr, TEXT("/Game/Widget/UI/LevelUpSelect/W_LevelUpSelect.W_LevelUpSelect_C"));
				
				if (LevelUpSelectWidgetClass)
				{
					// 위젯 생성 및 화면에 띄우기
					UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, LevelUpSelectWidgetClass);
					if (NewWidget)
					{
						NewWidget->AddToViewport();
					}
				}
			}
		}
	}

	const float PrevMax = m_FinalStat.MaxHP;
	m_BaseStat = *Next;
	ReCalculationStat();
	m_CurrentStat.CntLevel = NewLevel;
	const float LevelAddHP = m_FinalStat.MaxHP - PrevMax;

	ProgressHPMgr(LevelAddHP);


}

void UStatusComponent::OnDeath(bool isDead)
{
	//�����̺�Ʈ
	if (isDead)
	{
		AActor* Owner = Cast<AActor>(GetOwner());
		switch (m_ClassType)
		{
		case EClassType::None: break;
		case EClassType::Player: 
			if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwner()))
			{
				PC->DiedPlayer();
			}
			break;
		case EClassType::Enemy:

			if (AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0)))
			{
				if (URoundComponent* RC = PS->FindComponentByClass<URoundComponent>())
				{
					RC->CheckMonsterCount();
				}
			}
			
						SpawnExp(m_BaseStat.GetEXP);
						SpawnGold(m_BaseStat.GetMoney);

			if (USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
				if (UAnimInstance* Anim = Mesh->GetAnimInstance())
				{
					if (AEnemyCharacter* PS = Cast<AEnemyCharacter>(GetOwner()))
					{
						UE_LOG(LogTemp, Warning, TEXT("Enemy Clear = %s "), *PS->GetName());
						PS->SetDied();
					}

					if (DeadMontage)
					{
						Anim->Montage_Play(DeadMontage, 1.f);
						
						
					}
				}
			break;
		case EClassType::Ally:
			if (USkeletalMeshComponent* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>())
				if (UAnimInstance* Anim = Mesh->GetAnimInstance())
				{
					if (AAllyCharacter* PS = Cast<AAllyCharacter>(GetOwner()))
					{
						PS->SetDied();
					}
					if (DeadMontage)
					{
						Anim->Montage_Play(DeadMontage, 1.f);
						
					}
				}
			//ƽ�� �߰��������
			break;
		default: break;
		}

		if (ABuildingBase* BB = Cast<ABuildingBase>(Owner))
		{
			BB->OnBuildingDied();
		}

	}
}


bool UStatusComponent::IsDead() const
{
	if (m_CurrentStat.CntHP <= 0.f)
	{
		AGameCharacterBase* CB = Cast<AGameCharacterBase>(GetOwner());
		if (!CB) return false;
		CB->SetDied();
		return true;
	}
	return false;
}

// Called when the game starts
void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStatusComponent::SpawnGold(int32 Gold)
{
	const int32 G = Gold;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = GetOwner()->GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	float RandX = FMath::FRandRange(-50.f, 50.f);
	float RandY = FMath::FRandRange(-50.f, 50.f);

	FVector RandomOffset(RandX, RandY, 0.f);
	FVector FinalLocation = SpawnLocation + RandomOffset;

	AGold* SpawnedGold = GetWorld()->SpawnActor<AGold>(
		AGold::StaticClass(),
		FinalLocation,
		SpawnRotation,
		SpawnParams
	);

	SpawnedGold->GetMoney(G);
}

void UStatusComponent::SpawnExp(int32 Exp)
{
	const int32 E = Exp;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = GetOwner()->GetActorLocation();
	FRotator SpawnRotation = FRotator::ZeroRotator;

	float RandX = FMath::FRandRange(-50.f, 50.f);
	float RandY = FMath::FRandRange(-50.f, 50.f);

	FVector RandomOffset(RandX, RandY, 0.f);
	FVector FinalLocation = SpawnLocation + RandomOffset;

	AExp* SpawnedExp = GetWorld()->SpawnActor<AExp>(
		AExp::StaticClass(),
		FinalLocation,
		SpawnRotation,
		SpawnParams
	);
	SpawnedExp->GetExp(E);

}



// Called every frame
void UStatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_bHPRotationRatio)
	{
		m_HPBackRatio = m_HPBackRatio - (DeltaTime * 0.5f);
		if (m_HPBackRatio <= m_HPFrontRatio)
		{
			m_HPBackRatio = m_HPFrontRatio;
			m_bHPRotationRatio = false;
		}
	}

	if (APlayerCharacter* PC = Cast<APlayerCharacter>(GetOwner()))
		return;
	if (IsDead() && m_ClassType != EClassType::Player)
	{
		destroyStart += DeltaTime;
		if (AGameCharacterBase* GC = Cast<AGameCharacterBase>(GetOwner()))
		{
			GC->GetMesh()->AddLocalOffset(FVector(0.f, 0.f, -1 * destroyStart * 0.1f));
			if (destroyStart > 5.f)
				GC->Destroy();
		}
	}
}

