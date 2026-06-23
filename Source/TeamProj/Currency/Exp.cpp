#include "Exp.h"
#include "Kismet/GameplayStatics.h"
#include "../Status/StatusComponent.h"
#include "../Character/GameCharacterBase.h"
#include "../MyGamePlayerState.h"
AExp::AExp()
{
    Mesh->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));

}

void AExp::BeginPlay()
{
    Super::BeginPlay();

    if (ExpMesh.IsNull())
    {
        ExpMesh = FSoftObjectPath(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    }

    if (UStaticMesh* SM = ExpMesh.LoadSynchronous())
    {
        Mesh->SetStaticMesh(SM);
    }

    if (!ExpMaterial.IsNull())
    {
        if (UMaterialInterface* MI = ExpMaterial.LoadSynchronous())
        {
            Mesh->SetMaterial(0, MI);
        }
    }

    if (RotatingMovement) { RotatingMovement->RotationRate = FRotator(0.f, 180.f, 0.f); }
    if (InterpToMovement)
    {
        InterpToMovement->ResetControlPoints();
        InterpToMovement->AddControlPointPosition(FVector(0.f, 0.f, 0.f));
        InterpToMovement->AddControlPointPosition(FVector(0.f, 0.f, 100.f));
        InterpToMovement->Duration = 1.0f;
        InterpToMovement->BehaviourType = EInterpToBehaviourType::PingPong;
        InterpToMovement->bPauseOnImpact = false;
    }
}

void AExp::OnGet()
{
    APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (!Pawn) return;

    if (APlayerCharacter* PC = Cast<APlayerCharacter>(Pawn))
        if (UStatusComponent* StatComp = PC->FindComponentByClass<UStatusComponent>())
            StatComp->AddExp(Exp);

    if (APlayerState* PS = Pawn->GetPlayerState())
        if (AMyGamePlayerState* MyPS = Cast<AMyGamePlayerState>(PS))
            MyPS->Exp += Exp;

}

void AExp::GetExp(const int32 InExp)
{
    Exp = InExp;
}