// Fill out your copyright notice in the Description page of Project Settings.


#include "CurrencyBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../Character/GameCharacterBase.h"
#include "Kismet/GameplayStatics.h"

ACurrencyBase::ACurrencyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	InterpToMovement = CreateDefaultSubobject<UInterpToMovementComponent>(TEXT("InterpToMovement"));
	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	SphCom = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphere"));
	SphCom->SetupAttachment(Root);
	SphCom->SetSphereRadius(70.f);
	SphCom->SetCollisionProfileName(TEXT("Currency"));
	SphCom->SetGenerateOverlapEvents(true);

	OverlapCom = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphere"));
	OverlapCom->SetupAttachment(Root);
	OverlapCom->SetSphereRadius(1000.f);
	OverlapCom->SetCollisionProfileName(TEXT("Currency"));
	OverlapCom->SetGenerateOverlapEvents(true);

	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->bRotationFollowsVelocity = true;
	Move->bInitialVelocityInLocalSpace = true;
	Move->ProjectileGravityScale = 0.f;
	Move->InitialSpeed = 1000.f;
	Move->MaxSpeed = 1000.f;
	Move->bIsHomingProjectile = false;
	Move->bAutoActivate = false;                 // 필요할 때만 활성화

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(SphCom);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void ACurrencyBase::BeginPlay()
{
	Super::BeginPlay();

	// UpdatedComponent를 안전하게 지정
	if (Move && SphCom)
	{
		Move->SetUpdatedComponent(SphCom);
	}

	OverlapCom->OnComponentBeginOverlap.AddDynamic(this, &ACurrencyBase::OnOuterOverlap);
	SphCom->OnComponentBeginOverlap.AddDynamic(this, &ACurrencyBase::OnInnerOverlap);
}

void ACurrencyBase::OnOuterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCollected) return;

	APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!PC) return;
	if (OtherActor != PC) return;

	// 바깥 구역에 들어오면 호밍 전환
	if (Move && !Move->bIsHomingProjectile)
	{
		InterpToMovement->Deactivate();
		RotatingMovement->Deactivate();

		Move->bIsHomingProjectile = true;
		Move->HomingTargetComponent = PC->GetRootComponent();
		Move->HomingAccelerationMagnitude = 20000.f;

		const FVector ToTarget = (PC->GetRootComponent()->GetComponentLocation() - GetActorLocation()).GetSafeNormal();
		Move->Velocity = ToTarget * FMath::Max(Move->Velocity.Size(), Move->InitialSpeed);

		Move->Activate(); // 실제로 움직이기 시작
	}
}

void ACurrencyBase::OnInnerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("[as]1"));
	if (bCollected) return;

	APlayerCharacter* PC = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (!PC) return;
	if (OtherActor != PC) return;
	UE_LOG(LogTemp, Warning, TEXT("[as]2"));

	Collect(PC);
}

void ACurrencyBase::Collect(AActor* Picker)
{
	UE_LOG(LogTemp, Warning, TEXT("[as]3"));
	if (bCollected) return;
	bCollected = true;

	// 재진입 차단: 충돌/델리게이트/이동 모두 끄기
	if (OverlapCom) { OverlapCom->SetGenerateOverlapEvents(false); OverlapCom->SetCollisionEnabled(ECollisionEnabled::NoCollision); }
	if (SphCom) { SphCom->SetGenerateOverlapEvents(false);     SphCom->SetCollisionEnabled(ECollisionEnabled::NoCollision); }
	if (Move) { Move->StopMovementImmediately(); Move->Deactivate(); }
	if (InterpToMovement) InterpToMovement->Deactivate();
	if (RotatingMovement) RotatingMovement->Deactivate();

	// 보상 지급은 파생이 수행
	OnGet();

	// 안전 파괴: 즉시 Destroy() 대신 아주 짧은 Lifespan도 안정적
	SetLifeSpan(0.01f);
}

void ACurrencyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}