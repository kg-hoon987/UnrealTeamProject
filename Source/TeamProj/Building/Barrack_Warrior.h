// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Barrack.h"
#include "Barrack_Warrior.generated.h"

/**
 * 
 */
UCLASS()
class TEAMPROJ_API ABarrack_Warrior : public ABarrack
{
	GENERATED_BODY()
	
protected:
	int32 ClassId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Barrack_Warrior.DT_Barrack_Warrior"; // 데이터테이블 경로 넣기

public:
	ABarrack_Warrior();

	virtual void BeginPlay() override;
	virtual void OpenUI() override;
	virtual void PreInit() override;
};
