// Fill out your copyright notice in the Description page of Project Settings.



#include "BPDComponent_Tower.h"
#include "../Tower.h"
#include "../../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../../Character/GameCharacterBase.h"
#include "../../Status/StatusComponent.h"
#include "../../Item/SkillProjectile.h"

UBPDComponent_Tower::UBPDComponent_Tower()
{
	PrimaryComponentTick.bCanEverTick = true;
	OwnEffect = { EBuildingEffectType::Tower_Stat };

	AttackDmg = 0;
	AttackSpd = 0;
	AttackRadius = 0;
	ShotDelay = 0;
	bisRemoveDynamic = false;
	owner = Cast<ATower>(GetOwner());
}

void UBPDComponent_Tower::InitChild()
{
	for (FBuildingEffect ef : Effects)
	{
		if (CheckEffect(ef))
		{
			OnActivate(ef);
		}
	}
}

void UBPDComponent_Tower::ActivateEffect(const FBuildingEffect& Effect)
{
	AttackDmg = Effect.intValue;
	AttackSpd = Effect.floatValue_1;
	AttackRadius = Effect.floatValue_2;
	owner->InitRadius(AttackRadius);
}

void UBPDComponent_Tower::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Cast<ATower>(GetOwner())->GetCurrentLevel() <= 0) return;
	
	if (target)
	{
		FVector pos = owner->GetActorLocation();
		FVector Tpos = (IsValid(target)) ? target->GetActorLocation() : FVector::ZeroVector;

		if (Tpos == FVector::ZeroVector)
			return;

		FVector lookAtVector = Tpos - pos;
		lookAtVector = lookAtVector.GetSafeNormal();
		float Speed = 30.f;
		FRotator rotate = lookAtVector.Rotation();


		owner->RotScene->SetWorldRotation(rotate);
	}



	if (ShotDelay > 0)
	{
		ShotDelay -= DeltaTime;	
	}
	else
	{
		if (target)
		{
			Shot();
			ShotDelay = AttackSpd;
		}
	}
	

}

void UBPDComponent_Tower::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	bisRemoveDynamic = true;

	for (AActor* act : InRangeActors)
	{
		Cast<UStatusComponent>(Cast<AGameCharacterBase>(act)->StatusComp)->OnDied.RemoveDynamic(this, &UBPDComponent_Tower::RemoveActorCuaseDied);
	}

	Super::EndPlay(EndPlayReason);

}

void UBPDComponent_Tower::Shot()
{
	//DrawDebugSphere(GetWorld(), owner->ProjectileScene->GetComponentLocation(), 50.f, 4, FColor::Blue, false, 1.f);
	
	FActorSpawnParameters SpawnPara;
	SpawnPara.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASkillProjectile* Arrow;
	FVector pos = owner->ProjectileScene->GetComponentLocation();
	FRotator rot = owner->ProjectileScene->GetComponentRotation();
	Arrow = GetWorld()->SpawnActor<ASkillProjectile>(Projectile, pos, rot, SpawnPara);
	UE_LOG(LogTemp, Warning, TEXT("[Barrack]  Shotoottotoototootototo "));
	FSkillContext ctx;
	ctx.Caster = owner;
	Arrow->Init(ctx, ETargetAffiliation::HostileOnly, TArray<TSubclassOf<USkillEffect>>(), 20000, 30, true, target, 2000);
	Arrow->towerDmg = AttackDmg;
}

void UBPDComponent_Tower::RemoveActorCuaseDied(AActor* act)
{
	if (bisRemoveDynamic)  return;
	
	InRangeActors.Remove(act);
	Cast<UStatusComponent>(Cast<AGameCharacterBase>(act)->StatusComp)->OnDied.RemoveDynamic(this, &UBPDComponent_Tower::RemoveActorCuaseDied);

	if (target == act)
		target = (InRangeActors.Num() > 0) ? InRangeActors[0] : nullptr;


}
