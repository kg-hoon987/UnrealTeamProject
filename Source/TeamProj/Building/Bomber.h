// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "Bomber.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ABomber : public ABuildingBase
{
	GENERATED_BODY()
private:
	ABomber();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Bomber.DT_Bomber"; // 데이터테이블 경로 넣기

	virtual void PreInit() override;
	virtual void PostInit() override;

public:
	virtual void OpenUI() override;
};
