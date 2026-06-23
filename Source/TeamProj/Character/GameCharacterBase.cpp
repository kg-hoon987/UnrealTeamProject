// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "../AICon/AllyAIController.h"
#include "../Status/StatusComponent.h"
#include "../Skill/SkillComponent.h"
#include "../Damage/DamageComponent.h"
#include "../Widget/HealthBarWidget.h"
#include "../Building/BuildingBase.h"
#include "Components/SphereComponent.h"
#include "../Stage/RoundComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../MyGamePlayerState.h" 
#include "../MyGameController.h"
#include "../Item/ItemComponent.h"
#include "../MyGameAIController.h"
#include "../Perk/PerkComponent.h"
#include "Landscape.h"
#include "EngineUtils.h"

AGameCharacterBase::AGameCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
	

	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -90.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	//Capsule->SetCapsuleSize(35.f, 90.f);
	//capCom->SetCollisionProfileName(TEXT("PlayerPreset"));
	
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head")));
	Parts[0]->SetupAttachment(GetMesh());
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hair")));
	Parts[1]->SetupAttachment(GetMesh());
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Face")));
	Parts[2]->SetupAttachment(GetMesh());
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoulder")));
	Parts[3]->SetupAttachment(GetMesh());
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hand")));
	Parts[4]->SetupAttachment(GetMesh());
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Leg")));
	Parts[5]->SetupAttachment(GetMesh());
	Parts.Add(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Foot")));
	Parts[6]->SetupAttachment(GetMesh());

	HealthScene= CreateDefaultSubobject<USceneComponent>(TEXT("HealthScene"));
	HealthScene->SetupAttachment(RootComponent);
	HealthScene->SetRelativeLocation(FVector(0.f, -20.f, 250.f));


	HealthWidget=CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(HealthScene);
	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidget->SetDrawSize(FVector2D(100.f, 30.f));
	HealthWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	WeaponLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponLeft"));
	WeaponLeft->SetupAttachment(GetMesh());
	WeaponRight= CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponRight"));
	WeaponRight->SetupAttachment(GetMesh());
	WeaponLeft->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponRight->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	StatusComp = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComp"));
	SkillComp = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComp"));
	DamageComp = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComp"));

	

}

void AGameCharacterBase::LaboratoryTickHealEf(bool onTickHeal,int32 Heal)
{
	isTickHeal = onTickHeal;
	TickHeal = Heal;
}

// Called when the game starts or when spawned
void AGameCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMeshComponent* Leader = GetMesh();

	for (USkeletalMeshComponent* pt : Parts)
	{
		if (pt && Leader)
			pt->SetLeaderPoseComponent(Leader);
	}
}

// Called every frame
void AGameCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	URoundComponent* RC = PS->FindComponentByClass<URoundComponent>();

	HealTickTime += DeltaTime;
	if (isTickHeal && (RC->RoundState == ERoundState::WarStart) && (HealTickTime >= 1.0f))
	{
		HealTickTime = 0.f;
		UDamageComponent* DC = this->FindComponentByClass<UDamageComponent>();
		if (!DC)
		{
			return;
		}
		DC->CalculateHeal(this, TickHeal);
	}

}

// Called to bind functionality to input
void AGameCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void AGameCharacterBase::InitCharInfo(EClassType ClassType, FClassConfig StatID, TArray<FSkillConfig> SkillIDs,int32 SkillLevel, USkeletalMesh* pChest, TArray<USkeletalMesh*> pParts, UStaticMesh* pWeaponLeft, UStaticMesh* pWeaponRight, TSubclassOf<UAnimInstance> pAnimInstance, FVector pCharScale)
{
	StatusComp->Init(ClassType, StatID);
	SkillComp->InitSkill(SkillIDs, SkillLevel);
	AttachParts(pChest, pParts);
	AttachWeapon(StatID, pWeaponLeft, pWeaponRight);
	GetMesh()->SetRelativeScale3D(pCharScale);

	GetMesh()->SetAnimInstanceClass(pAnimInstance);
	
}

void AGameCharacterBase::SetDied()
{
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->Team = ETeam::NoTeam;
	SetGenericTeamId(FGenericTeamId((uint8)ETeam::NoTeam));
	if (AMyGameAIController* AICon = Cast<AMyGameAIController>(GetController()))
	{
		AICon->SetGenericTeamId(FGenericTeamId((uint8)ETeam::NoTeam));
	}

}



