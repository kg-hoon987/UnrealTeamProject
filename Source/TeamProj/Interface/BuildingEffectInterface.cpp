// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingEffectInterface.h"
#include "../Building/BuildingStateComponent.h"
#include "../Building/BuildingBase.h"
#include "../Character/GameCharacterBase.h"
#include "../Status/StatusComponent.h"
#include "../Building/BuildingProductionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../MyGamePlayerState.h"

void IBuildingEffectInterface::InitMyGlEf(AActor* SelfActor, UObject* WorldContext, ETeam team)
{
    if (!WorldContext || !SelfActor) return;

    RemoveAllEf(SelfActor);

    if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContext))
    {
        APawn* Pawn = UGameplayStatics::GetPlayerPawn(World, 0);
        if (Pawn)
        {
            if (UBuildingStateComponent* buildingCompo = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
            {
                TArray<FGLBuildingEF>CheckList = buildingCompo->GetGlobalEfList(team);
                for (const FGLBuildingEF& ef : CheckList)
                {
                    UE_LOG(LogTemp, Warning, TEXT("[dk] %s"), *SelfActor->GetName());
                    AddEf(SelfActor, ef);
                }
            }
        }
    }

}

void IBuildingEffectInterface::AddEf(AActor* SelfActor, FGLBuildingEF ef)
{
    if (SelfActor->IsA(ABuildingBase::StaticClass()))
        AddEf_Building_Impl(SelfActor, ef);
    else
        AddEf_Charcter_Impl(SelfActor, ef);
}

void IBuildingEffectInterface::RemoveAllEf(AActor* SelfActor)
{
    {
        if (SelfActor->IsA(ABuildingBase::StaticClass()))
            RemoveAllEf_Building_Impl(SelfActor);
        else
            RemoveAllEf_Charcter_Impl(SelfActor);
    }
}

void IBuildingEffectInterface::AddEf_Charcter_Impl(AActor* SelfActor, FGLBuildingEF ef)
{
    {
        if (!SelfActor) return;

        if (AGameCharacterBase* Unit = Cast<AGameCharacterBase>(SelfActor))
        {
            if (UStatusComponent* Statcom = Unit->StatusComp)
            {
                if (ef.hasStatEf)
                {
                    Statcom->AddModifier(ef.StatEf);
                    Unit->MyGlEf.Add(ef);
                    
                }
                else if(ef.OnOffEf)
                {
                    if (ef.Type == EBuildingEffectType::Laboratory_HPRegenPerSec)
                    {
                        ILaboratoryTickHealInterface* LaboEf = Cast<ILaboratoryTickHealInterface>(Unit);
                        if (!LaboEf) return;
                        LaboEf->LaboratoryTickHealEf(true, ef.Damage);
                    }
                }
                else
                    UE_LOG(LogTemp, Warning, TEXT("[%s] has none of StatModifier in effect : [%s]"),
                        *SelfActor->GetName(),
                        *UEnum::GetValueAsString(ef.Type));
            }
        }

    }
}

void IBuildingEffectInterface::AddEf_Building_Impl(AActor* SelfActor, FGLBuildingEF ef)
{
    {
        if (!SelfActor) return;

        if (ABuildingBase* Unit = Cast<ABuildingBase>(SelfActor))
        {
            if (UStatusComponent* Statcom = Unit->StatusComp)
            {
                if (ef.OnOffEf)
                {
                    Unit->ProductionComp->bIsBomb = true;
                    Unit->ProductionComp->ExplosionDmg = ef.Damage;
                    Unit->ProductionComp->ExplosionRange = ef.Radius;
                    Unit->MyGlEf.Add(ef);
                }
                else if (ef.hasStatEf)
                {
                    Statcom->AddModifier(ef.StatEf);
                    Unit->MyGlEf.Add(ef);
                }
                else
                    UE_LOG(LogTemp, Warning, TEXT("[%s] has none Bool,StatModifier in effect : [%s]"),
                        *SelfActor->GetName(),
                        *UEnum::GetValueAsString(ef.Type));
            }
        }
    }
}

void IBuildingEffectInterface::RemoveAllEf_Charcter_Impl(AActor* SelfActor)
{
    if (!SelfActor) return;

    if (AGameCharacterBase* Unit = Cast<AGameCharacterBase>(SelfActor))
    {
        TArray<FGLBuildingEF> efs = Unit->MyGlEf;
        if (UStatusComponent* Statcom = Unit->StatusComp)
        {
            for (FGLBuildingEF ef : efs)
            {
                if (ef.hasStatEf)
                {
                    Statcom->RemoveModifier(ef.StatEf);
                }
                else
                    UE_LOG(LogTemp, Warning, TEXT("[%s] has none of effect : [%s]"),
                        *SelfActor->GetName(),
                        *UEnum::GetValueAsString(ef.Type));
            }
            Unit->MyGlEf.Empty();
        }
    }

}

void IBuildingEffectInterface::RemoveAllEf_Building_Impl(AActor* SelfActor)
{
    if (!SelfActor) return;


    if (ABuildingBase* Unit = Cast<ABuildingBase>(SelfActor))
    {
        TArray<FGLBuildingEF> efs = Unit->MyGlEf;
        if (UStatusComponent* Statcom = Unit->StatusComp)
        {
            for (FGLBuildingEF ef : efs)
            {
                if (ef.OnOffEf)
                {
                    Unit->ProductionComp->bIsBomb = false;
                }
                else if (ef.hasStatEf)
                {
                    Statcom->RemoveModifier(ef.StatEf);
                }
                else
                    UE_LOG(LogTemp, Warning, TEXT("[%s] has none of effect : [%s]"),
                        *SelfActor->GetName(),
                        *UEnum::GetValueAsString(ef.Type));
            }
            Unit->MyGlEf.Empty();
        }
    }
}
