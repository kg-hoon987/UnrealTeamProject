// Fill out your copyright notice in the Description page of Project Settings.


#include "SpiderChart.h"

// Sets default values
ASpiderChart::ASpiderChart()
{
	PrimaryActorTick.bCanEverTick = true;

    // 风飘 技泼
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // Procedural Mesh 积己
    ProcMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    ProcMesh->SetupAttachment(RootComponent);
    ProcMesh->SetRenderCustomDepth(true);
    ProcMesh->CustomDepthStencilValue = 1;

    // Scene Capture 积己
    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
    SceneCapture->SetupAttachment(RootComponent);
    SceneCapture->SetRelativeLocation(FVector(-900.f, 0.f, 0.f));
    SceneCapture->bCaptureEveryFrame = true;
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
    SceneCapture->ProjectionType = ECameraProjectionMode::Orthographic;
    SceneCapture->OrthoWidth = 400.f;

  

    // SceneCapture俊 坊歹 鸥百 利侩
    SceneCapture->ShowOnlyActors.Empty();
    SceneCapture->ShowFlags.SetLandscape(false);
    SceneCapture->ShowFlags.SetStaticMeshes(false);

    SceneCapture->ShowOnlyComponents.Add(ProcMesh);

    MoveDir_n.Init(FVector(0.f,0.f,0.f),6);
    TargetPos.Init(FVector(0.f, 0.f, 0.f), 6);

}

// Called when the game starts or when spawned
void ASpiderChart::BeginPlay()
{
	Super::BeginPlay();
    GenerateSpider();

}

// Called every frame
void ASpiderChart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    UpdateProc(DeltaTime);

    ProcMesh->UpdateMeshSection_LinearColor(0,Vertices, TArray<FVector>(),TArray<FVector2D>(),Colors,TArray<FProcMeshTangent>());
}

