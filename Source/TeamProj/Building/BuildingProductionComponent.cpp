// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingProductionComponent.h"
#include "../Character/GameCharacterBase.h" 
#include "../Damage/DamageComponent.h"
#include "../Skill/TargetingUtil.h"
#include "../Skill/TeamFilterUtil.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BuildingStateComponent.h"
#include "../MyGamePlayerState.h"
#include "Kismet/GameplayStatics.h"

UBuildingProductionComponent::UBuildingProductionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


void UBuildingProductionComponent::InitBuildingEffect(const FBuildingLevelData& Row)
{
	CurrentLevel = Row.Level;
	Effects = Row.buildingEffects;
	
	InitChild();

	
}

// Called when the game starts
void UBuildingProductionComponent::BeginPlay()
{
	Super::BeginPlay();
	OnBinding();
}

bool UBuildingProductionComponent::CheckEffect(FBuildingEffect ef)
{
	for (EBuildingEffectType Oef : OwnEffect)
	{
		if (ef.Type == Oef)
			return true;
	}


	return false;
}


// Called every frame
void UBuildingProductionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBuildingProductionComponent::OnActivate(const FBuildingEffect& effect)
{
	if (effect.isGlobal)
		RegisterEffect(effect);
	else
		ActivateEffect(effect);
}

void UBuildingProductionComponent::OnDeActivate(const FBuildingEffect& effect)
{
	if (effect.isGlobal)
	{
		UnRegisterEffect(effect);
	}
	else
	{
		DeActivateEffect(effect);
	}
}

void UBuildingProductionComponent::OnExplosion()
{
	if (!bIsBomb)
		return;



	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (UBuildingStateComponent* BSC = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
		{
			FVector ori;
			FVector ext;
			GetOwner()->GetActorBounds(false, ori, ext);
			for (UNiagaraSystem* Nia : BSC->GetExplosionFX())
			{
				FVector pos = ori + FVector(FMath::FRandRange(-ExplosionRange, ExplosionRange),
					FMath::FRandRange(-ExplosionRange, ExplosionRange),
					FMath::FRandRange(0, ext.Z)
				);
				FVector scale = FVector(FMath::FRandRange(1.f, 4.0f));
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					Nia,          // UNiagaraSystem*
					pos,     // 위치
					GetOwner()->GetActorRotation(),     // 회전
					scale,          // 스케일
					true,                   // 자동 파괴 여부
					true,
					ENCPoolMethod::None    // 풀링 방식
				);
			}
		}
	}
	TArray<AActor*> Temp;
	TArray<AActor*> Ignored; Ignored.Add(this->GetOwner());
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	TargetingUtil::CollectActors_Sphere(this->GetOwner(), ExplosionRange, Temp, ObjectTypes, Ignored);
	if (Temp.Num() != 0)
	{
		for (AActor* A : Temp)
		{
			if (!IsValid(A)) continue;
			if (TeamFilterUtil::PassAffiliation(this->GetOwner(), A, ETargetAffiliation::HostileOnly))
			{
				if (UDamageComponent* DamageComp = A->FindComponentByClass<UDamageComponent>())
				{
					DamageComp->CalculateDamage(A, ExplosionDmg);
				}
			}
		}
	}

	//터지는거 구현.
}

