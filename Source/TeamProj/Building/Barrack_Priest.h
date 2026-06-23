// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Barrack.h"
#include "Barrack_Priest.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ABarrack_Priest : public ABarrack
{
	GENERATED_BODY()

protected:
	int32 ClassId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Barrack_Healer.DT_Barrack_Healer"; // 데이터테이블 경로 넣기

public:
	ABarrack_Priest();

	virtual void BeginPlay() override;
	virtual void OpenUI() override;
	virtual void PreInit() override;
	
};
