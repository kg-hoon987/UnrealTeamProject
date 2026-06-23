// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillTrigger.h"
#include "TargetingUtil.h"
#include "../Damage/DamageComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Targeting.generated.h"

/**
 * 
 */
UCLASS(EditInlineNew)
class TEAMPROJ_API UTargeting_ConeForward : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargeting_ConeForward()
	{
		// 기본적으로 Pawn은 잡히게
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		// 건물도 잡고 싶으면 필요에 맞게 추가
		
	}



	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUsePlayerControlRotation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bPreferTargetOrPoint = true;
	
	virtual void ResolveTargets(const FSkillContext& Ctx, TArray<AActor*>& OutTargets) override
	{
		if (!Ctx.Caster || !Ctx.Data) return;

		const float Range = Ctx.Data->Range + (Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddRange : 0.f);
		const float Radius = Ctx.Data->Radius + (Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddRadius : 0.f);
		const float HalfAngle = Ctx.Data->ConeHalfAngle + (Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddConeHalfAngle : 0.f);

		FVector UseDir = FVector::ZeroVector;

		if (const APawn* P = Cast<APawn>(Ctx.Caster))
		{
			if (P->IsPlayerControlled())
			{
				//이거슨 타겟,포인트가 있으면 그쪽을 향하게 하도록한거
				if (bPreferTargetOrPoint)
				{
					if (IsValid(Ctx.OptionalTarget))
						UseDir = (Ctx.OptionalTarget->GetActorLocation() - Ctx.Caster->GetActorLocation()).GetSafeNormal();
					else if (!Ctx.OptionalLocation.IsNearlyZero())
						UseDir = (Ctx.OptionalLocation - Ctx.Caster->GetActorLocation()).GetSafeNormal();
				}
				//없다면 컨트롤러 기준 방향 사용으로
				if (UseDir.IsNearlyZero() && bUsePlayerControlRotation)
				{
					if (const AController* C = P->GetController())
						UseDir = C->GetControlRotation().Vector();
				}
				//그래도 없으면 최종 풀백 : 액터 전방으로
				if (UseDir.IsNearlyZero())
					UseDir = Ctx.Caster->GetActorForwardVector();
			}
			else
				UseDir = Ctx.Caster->GetActorForwardVector(); //ai는 전방으로 슈우ㅜㅅ
		}
		else
			UseDir = Ctx.Caster->GetActorForwardVector();

		//UE_LOG(LogTemp, Warning, TEXT("[ConeForward] Caster=%s Range=%.1f Radius=%.1f HalfAngle=%.1f Affiliation=%d"),
		//	*GetNameSafe(Ctx.Caster), Range, Radius, HalfAngle, (int32)Affiliation);

		if (ObjectTypes.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ConeForward] ObjectTypes is EMPTY! (이러면 SphereOverlap에서 'Invalid object types')"));
		}
		else
		{
			for (int32 i = 0; i < ObjectTypes.Num(); ++i)
			{
				const ECollisionChannel CC = UEngineTypes::ConvertToCollisionChannel(ObjectTypes[i]);
				//UE_LOG(LogTemp, Warning, TEXT("[ConeForward] ObjectTypes[%d]=%d  (CollisionChannel=%d)"),
				//	i, (int32)ObjectTypes[i], (int32)CC);
			}
		}


		TArray<AActor*> Temp;
		TargetingUtil::CollectActors_ConeForward(Ctx.Caster, Range, Radius, HalfAngle, Temp, ObjectTypes,TArray<AActor*>(),UseDir);
		/*DrawDebugCone(GetWorld(), Ctx.Caster->GetActorLocation(), UseDir, Range,
			FMath::DegreesToRadians(HalfAngle), FMath::DegreesToRadians(HalfAngle),
			16, FColor::Blue, false, 1.f, 0, 2.f);
			*/
		OutTargets.Reset();
		for (AActor* A : Temp)
			if (IsValid(A) && TeamFilterUtil::PassAffiliation(Ctx.Caster, A, Affiliation))
				OutTargets.AddUnique(A);
	}
};

UCLASS(EditInlineNew)
class UTargeting_Self : public UTargetingTask
{
	GENERATED_BODY()
public:
	UTargeting_Self() { Affiliation = ETargetAffiliation::SelfOnly; }

	virtual void ResolveTargets(const FSkillContext& Ctx, TArray<AActor*>& OutTargets) override
	{
		OutTargets.Reset();
		if (IsValid(Ctx.Caster))
			OutTargets.Add(Ctx.Caster);
	}
};


UCLASS(EditInlineNew)
class TEAMPROJ_API UTargeting_SelfRadius : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargeting_SelfRadius()
	{
		// 기본적으로 Pawn은 잡히게
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		// 건물도 잡고 싶으면 필요에 맞게 추가
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
	virtual void ResolveTargets(const FSkillContext& Ctx, TArray<AActor*>& OutTargets) override
	{
		if (!Ctx.Caster || !Ctx.Data) return;

		if (Affiliation == ETargetAffiliation::SelfOnly)
		{
			OutTargets.Reset();
			OutTargets.Add(Ctx.Caster);
			return;
		}

		const float Radius = Ctx.Data->Radius + (Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddRadius : 0.f);


		TArray<AActor*> Temp;
		TArray<AActor*> Ignored; Ignored.Add(Ctx.Caster);
		TargetingUtil::CollectActors_Sphere(Ctx.Caster, Radius, Temp, ObjectTypes, Ignored);

		// 2) 팀/자기자신 조건으로 필터링해서 최종 채우기
		OutTargets.Reset();
		for (AActor* A : Temp)
		{
			if (!IsValid(A)) continue;
			if (TeamFilterUtil::PassAffiliation(Ctx.Caster, A, Affiliation))
				OutTargets.AddUnique(A);
		}

		UE_LOG(LogTemp, Warning, TEXT("[SelfRadius] Aff=%d -> %d targets"),
			(int32)Affiliation, OutTargets.Num());
	}
};



