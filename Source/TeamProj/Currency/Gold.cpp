#include "Gold.h"
#include "Kismet/GameplayStatics.h"
#include "../MyGamePlayerState.h"

AGold::AGold()
{
    Mesh->SetRelativeScale3D(FVector(8.f, 8.f, 8.f));
}

void AGold::BeginPlay()
{
    Super::BeginPlay();

    if (MeshSmall.IsNull()) MeshSmall = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Building/Models/SM_Coin_01.SM_Coin_01")));
    if (MeshMid.IsNull())   MeshMid = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Building/Models/SM_Coin_06.SM_Coin_06")));
    if (MeshBig.IsNull())   MeshBig = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Building/Models/SM_Coin_07.SM_Coin_07")));
    if (MeshBag.IsNull())   MeshBag = TSoftObjectPtr<UStaticMesh>(FSoftObjectPath(TEXT("/Game/Building/Models/SM_Money_Bag_02.SM_Money_Bag_02")));

    if (RotatingMovement) { RotatingMovement->RotationRate = FRotator(0.f, 180.f, 0.f); }
    if (InterpToMovement)
    {
        InterpToMovement->ResetControlPoints();
        InterpToMovement->AddControlPointPosition(FVector(0, 0, 0));
        InterpToMovement->AddControlPointPosition(FVector(0, 0, 100));
        InterpToMovement->Duration = 1.0f;
        InterpToMovement->BehaviourType = EInterpToBehaviourType::PingPong;
        InterpToMovement->bPauseOnImpact = false;
    }

    GetMoney(Money);
}

void AGold::OnGet()
{
    if (APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
        if (APlayerState* PS = Pawn->GetPlayerState())
            if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(PS))
                MyPS->Gold += Money;
}

void AGold::GetMoney(int32 InMoney)
{
    Money = InMoney;

    UStaticMesh* UseMesh = nullptr;

    if (Money < 5)              UseMesh = MeshSmall.LoadSynchronous();
    else if (Money < 10)        UseMesh = MeshMid.LoadSynchronous();
    else if (Money < 15)        UseMesh = MeshBig.LoadSynchronous();
    else                        UseMesh = MeshBag.LoadSynchronous();

    if (UseMesh)
    {
        Mesh->SetStaticMesh(UseMesh);
        if (Money < 5)
            Mesh->SetWorldRotation(FRotator(90.f, 0.f, 0.f));
    }
}