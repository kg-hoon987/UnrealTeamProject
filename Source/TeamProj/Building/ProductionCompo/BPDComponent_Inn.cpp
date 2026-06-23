// Fill out your copyright notice in the Description page of Project Settings.


#include "BPDComponent_Inn.h"
#include "Kismet/GameplayStatics.h"
#include "../../Currency/Gold.h"
#include "../../Stage/RoundComponent.h"
#include "../../MyGamePlayerState.h"

UBPDComponent_Inn::UBPDComponent_Inn()
{
	OwnEffect = { EBuildingEffectType::Inn_Gold };// 데이터 테이블 바꾸면 얘도 바꿔야함.

}

void UBPDComponent_Inn::OnBinding()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnWarEnd.AddDynamic(this, &UBPDComponent_Inn::HandleOnActivate);
			
		}
	}
}

void UBPDComponent_Inn::HandleOnActivate()
{
	for (FBuildingEffect ef : Effects)
	{
		if (CheckEffect(ef))
		{
			OnActivate(ef); // 실제 효과 적용
		}
	}
}

void UBPDComponent_Inn::ActivateEffect(const FBuildingEffect& Effect)
{
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = GetOwner()->GetActorLocation() - GetOwner()->GetActorForwardVector() * 400;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	switch (Effect.Type)
	{
	case EBuildingEffectType::Inn_Gold:
		makeMoney = Effect.intValue;


		for (int i = 0; i < makeMoney; i++)
		{
			FVector BaseLocation = SpawnLocation + GetOwner()->GetActorRightVector() * (-40 * i);
			float RandX = FMath::FRandRange(-50.f, 50.f);
			float RandY = FMath::FRandRange(-50.f, 50.f);

			FVector RandomOffset(RandX, RandY,0.f);
			FVector FinalLocation = BaseLocation + RandomOffset;

			AGold* SpawnedGold = GetWorld()->SpawnActor<AGold>(
				AGold::StaticClass(),
				FinalLocation,
				SpawnRotation,
				SpawnParams
			);
			SpawnedGold->GetMoney(1);
		}
		break;
	default:
		break;
	}

}

// destroy될때 알아서 바인드 풀기
void UBPDComponent_Inn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			RoundComp->OnWarEnd.RemoveDynamic(this, &UBPDComponent_Inn::HandleOnActivate);
		}
	}

	Super::EndPlay(EndPlayReason);
}