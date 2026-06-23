// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BuildingEffectInterface.generated.h"

enum class ETeam : uint8;
struct FGLBuildingEF;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBuildingEffectInterface : public UInterface
{
	GENERATED_BODY()
};

class TEAMPROJ_API IBuildingEffectInterface
{
    GENERATED_BODY()

public:
    void InitMyGlEf(AActor* SelfActor, UObject* WorldContext, ETeam team);

    static void AddEf(AActor* SelfActor, FGLBuildingEF ef);

    static void RemoveAllEf(AActor* SelfActor);

    static void AddEf_Charcter_Impl(AActor* SelfActor, FGLBuildingEF ef);

    static void AddEf_Building_Impl(AActor* SelfActor, FGLBuildingEF ef);


    static void RemoveAllEf_Charcter_Impl(AActor* SelfActor);

    static void RemoveAllEf_Building_Impl(AActor* SelfActor);

};

UINTERFACE(MinimalAPI)
class ULaboratoryTickHealInterface : public UInterface
{
    GENERATED_BODY()
};

class TEAMPROJ_API ILaboratoryTickHealInterface
{
    GENERATED_BODY()

public:
    UFUNCTION()
    virtual void LaboratoryTickHealEf(bool onTickHeal, int32 Heal) = 0;
};

