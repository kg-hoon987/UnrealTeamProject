// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "ProductionCompo/BPDComponent_Tower.h"
#include "Components/SphereComponent.h"
#include "../Character/GameCharacterBase.h"
#include "../Widget/UI/TowerInfo.h"
#include "../Status/StatusComponent.h"
#include "Blueprint/UserWidget.h"

ATower::ATower()
{
	ProductionComp = CreateDefaultSubobject<UBPDComponent_Tower>(TEXT("ProductionComp"));

	SphereCom = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collsion"));
	SphereCom->SetupAttachment(Root);
	SphereCom->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	SphereCom->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCom->SetGenerateOverlapEvents(true);
	SphereCom->SetVisibility(true);
	SphereCom->SetHiddenInGame(true);

	RotScene = CreateDefaultSubobject<USceneComponent>(TEXT("Rotate Scene"));
	RotScene->SetupAttachment(SphereCom);
	RotScene->SetRelativeLocation(FVector(0.f, 0.f, 600.f));

	ProjectileScene = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Scene"));
	ProjectileScene->SetupAttachment(RotScene);
	ProjectileScene->SetRelativeLocation(FVector(200.f, 0.f, 0.f));

	//widgetclass
	static ConstructorHelpers::FClassFinder<UUserWidget>UI(TEXT("/Game/Widget/UI/Tower/W_TowerBuilding.W_TowerBuilding_C"));
	if (UI.Succeeded())
	{
		BuildingUIClass = UI.Class;
	}
}

void ATower::InitRadius(float radius)
{
	SphereCom->SetSphereRadius(radius, true);
}

void ATower::BeginPlay()
{
	Super::BeginPlay();
	SphereCom->OnComponentBeginOverlap.AddDynamic(this, &ATower::OnSphereBeginOverlap);
	SphereCom->OnComponentEndOverlap.AddDynamic(this, &ATower::OnSphereEndOverlap);
}

void ATower::PreInit()
{
	SetDataTable(Path);
}

void ATower::PostInit()
{
}

AActor* ATower::FilteringEnemy(AActor* act)
{
	if (!IsValid(act)) return nullptr;
	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(act);
	if (Enemy && Enemy->Team == ETeam::Enemy)
	{
		return act;
	}
	return nullptr;
}

void ATower::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Overlap!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"));
	if (!OtherActor) return;
	UE_LOG(LogTemp, Warning, TEXT("This is myGame Character"));
	OtherActor = FilteringEnemy(OtherActor);
	if (OtherActor == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("This is myGame Character ->Enemy"));
	if (Cast<UBPDComponent_Tower>(ProductionComp)->InRangeActors.Num() <= 0)
		Cast<UBPDComponent_Tower>(ProductionComp)->target = OtherActor;

	Cast<UBPDComponent_Tower>(ProductionComp)->InRangeActors.AddUnique(OtherActor);

	Cast<UStatusComponent>(Cast<AGameCharacterBase>(OtherActor)->StatusComp)->OnDied.AddDynamic(Cast<UBPDComponent_Tower>(ProductionComp), &UBPDComponent_Tower::RemoveActorCuaseDied);
	UE_LOG(LogTemp, Warning, TEXT("[Tower] Overlap [ In ] Actor : %s"), *OtherActor->GetName());
}

void ATower::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this) return;

	OtherActor = FilteringEnemy(OtherActor);
	if (OtherActor == nullptr) return;

	Cast<UBPDComponent_Tower>(ProductionComp)->InRangeActors.Remove(OtherActor);
	Cast<UStatusComponent>(Cast<AGameCharacterBase>(OtherActor)->StatusComp)->OnDied.RemoveDynamic(Cast<UBPDComponent_Tower>(ProductionComp), &UBPDComponent_Tower::RemoveActorCuaseDied);
	UE_LOG(LogTemp, Warning, TEXT("[Tower] Overlap [ Out ] Actor : %s"), *OtherActor->GetName());
}

void ATower::OpenUI()
{
	Super::OpenUI();
	if (BuildingUIClass)
	{
		BuildingUI = CreateWidget<UUserWidget>(GetWorld(), BuildingUIClass);
		if (BuildingUI)
		{
			if (UTowerInfo* TowerUI = Cast<UTowerInfo>(BuildingUI))
			{
				TowerUI->SetBuilding(this);
			}
			BuildingUI->AddToViewport();
		}

	}
}