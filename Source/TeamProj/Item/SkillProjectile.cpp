// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "../Skill/SkillTrigger.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/EngineTypes.h" 
#include "Kismet/KismetSystemLibrary.h"
#include "../Damage/DamageComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Status/StatusComponent.h"


ASkillProjectile::ASkillProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->InitSphereRadius(8.f);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECC_WorldDynamic);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	Sphere->SetGenerateOverlapEvents(true);
	RootComponent = Sphere;
	if (APawn* P = Cast<APawn>(Caster.Get()))
	{
		SetInstigator(P);
		Sphere->IgnoreActorWhenMoving(P, true);
	}

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetRelativeScale3D(FVector(1.f)); // 작게

	Move = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Move"));
	Move->InitialSpeed = 1500.f;
	Move->MaxSpeed = 1500.f;
	Move->bRotationFollowsVelocity = true;
	Move->ProjectileGravityScale = 0.f;

	VFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("VFX"));
	VFX->SetupAttachment(RootComponent);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ASkillProjectile::OnSphereOverlap);
	Sphere->OnComponentHit.AddDynamic(this, &ASkillProjectile::OnSphereHit);

	ExplosionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	//ExplosionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic)); // 건물
	//나중에 오브젝트 채널 각자 만들어서 건물이랑 넣어주면 될듯 ㅇ
}



void ASkillProjectile::Init(const FSkillContext& InCtx,
	ETargetAffiliation InAffiliation,
	const TArray<TSubclassOf<USkillEffect>>& InEffects,
	float InSpeed, float InRadius,
	bool bHoming, AActor* HomingTarget, float HomingAcceleration,
	bool bInExplodeOnGround, float InExplosionRadius)
{
	Ctx = InCtx;
	Caster = InCtx.Caster;
	Affiliation = InAffiliation;
	Effects = InEffects;

	Move->InitialSpeed = Move->MaxSpeed = InSpeed;
	Sphere->SetSphereRadius(InRadius);

	bExplodeOnGround = bInExplodeOnGround;
	if (InExplosionRadius > 0.f) ExplosionRadius = InExplosionRadius;

	//호밍
	if (bHoming && HomingTarget && HomingTarget->GetRootComponent())
	{
		Move->bIsHomingProjectile = true;
		Move->HomingAccelerationMagnitude = (HomingAcceleration > 0.f ? HomingAcceleration : 20000.f);
		Move->HomingTargetComponent = HomingTarget->GetRootComponent();
	}

	//시전자 , 타겟 설정 1
	SetOwner(Caster.Get());
	if (APawn* P = Cast<APawn>(Caster.Get()))
	{
		SetInstigator(P);
		Sphere->IgnoreActorWhenMoving(P, true);
	}
}

void ASkillProjectile::BeginPlay()
{
	Super::BeginPlay();

}

void ASkillProjectile::OnSphereOverlap(UPrimitiveComponent* Overlapped, AActor* Other, UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!IsValid(Other) || Other == this || Other == GetOwner()) return;
	if (HitOnce.Contains(Other)) return;
	//팀필터링
	if (!TeamFilterUtil::PassAffiliation(Caster.Get(), Other, Affiliation))
		return;

	ApplyEffectsToActor(Other);
	HitOnce.Add(Other);

	HitAt(Hit.ImpactPoint);
	if (towerDmg > 0)
	{
		if (AEnemyCharacter* ch = Cast<AEnemyCharacter>(Other))
		{
			if (UStatusComponent* statcom = Cast<UStatusComponent>(ch->StatusComp))
				statcom->TakeDamage(towerDmg);
			else
				UE_LOG(LogTemp, Warning, TEXT("[Arrow] 3"));
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("[Arrow] 2"));
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("[Arrow] 1"));

	if (Penetrations >= MaxPenetrations)
	{
		if (bExplodeOnGround)
			ExplodeAt(Hit.ImpactPoint);
		else
			Destroy();
	}
	else
		++Penetrations;
}

void ASkillProjectile::OnSphereHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const ECollisionChannel ObjType = OtherComp ? OtherComp->GetCollisionObjectType() : ECC_WorldStatic;
	const bool bIsWorld = (ObjType == ECC_WorldStatic || ObjType == ECC_WorldDynamic);

	if (bExplodeOnGround && bIsWorld)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExplodeStart Projectile"));
		ExplodeAt(Hit.ImpactPoint);
		return;
	}


	if (OtherActor && !TeamFilterUtil::PassAffiliation(Caster.Get(), OtherActor, Affiliation))
		return;

	if (Penetrations >= MaxPenetrations) Destroy();
	else ++Penetrations;
}

void ASkillProjectile::ApplyEffectsToActor(AActor* Target)
{
	for (auto EfClass : Effects)
	{
		if (!*EfClass) continue;
		if (USkillEffect* E = NewObject<USkillEffect>(this, EfClass))
		{
			E->Apply(Ctx, Target);
		}
	}

	if (UDamageComponent* DmgComp = Target->FindComponentByClass<UDamageComponent>())
	{
		if (Ctx.Data && Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel))
		{
			const FSkillLevelData& ExtraData = Ctx.Data->SkillLevels[Ctx.SkillLevel];

			if (ExtraData.ExtraEffect.EffectType != ESkillEffectType::None)
				DmgComp->AddStatusEffect(ExtraData.ExtraEffect);

			if (ExtraData.ExtraCC.EffectType != ESkillCCType::None)
				DmgComp->ApplyCC(ExtraData.ExtraCC, Ctx.Caster);

		}
	}

}

void ASkillProjectile::ExplodeAt(const FVector& Center)
{
	UWorld* W = GetWorld();
	if (!W) { Destroy(); return; }

	if (ExplosionFX)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(W, ExplosionFX, Center, {}, FVector(4.f, 4.f, 4.f));
	if (ExplosionSFX)
		UGameplayStatics::PlaySoundAtLocation(W, ExplosionSFX, Center);

	TArray<TEnumAsByte<EObjectTypeQuery>> Types = ExplosionObjectTypes;

	if (Types.Num() == 0)
		Types.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> Ignore;
	Ignore.Add(this);
	if (AActor* OwnerAct = GetOwner()) Ignore.Add(OwnerAct);

	TArray<AActor*> Victims;

	UKismetSystemLibrary::SphereOverlapActors(
		this, Center, ExplosionRadius, Types, AActor::StaticClass(), Ignore, Victims);

	for (AActor* V : Victims)
	{
		if (!IsValid(V)) continue;
		if (!TeamFilterUtil::PassAffiliation(Caster.Get(), V, Affiliation)) continue;
		ApplyEffectsToActor(V);
	}
	Destroy();
	return;
}

void ASkillProjectile::HitAt(const FVector& Center)
{
	UWorld* W = GetWorld();
	if (!W) { Destroy(); return; }

	if (HitFX)
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(W, HitFX, Center);
	if (HitSFX)
		UGameplayStatics::PlaySoundAtLocation(W, HitSFX, Center);
}