void ASpiderChart::GenerateSpider()
{
    Vertices.Add(FVector(-1.f, 0.f, 0.f));

    FVector back = FVector(-1.f, 0.f, Radius);
    FVector front = FVector(0.f, 0.f, Radius);
    TargetPos[0] = front;
    Vertices.Add(back);
    Vertices.Add(front);
    Dir_n.Add(front.GetSafeNormal());

    for (int i = 1; i < 6; i++)
    {
        Vertices.Add(back.RotateAngleAxis(-60.f * i, FVector(-1.f, 0.f, 0.f)));
        Vertices.Add(front.RotateAngleAxis(-60.f * i, FVector(-1.f, 0.f, 0.f)));
        Dir_n.Add(front.RotateAngleAxis(-60.f * i, FVector(-1.f, 0.f, 0.f)).GetSafeNormal());
        TargetPos[i] = front;
    }
    Vertices.Add(FVector(0.f, 0.f, 0.f));

    Triangles = {
        0,1,3
        ,0,3,5
        ,0,5,7
        ,0,7,9
        ,0,9,11
        ,0,11,1
    };
    Colors.Init(FLinearColor(1.f, 1.f, 1.f, 1.f), Vertices.Num());

    for (int i = 2; i < 13; i += 2)
    {
        Colors[i] = FLinearColor(1.f, 0.f, 1.f, 0.5f);
    }

    MoveDir_n.Init(FVector(0.f), 6);

    DynMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    if (DynMaterial)
        ProcMesh->SetMaterial(0, DynMaterial);

    for (int k = 1; k < 6; k++)
    {
        FVector a = FVector(0.5f, 0.f, (Radius * 0.2 * k));
        FVector b = FVector(0.5f, 0.f, (Radius * 0.2 * k) - lineThick);
        for (int i = 0; i < 6; i++)
        {
            Vertices.Add(a.RotateAngleAxis(-60.f * i, FVector(-1.f, 0.f, 0.f)));
            Colors.Add(FLinearColor(0.5f, 0.5f, 0.5f, 1.f));
            Vertices.Add(b.RotateAngleAxis(-60.f * i, FVector(-1.f, 0.f, 0.f)));
            Colors.Add(FLinearColor(0.5f, 0.5f, 05.f, 1.f));
        }
    }
    for (int k = 0; k < 5; k++)
    {
        for (int i = 0; i < 6; i++)
        {
            int id = 14 + i * 2 + 1 + k * 12;

            if (i == 5)
            {
                Triangles.Add(id);
                Triangles.Add(id - 1);
                Triangles.Add(14 + k * 12);

                Triangles.Add(id);
                Triangles.Add(14 + k * 12);
                Triangles.Add(15 + k * 12);
            }
            else
            {
                Triangles.Add(id);
                Triangles.Add(id - 1);
                Triangles.Add(id + 1);

                Triangles.Add(id);
                Triangles.Add(id + 1);
                Triangles.Add(id + 2);
            }
        }
    }

    Triangles.Append({ 13, 2, 4
        , 13, 4, 6
        , 13, 6, 8
        , 13, 8, 10
        , 13, 10, 12
        , 13, 12, 2
        });

    ChartId = Vertices.Num();

    for (int i = 0; i < 6; i++)
    {
        Vertices.Add(TargetPos[i] + FVector(0.5f, 0.f, 0.f));
        Colors.Add(FLinearColor(1.f, 0.f, 0.f, 1.f));
        Vertices.Add(TargetPos[i].GetSafeNormal() * (TargetPos[i].Length() + lineThick) + FVector(0.5f, 0.f, 0.f));
        Colors.Add(FLinearColor(1.f, 0.f, 1.f, 1.f));
    }
    
    for (int i = 0; i < 6; i++)
    {
        int id = ChartId + i * 2 + 1;

        if (i == 5)
        {
            Triangles.Add(id);
            Triangles.Add(ChartId);
            Triangles.Add(id - 1);

            Triangles.Add(id);
            Triangles.Add(ChartId + 1);
            Triangles.Add(ChartId);
        }
        else
        {
            Triangles.Add(id);
            Triangles.Add(id + 1);
            Triangles.Add(id - 1);

            Triangles.Add(id);
            Triangles.Add(id + 2);
            Triangles.Add(id + 1);
        }
    }


    ProcMesh->CreateMeshSection_LinearColor(
        0,
        Vertices,
        Triangles,
        TArray<FVector>(),     // Normals
        TArray<FVector2D>(),   // UVs
        Colors,
        TArray<FProcMeshTangent>(),
        true);

}

void ASpiderChart::UpdateProc(float delta)
{
    for (int i = 0; i < Dir_n.Num(); i++)
    {
        FVector movedir = (MoveDir_n[i] * delta);

        if (FVector::Dist(TargetPos[i], Vertices[i * 2 + 2] + movedir) < 5.0f)
        {
            Vertices[i * 2 + 2] = TargetPos[i];
            Vertices[ChartId + (i*2)] = TargetPos[i];
            Vertices[ChartId + (i*2) + 1] = TargetPos[i] + Dir_n[i].GetSafeNormal() * lineThick;

            MoveDir_n[i] = FVector::ZeroVector;
        }
        else
        {
            Vertices[i * 2 + 2] += movedir;
            Vertices[ChartId + (i * 2)] = Vertices[i * 2 + 2];
            Vertices[ChartId + (i * 2) + 1] = Vertices[i * 2 + 2] +Dir_n[i].GetSafeNormal() * lineThick;
        }
    }
}

void ASpiderChart::SetTarget(TArray<int>stat)
{
    for (int i = 0; i < 6; i++)
    {
        TargetPos[i]=(stat[i] * 0.2f * Radius) * Dir_n[i];
        MoveDir_n[i] = FMath::Lerp(TargetPos[i], Vertices[i * 2 + 2], 0.1f) - Vertices[i * 2 + 2];
    }
}