UCLASS(EditInlineNew)
class TEAMPROJ_API UTargeting_PointRadius : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargeting_PointRadius()
	{
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bUseForwardFallbackWhenNoLocation = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bClampToRange = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool bSnapToGround = true;

	virtual void ResolveTargets(const FSkillContext& Ctx, TArray<AActor*>& OutTargets) override
	{
		OutTargets.Reset();

		if (!Ctx.Caster || !Ctx.Data) return;

		UWorld* W = Ctx.Caster->GetWorld();
		if (!W) return;

		const float BaseRange = Ctx.Data->Range +
			(Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddRange : 0.f);
		const float Radius = Ctx.Data->Radius +
			(Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddRadius : 0.f);

		FVector Center = Ctx.OptionalLocation;

		if (Center.IsNearlyZero() && bUseForwardFallbackWhenNoLocation)
		{
			const FVector O = Ctx.Caster->GetActorLocation();
			FVector Dir = Ctx.Caster->GetActorForwardVector();

			if (const APawn* P = Cast<APawn>(Ctx.Caster))
				if (const AController* C = P->GetController())
					Dir = C->GetControlRotation().Vector();
		
			Center = O + Dir * (BaseRange > 0.f ? BaseRange : 800.f);
		}

		if (bSnapToGround)
		{
			FHitResult Hit;
			FVector S = Center + FVector(0, 0, 3000.f);
			FVector E = Center - FVector(0, 0, 3000.f);
			FCollisionQueryParams P(SCENE_QUERY_STAT(PointTarget), false);
			P.AddIgnoredActor(Ctx.Caster);

			if (W->LineTraceSingleByChannel(Hit, S, E, ECC_Visibility, P))
				Center = Hit.ImpactPoint;
		}

		// Range 밖이면 클램프(옵션)
		if (bClampToRange && BaseRange > 0.f)
		{
			const FVector O = Ctx.Caster->GetActorLocation();
			const FVector To = Center - O;
			const float   Dist = To.Size();
			if (Dist > BaseRange)
				Center = O + To / Dist * BaseRange;
		}

		// 2) 오버랩 수집
		TArray<TEnumAsByte<EObjectTypeQuery>> Types = ObjectTypes;
		if (Types.Num() == 0)
			Types.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // 기본값 보장

		TArray<AActor*> Ignore;
		Ignore.Add(Ctx.Caster);

		TArray<AActor*> Hits;
		UKismetSystemLibrary::SphereOverlapActors(
			Ctx.Caster, Center, Radius, Types, AActor::StaticClass(), Ignore, Hits);

		// 3) 팀 필터 + NotSelf 등
		for (AActor* A : Hits)
		{
			if (!IsValid(A)) continue;
			if (!TeamFilterUtil::PassAffiliation(Ctx.Caster, A, Affiliation)) continue;
			OutTargets.AddUnique(A);
		}

		
		//DrawDebugSphere(W, Center, Radius, 16, FColor::Blue, false, 1.0f, 0, 1.5f);
		
	}
};





UCLASS(EditInlineNew)
class TEAMPROJ_API UTargeting_AutoAimSphere : public UTargetingTask
{
	GENERATED_BODY()
	
public:
	UTargeting_AutoAimSphere()
	{
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float ExtraRadius = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bRequireLOS = false;

	virtual void ResolveTargets(const FSkillContext& Ctx, TArray<AActor*>& OutTargets) override
	{
		if (!Ctx.Caster || !Ctx.Data) return;

		const float Range = Ctx.Data->Range +
			(Ctx.Data->SkillLevels.IsValidIndex(Ctx.SkillLevel) ? Ctx.Data->SkillLevels[Ctx.SkillLevel].AddRange : 0.f);

		TArray<AActor*> Candidates;
		TargetingUtil::CollectActors_Sphere(Ctx.Caster, Range, Candidates, ObjectTypes);


		const FVector O = Ctx.Caster->GetActorLocation();
		AActor* Best = nullptr; float BestD2 = TNumericLimits<float>::Max();

		for (AActor* A : Candidates)
		{
			if (!IsValid(A) || A == Ctx.Caster) continue;
			if (!TeamFilterUtil::PassAffiliation(Ctx.Caster, A, Affiliation)) continue;

			if (bRequireLOS)
			{
				FHitResult Hit;
				if (!Ctx.Caster->GetWorld()->LineTraceSingleByChannel(
					Hit, O + FVector(0, 0, 50), A->GetActorLocation() + FVector(0, 0, 50), ECC_Visibility) ||
					Hit.GetActor() != A)
				{
					continue;
				}
			}

			const float D2 = FVector::DistSquared(O, A->GetActorLocation());
			if (D2 < BestD2) { BestD2 = D2; Best = A; }
		}

		OutTargets.Reset();
		if (Best) OutTargets.Add(Best);
	}


};