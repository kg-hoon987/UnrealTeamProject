// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Engine/SceneCapture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "SpiderChart.generated.h"

UCLASS()
class TEAMPROJ_API ASpiderChart : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpiderChart();

private:
	float Radius = 200.f;

	float lineThick = 5.0f;

	float ChartId;

	TArray<FVector> Vertices_line{};
	TArray<int32> Triangles_line{};

	TArray<FVector> Vertices{};
	TArray<int32> Triangles{};
	TArray<FLinearColor> Colors;

	TArray<FVector> Dir_n{};
	TArray<FVector> MoveDir_n{};
	TArray<FVector> TargetPos{};
;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UMaterialInterface* BaseMaterial;  // 에디터에서 할당 가능

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* DynMaterial;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UProceduralMeshComponent* ProcMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneCaptureComponent2D* SceneCapture;


	virtual void Tick(float DeltaTime) override;
	
	void GenerateSpider();

	void UpdateProc(float delta);

	UFUNCTION(BlueprintCallable)
	void SetTarget(TArray<int>stat);
};
