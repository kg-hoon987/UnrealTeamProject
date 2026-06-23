// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "DoorNWall.generated.h"

class USplineComponent;
class ACastleDoor;
class ACastleWall;
/**
 * 
 */

UCLASS()
class TEAMPROJ_API ADoorNWall : public ABuildingBase
{
	GENERATED_BODY()
	
public:
	ADoorNWall();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_DoorNWall.DT_DoorNWall"; // 데이터테이블 경로 넣기
	FString DoorPath = "/Game/Data/Building/DT_Door.DT_Door";
	FString WallPath = "/Game/Data/Building/DT_Wall.DT_Wall";
	virtual void BeginPlay() override;

	virtual void PreInit() override;

	TSubclassOf<ACastleWall> wall;
	TSubclassOf<ACastleDoor> door;
	TArray<AActor*> SpawnedActors;

	float wallX;
	float doorX;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (doorIndex))
	int doorPointID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USplineComponent* SplineComp;
	int SplinePointNum;

	TArray<FSplinePointData> SavedPoints;

	TArray<FVector> spawnPos;
	TArray<FRotator> spawnRot;
	
	void CalculateSpawnTrans();

	void getMeshExtent(FString path1, FString path2);

	void SpawnDoorWall();

	virtual void PostApplyRow() override;

	virtual void PostSave(FBuildingStateInfo* Info) override;

	void RecieveData(FWallDoorManager data);
};

// 비긴하면 성벽과 문 스폰
// + 변수로 들고 있자.
// 
// 
// 얘는 메쉬 날려야하고
// 성벽과 성문은 세이브 데이터안해야함.
// 
// 레벨업하면 자식들 레벨업 시키는거 PostAppltRow()
// 
// 저번에 이미 지어져서 새로지어진다고 하면
// ImportStateData에서 자식들에게 ImportStateData하면 될듯마지막에.
//
//애체력이 뭐 필욯ㄴ가> 그리고 stimulli는 꺼야하는디
// 
// 컨스트럭트에서 스플라인 생성하고 거기다 성벽 성문 자시 스폰
//
//