void AGameCharacterBase::AttachParts(USkeletalMesh* pChest, TArray<USkeletalMesh*> pParts) const
{
	GetMesh()->SetSkeletalMesh(pChest);
	int32 i = 0;
	for (auto pt : pParts)
	{
		if (!pt)
		{
			++i;
			continue;
		}


		Parts[i++]->SetSkeletalMesh(pt);
	}
}

void AGameCharacterBase::AttachWeapon(FClassConfig pStat, UStaticMesh* pWeaponLeft, UStaticMesh* pWeaponRight) const
{
	USkeletalMeshComponent* Chest = GetMesh();
	if (Chest && pWeaponLeft)
	{
		const FName SocketName(TEXT("Weapon_l"));

		if (Chest->DoesSocketExist(SocketName))
		{
			WeaponLeft->AttachToComponent(Chest, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
			WeaponLeft->SetStaticMesh(pWeaponLeft);
		}
	}
	if (Chest && pWeaponRight)
	{
		const FName SocketName(TEXT("Weapon_r"));

		if (Chest->DoesSocketExist(SocketName))
		{
			WeaponRight->AttachToComponent(Chest, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
			WeaponRight->SetStaticMesh(pWeaponRight);
		}
	}
}

void AGameCharacterBase::WRAP_BuildingEfInterface()

{
	InitMyGlEf(this, this, Team);

}



///////////////////////Player//////////////////////////////////////////////////

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	IsDetach = false;
	DesiredArmLength = 2000.f;
	CameraSpeed = 4000.f;
	MinArmLength = 1000.f;
	MaxArmLength = 3000.f;
	Dragon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Dragon"));
	Dragon->SetupAttachment(GetMesh());
	Dragon->SetRelativeScale3D(FVector(2.f, 2.f, 2.f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = DesiredArmLength;
	SpringArm->SetRelativeRotation(FRotator(-50.f,0.f,0.f));
	SpringArm->bEnableCameraLag= true;
	SpringArm->CameraLagSpeed = 10.f;
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritYaw = false;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	Camera->SetupAttachment(SpringArm);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapShere"));
	OverlapSphere->SetupAttachment(GetRootComponent());
	OverlapSphere->SetGenerateOverlapEvents(true);
	OverlapSphere->SetSphereRadius(50.f);
	OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	CurrencyColl = CreateDefaultSubobject<USphereComponent>(TEXT("CurrencyColl"));
	CurrencyColl->SetupAttachment(GetRootComponent());
	CurrencyColl->SetGenerateOverlapEvents(true);
	CurrencyColl->SetSphereRadius(200.f);
	CurrencyColl->SetCollisionProfileName(TEXT("Player"));

	DefaultAttackRadius = CreateDefaultSubobject<USphereComponent>(TEXT("DefaultAttackRadiusOverlap"));
	DefaultAttackRadius->SetupAttachment(GetRootComponent());
	DefaultAttackRadius->SetGenerateOverlapEvents(true);
	DefaultAttackRadius->SetCollisionProfileName(TEXT("NoCollision"));


	ItemComp = CreateDefaultSubobject<UItemComponent>(TEXT("ItemComponent"));
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = true;
	}

	HoldTabWidgetScene = CreateDefaultSubobject<USceneComponent>(TEXT("HoldTabScene"));
	HoldTabWidgetScene->SetupAttachment(RootComponent);
	HoldTabWidgetScene->SetRelativeLocation(FVector(0.f, -20.f, 250.f));


	HoldTabWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HoldTabWidget"));
	HoldTabWidget->SetupAttachment(HealthScene);
	HoldTabWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HoldTabWidget->SetDrawSize(FVector2D(50.f, 10.f));
	HoldTabWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Team = ETeam::Player;
	
}

void APlayerCharacter::SetHoldTabWidget(UUserWidget* Widget)
{
	HoldTabWidget->SetWidget(Widget);
}

void APlayerCharacter::SetAliveState(bool bAlive)
{
	SetActorHiddenInGame(!bAlive);
	GetMesh()->SetVisibility(bAlive, true);
	if (HealthWidget) HealthWidget->SetVisibility(bAlive);

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		//Capsule->SetCollisionEnabled(bAlive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
		Capsule->SetCollisionProfileName(TEXT("Pawn"));
	}
	GetMesh()->SetCollisionEnabled(bAlive ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	
	
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
		(bAlive ? Move->SetMovementMode(MOVE_Walking) : Move->DisableMovement());

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetIgnoreMoveInput(!bAlive);
		PC->SetIgnoreLookInput(!bAlive);
	}

	Team = bAlive ? ETeam::Player : ETeam::NoTeam;

	if (SkillComp)  SkillComp->SetComponentTickEnabled(bAlive);
}

void APlayerCharacter::DiedPlayer()
{
	SetAliveState(false);
	DeadStart = true;
	DetachCamera();
	SetActorLocation(FVector(0, 0, 1000.f));


}

void APlayerCharacter::GenPlayer()
{
	AMyGamePlayerState* PS = Cast<AMyGamePlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	FVector RespawnPos = PS->RespawnLocation();
	SetActorLocation(FVector(RespawnPos.X - 800.f, RespawnPos.Y, RespawnPos.Z + 50.f));
	DeadStart = false;
	RespawnDelay = 10.f;
	if (StatusComp)
		StatusComp->ResetHPToMax();
	SetAliveState(true);
	AttachCamera();
	UE_LOG(LogTemp, Warning, TEXT("COnt = %s"), *this->GetController()->GetName());
	LaunchCharacter(FVector(0.f, 0.f, 500.f),false,true);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ItemComp)
	{
		ItemComp->InitAllItems();
	}

	OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapBegin);
	OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnOverlapEnd);
	DefaultAttackRadius->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnDefaultAttackOverlap);
	DefaultAttackRadius->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnDefaultAttackOverlapEnd);

	
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnWarStart.AddDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
			
		}
		if (UBuildingStateComponent* buildingStateCom = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)

		{
			buildingStateCom->OnActivate_Player.AddDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
		}
	}
}



