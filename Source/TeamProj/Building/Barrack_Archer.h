// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Barrack.h"
#include "Barrack_Archer.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ABarrack_Archer : public ABarrack
{
	GENERATED_BODY()
	
protected:
	int32 ClassId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Barrack_Archer.DT_Barrack_Archer"; // 데이터테이블 경로 넣기

public:
	ABarrack_Archer();

	virtual void BeginPlay() override;
	virtual void OpenUI() override;
	virtual void PreInit() override;
};
