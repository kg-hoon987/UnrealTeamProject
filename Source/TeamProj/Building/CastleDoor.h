// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "CastleDoor.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ACastleDoor : public ABuildingBase
{
	GENERATED_BODY()
	
public:
	ACastleDoor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Door.DT_Door"; // 데이터테이블 경로 넣기

	virtual void PreInit() override;
};