void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnWarStart.RemoveDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
		
		}
		if (UBuildingStateComponent* buildingStateCom = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
		{
			buildingStateCom->OnActivate_Player.RemoveDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
		}
	}



	Super::EndPlay(EndPlayReason);
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DeadStart)
	{
		RespawnDelay -= DeltaTime;

		if (RespawnDelay <= 0.f)
		{
			GenPlayer();
		}
	}

	if (SpringArm)
	{
		const float Cur = SpringArm->TargetArmLength;
		const float NewLen = FMath::FInterpTo(Cur, DesiredArmLength, DeltaTime, 10.f);
		SpringArm->TargetArmLength = NewLen;
	}

	DirectionCameraMove(DeltaTime);
	
	USkillComponent* SC = FindComponentByClass<USkillComponent>();
	if (!SC) return;
	if (bDefaultAttack)
	{
		SC->TryUseSKill(3);
	}
}

void APlayerCharacter::InitCharInfo(EClassType ClassType, FClassConfig StatID, TArray<FSkillConfig> SkillIDs, int32 SkillLevel, USkeletalMesh* pChest, TArray<USkeletalMesh*> pParts, UStaticMesh* pWeaponLeft, UStaticMesh* pWeaponRight, TSubclassOf<UAnimInstance> pAnimInstance, FVector CharScale)
{
	Super::InitCharInfo(ClassType, StatID, SkillIDs, SkillLevel, pChest, pParts, pWeaponLeft, pWeaponRight, pAnimInstance, CharScale);

	USkeletalMeshComponent* DragonRiding = GetMesh();
	if (!GetMesh()) return;

	if (DragonRiding)
	{
		const FName SocketName(TEXT("Dragon"));

		if (DragonRiding->DoesSocketExist(SocketName))
		{
			Dragon->AttachToComponent(DragonRiding, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		}
	}
	SettingDefaultAttack();
}



void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	
}

void APlayerCharacter::DetachCamera()
{
	IsDetach = true;
	SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	SpringArm->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
}

void APlayerCharacter::AttachCamera()
{
	IsDetach = false;
	SpringArm->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 0.f),false,nullptr);
	SpringArm->SetRelativeRotation(FRotator(-50.f, 0.f, 0.f));
}

void APlayerCharacter::ZoomInCamera(float v)
{
	if (FMath::IsNearlyZero(v)) return;

	DesiredArmLength = FMath::Clamp(DesiredArmLength + (-v) * 100.f,MinArmLength, MaxArmLength);
}

