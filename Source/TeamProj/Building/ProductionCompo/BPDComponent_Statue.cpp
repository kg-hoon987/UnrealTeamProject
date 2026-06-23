// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDComponent_Statue.h"
#include "Kismet/GameplayStatics.h"
#include "../../Currency/Exp.h"
#include "../../Stage/RoundComponent.h"
#include "../../MyGamePlayerState.h"


UBPDComponent_Statue::UBPDComponent_Statue()
{
    OwnEffect = { EBuildingEffectType::Statue_Exp };
}

void UBPDComponent_Statue::OnBinding()
{
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Pawn)
    {
        if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
        {
            RoundComp->OnWarEnd.AddDynamic(this, &UBPDComponent_Statue::HandleOnActivate);
          
        }
    }
}

void UBPDComponent_Statue::HandleOnActivate()
{
    for (FBuildingEffect ef : Effects)
    {
        if (CheckEffect(ef))
        {
            OnActivate(ef); // 실제 효과 적용
        }
    }
}

void UBPDComponent_Statue::ActivateEffect(const FBuildingEffect& Effect)
{
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    FVector SpawnLocation = GetOwner()->GetActorLocation() - GetOwner()->GetActorForwardVector() * 400;
    FRotator SpawnRotation = FRotator::ZeroRotator;
    switch (Effect.Type)
    {
    case EBuildingEffectType::Statue_Exp:
        makeExp = Effect.intValue;
        for (int i = 0; i < makeExp; i++)
        {
            FVector BaseLocation = SpawnLocation + GetOwner()->GetActorRightVector() * (-40 * i);
            float RandX = FMath::FRandRange(-50.f, 50.f);
            float RandY = FMath::FRandRange(-50.f, 50.f);

            FVector RandomOffset(RandX, RandY, 0.f);
            FVector FinalLocation = BaseLocation + RandomOffset;

            AExp* SpawnedExp = GetWorld()->SpawnActor<AExp>(
                AExp::StaticClass(), 
                FinalLocation,
                SpawnRotation,
                SpawnParams
            );
            SpawnedExp->GetExp(10);
        }
        break;
    default:
        break;
    }
    
}

// destroy될때 알아서 바인드 풀기
void UBPDComponent_Statue::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (Pawn)
    {
        if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
        {
            RoundComp->OnWarEnd.RemoveDynamic(this, &UBPDComponent_Statue::HandleOnActivate);
        
        }
    }

    Super::EndPlay(EndPlayReason);
}



