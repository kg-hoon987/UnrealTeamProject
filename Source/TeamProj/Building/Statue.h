// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "Statue.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API AStatue : public ABuildingBase
{
	GENERATED_BODY()
public:
	AStatue();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpanwnPoint")
	FVector SpawnPoint = FVector(0.f, 0.f, 600.f);	//메쉬만들고 수정 필요.
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Statue.DT_Statue"; // 데이터테이블 경로 넣기

	virtual void PreInit() override;
	virtual void PostInit() override;

public:
	virtual void OpenUI() override;
};
