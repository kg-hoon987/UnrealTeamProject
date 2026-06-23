// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Barrack.h"
#include "Barrack_Mage.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ABarrack_Mage : public ABarrack
{
	GENERATED_BODY()
	
protected:
	int32 ClassId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Barrack_Mage.DT_Barrack_Mage"; // 데이터테이블 경로 넣기

public:
	ABarrack_Mage();

	virtual void BeginPlay() override;
	virtual void OpenUI() override;
	virtual void PreInit() override;
};