void APlayerCharacter::DirectionCameraMove(float DeltaTime)
{
	float X{};
	float Y{};

	int32 ViewX{};
	int32 ViewY{};

	AMyGameController* Cont = Cast<AMyGameController>(GetController());
	Cont->GetMousePosition(X, Y);
	Cont->GetViewportSize(ViewX, ViewY);

	if (Y <= 150.f) CameraMove(ECameraMove::Up,DeltaTime);
	if (X <= 150.f)CameraMove(ECameraMove::Left, DeltaTime);
	if (Y >= (float)ViewY - 150.f)CameraMove(ECameraMove::Down, DeltaTime);
	if (X >= (float)ViewX - 150.f)CameraMove(ECameraMove::Right, DeltaTime);

}


void APlayerCharacter::CameraMove(ECameraMove CameraWay,float DeltaTime)
{
	if (!IsDetach) return;
	float MoveSpeed = CameraSpeed * DeltaTime;
	switch (CameraWay)
	{
	case ECameraMove::Up:
		SpringArm->AddRelativeLocation(FVector(MoveSpeed, 0.f, 0.f), false, nullptr);
		if (Max.X <= SpringArm->GetComponentLocation().X)
		{
			FVector pos = SpringArm->GetComponentLocation();
			SpringArm->SetWorldLocation(FVector(Max.X, pos.Y, pos.Z), false, nullptr);
		}
		break;
	case ECameraMove::Down:
		SpringArm->AddRelativeLocation(FVector(MoveSpeed * -1.f, 0.f, 0.f), false, nullptr);
		if (Min.X >= SpringArm->GetComponentLocation().X)
		{
			FVector pos = SpringArm->GetComponentLocation();
			SpringArm->SetWorldLocation(FVector(Min.X, pos.Y, pos.Z), false, nullptr);
		}
		break;
	case ECameraMove::Left:
		SpringArm->AddRelativeLocation(FVector(0.f, MoveSpeed * -1.f, 0.f), false, nullptr);
		if (Min.Y >= SpringArm->GetComponentLocation().Y)
		{
			FVector pos = SpringArm->GetComponentLocation();
			SpringArm->SetWorldLocation(FVector(pos.X, Min.Y, pos.Z), false, nullptr);
		}
		break;
	case ECameraMove::Right:
		SpringArm->AddRelativeLocation(FVector(0.f, MoveSpeed, 0.f), false, nullptr);
		if (Max.Y <= SpringArm->GetComponentLocation().Y)
		{
			FVector pos = SpringArm->GetComponentLocation();
			SpringArm->SetWorldLocation(FVector(pos.X, Max.Y, pos.Z), false, nullptr);
		}
		break;
	}
}

void APlayerCharacter::SettingDefaultAttack()
{
	USkill* DS = SkillComp->GetSkillSlot(3);
	float Radius = DS->GetSkillData().Radius + DS->GetSkillLevelData().AddRadius; 
	DefaultAttackRadius->SetSphereRadius(Radius);
	DefaultAttackRadius->bHiddenInGame = true;
	DefaultAttackRadius->SetGenerateOverlapEvents(true);
}


void APlayerCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		if (OtherActor)
		{
			if (ABuildingBase* BB = Cast<ABuildingBase>(OtherActor))
			{
				BuildingInfo = BB;
			}
		}


}


void APlayerCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (BuildingInfo)
	{
		if(ABuildingBase* BB = Cast<ABuildingBase>(BuildingInfo))
		{
			if (BB->GetBuildingUI())
				BB->GetBuildingUI()->RemoveFromParent();

			if (AMyGameController* GC = Cast<AMyGameController>(GetController()))
			{
				GC->CloseBuildingUI();
			}
		}
	}
	
	BuildingInfo = nullptr;
}

void APlayerCharacter::OnDefaultAttackOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemyCharacter* EC = Cast<AEnemyCharacter>(OtherActor))
	{
		bDefaultAttack = true;
	}
}

void APlayerCharacter::OnDefaultAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AEnemyCharacter* EC = Cast<AEnemyCharacter>(OtherActor))
	{
		bDefaultAttack = false;
	}
}


///////////////////////Enemy//////////////////////////////////////////////////



AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Team = ETeam::Enemy;
}

void AEnemyCharacter::BeginPlay()
{

	Super::BeginPlay();
	
	if (!StatusComp)
	{
		return;
	}

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		AMyGamePlayerState* GamePlayerState = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
		if (GamePlayerState)
		{
			if (URoundComponent* RoundComp = GamePlayerState->RoundCom)
			{
				RoundComp->OnWarStart.AddDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
			}
			if (UBuildingStateComponent* buildingStateCom = GamePlayerState->BuildingCom)
			{
				buildingStateCom->OnActivate_Enemy.AddDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
			}
		}
	}

	if (StatusComp->GetClassType() == EClassType::None)
	{
		GetWorld()->GetTimerManager().SetTimer(DelayedPerkApplicationTimer, this, &AEnemyCharacter::ApplyPerksAfterInit, 0.1f, false);
		return;
	}

	ApplyPerksToNewEnemy();
}

void AEnemyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnWarStart.RemoveDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
			
		}
		if (UBuildingStateComponent* buildingStateCom = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
		{
			buildingStateCom->OnActivate_Enemy.RemoveDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
		}
	}

	Super::EndPlay(EndPlayReason);
}


void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AEnemyCharacter::ApplyPerksToNewEnemy()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		AMyGamePlayerState* GamePlayerState = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
		if (GamePlayerState)
		{
			if (UPerkComponent* PerkComp = GamePlayerState->PerkComp)
			{
				PerkComp->ApplyAllEnemyStatModifiersToNewEnemy(StatusComp);
				PerkComp->ApplyAllEnemyWeaponStatModifiersToNewEnemy(StatusComp);
			}
			else
			{
			}
		}
	}
}

void AEnemyCharacter::ApplyPerksAfterInit()
{
	if (StatusComp && StatusComp->GetClassType() != EClassType::None)
	{
		ApplyPerksToNewEnemy();
	}
	else
	{
	}
}

///////////////////////Ally//////////////////////////////////////////////////


AAllyCharacter::AAllyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Team = ETeam::Ally;
}

void AAllyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!StatusComp)
	{
		return;
	}

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		AMyGamePlayerState* GamePlayerState = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
		if (GamePlayerState)
		{
			if (URoundComponent* RoundComp = GamePlayerState->RoundCom)
			{
				RoundComp->OnWarStart.AddDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
			}
			if (UBuildingStateComponent* buildingStateCom = GamePlayerState->BuildingCom)
			{
				buildingStateCom->OnActivate_Ally.AddDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
			}
		}
	}

	if (StatusComp->GetClassType() == EClassType::None)
	{
		GetWorld()->GetTimerManager().SetTimer(DelayedPerkApplicationTimer, this, &AAllyCharacter::ApplyPerksAfterInit, 0.1f, false);
		return;
	}
	ApplyPerksToNewAlly();

}

void AAllyCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnWarStart.RemoveDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
		
		}
		if (UBuildingStateComponent* buildingStateCom = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
		{
			buildingStateCom->OnActivate_Ally.RemoveDynamic(this, &AGameCharacterBase::WRAP_BuildingEfInterface);
		}
	}

	Super::EndPlay(EndPlayReason);
}


void AAllyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAllyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
void AAllyCharacter::SelectUnit_Implementation()
{
	USkeletalMeshComponent* Leader = GetMesh();

	Leader->SetRenderCustomDepth(true);
	for (USkeletalMeshComponent* pt : Parts)
	{
		if (pt && Leader)
			pt->SetRenderCustomDepth(true);
	}
}
void AAllyCharacter::DeSelectUnit_Implementation()
{
	USkeletalMeshComponent* Leader = GetMesh();

	Leader->SetRenderCustomDepth(false);
	for (USkeletalMeshComponent* pt : Parts)
	{
		if (pt && Leader)
			pt->SetRenderCustomDepth(false);
	}
	
}

void AAllyCharacter::Move_Implementation(FVector2D movePos)
{
	
	if (AAllyAIController* Con = Cast<AAllyAIController>(GetController()))
	{
		Con->MoveFromMousePick(movePos);
	}
	
	
	
}

void AAllyCharacter::ApplyPerksToNewAlly()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		AMyGamePlayerState* GamePlayerState = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
		if (GamePlayerState)
		{
			if (UPerkComponent* PerkComp = GamePlayerState->PerkComp)
			{
				PerkComp->ApplyAllAllyStatModifiersToNewAlly(StatusComp);
				PerkComp->ApplyAllWeaponStatModifiersToNewAlly(StatusComp);
			}
			else
			{
			}
		}
	}
}

void AAllyCharacter::ApplyPerksAfterInit()
{
	if (StatusComp && StatusComp->GetClassType() != EClassType::None)
	{
		ApplyPerksToNewAlly();
	}
	else
	{
	}
}


///////////////////////Boss//////////////////////////////////////////////////



ABossCharacter::ABossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	Team = ETeam::Enemy;
}

void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
