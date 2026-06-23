// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/InterpToMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "CurrencyBase.generated.h"

class UProjectileMovementComponent;

UCLASS()
class TEAMPROJ_API ACurrencyBase : public AActor
{
	GENERATED_BODY()
	public:
	ACurrencyBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USphereComponent* SphCom;       // “줍는” 안쪽 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USphereComponent* OverlapCom;   // “끌어당김” 바깥 범위
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UInterpToMovementComponent* InterpToMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) URotatingMovementComponent* RotatingMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) UProjectileMovementComponent* Move;

	// 픽업 중복 방지
	UPROPERTY(Transient) bool bCollected = false;

	UFUNCTION()
	void OnOuterOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnInnerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 파생에서 “보상 지급”만 하고, 파괴는 베이스가 담당
	virtual void OnGet() {}

	// 안전 픽업 처리(가드, 충돌 끄기, OnGet 호출, 파괴)
	void Collect(AActor* Picker);

public:
	virtual void Tick(float DeltaTime) override;
};
