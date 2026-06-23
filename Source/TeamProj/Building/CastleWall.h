// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "CastleWall.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ACastleWall : public ABuildingBase
{
	GENERATED_BODY()
	
public:
	ACastleWall();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Wall.DT_Wall"; // 데이터테이블 경로 넣기

	virtual void PreInit() override;
};
