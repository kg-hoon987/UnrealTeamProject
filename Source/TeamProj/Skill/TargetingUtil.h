// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */


namespace TargetingUtil
{

    void CollectActors_Sphere(AActor* Origin, float Radius, TArray<AActor*>& Out,
        const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
        const TArray<AActor*>& ActorsToIgnore = {});


    void CollectActors_ConeForward(AActor* Origin, float Range, float Radius, float HalfAngleDeg,
        TArray<AActor*>& Out,
        const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
        const TArray<AActor*>& ActorsToIgnore = TArray<AActor*>(),
        const FVector& DirOverride = FVector::ZeroVector);

    static void DrawConeDebug(AActor* Origin,FVector UseDir, float Range, float HalfAngleDeg, FColor Color, float LifeTime, float Thickness);
    static void DrawPointDebug(UWorld* World, const FVector& P, bool bHit, float LifeTime);

}