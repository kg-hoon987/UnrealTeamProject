// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GenericTeamAgentInterface.h"
#include "GameUnitBase.generated.h"


UENUM(BlueprintType)
enum class ETeam : uint8
{
    None = 0,
    Player = 1,
    Ally = 2,
    Building = 3,
    Enemy = 4,
    NoTeam = 255
};


UCLASS()
class AGameUnitBase : public AActor, public IGenericTeamAgentInterface
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
    ETeam Team = ETeam::None;

    virtual FGenericTeamId GetGenericTeamId() const override { return FGenericTeamId(static_cast<uint8>(Team)); }
    virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override { Team = static_cast<ETeam>(InTeamID.GetId()); }
};