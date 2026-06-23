// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "BuildingStruct.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "../Interface/BuildingEffectInterface.h"
#include "BuildingStateComponent.h"
#include "BuildingBase.generated.h"

class UStatusComponent;
class UDamageComponent;
//플레이스테이트
class AMyGamePlayerState;
class UBuildingProductionComponent;
struct FBuildingLevelData;

UCLASS(Abstract)
class TEAMPROJ_API ABuildingBase : public AActor, public IBuildingEffectInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ABuildingBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* DefaultMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* Boxcollision;



	// 에디터에서 지정(Soft 권장). Soft 없으면 UDataTable*로 두고 그대로 사용해도 OK.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TSoftObjectPtr<UDataTable> DataTableAsset;

	// 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentLevel = 0;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDestroyed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float overlapoffset;

	//HDH 추가코드=======================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDamageComponent* DamageComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> BuildingUIClass{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UUserWidget> BuildingUI{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> HealthUIClass{};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UUserWidget> HealthUI{};

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* HealthScene;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UWidgetComponent* HealthWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bisDoorWall = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bisDoorWallManager = false;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsBuilt = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatusComponent* StatusComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBuildingProductionComponent* ProductionComp; // 건물효과담당

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FGLBuildingEF> MyGlEf;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAIPerceptionStimuliSourceComponent* Stimuli;

	TObjectPtr<UUserWidget> GetBuildingUI() { return BuildingUI; }

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentLevel() { return CurrentLevel; }

	UFUNCTION(BlueprintCallable)
	FBuildingLevelData GetRow_BP(int32 InLevel) const
	{
		if (auto* Row = GetRow(InLevel))
			return *Row;
		return FBuildingLevelData();
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	ETeam Team = ETeam::Building;

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		return FGenericTeamId(static_cast<uint8>(Team));
	}

	virtual void SetGenericTeamId(const FGenericTeamId& InTeamID) override
	{
		Team = static_cast<ETeam>(InTeamID.GetId());
	}

	UFUNCTION()
	virtual void OnBuildingDied();

	//HDH 추가코드=======================================

protected:
	virtual void BeginPlay() override;

	const FBuildingLevelData* GetRow(int32 InLevel) const;


	UFUNCTION()
	void UpdateVisualForLevel(const FString& Ref);

	
	UFUNCTION()
	void SetDataTable(FString path);

	UFUNCTION()
	void OnInteraction();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 아래에 뭐 있는 지 확인.
	void SomethingInBound();
	UFUNCTION()
	void WRAP_BuildingEfInterface();

public:

	virtual void ApplyRow(const FBuildingLevelData& Row);

	UFUNCTION(BlueprintCallable)
	void OnReceiveAnyDamage(float Amount);
	
	UFUNCTION()
	virtual void PostApplyRow() {};

	UFUNCTION()
	virtual void PreInit() {};

	UFUNCTION()
	virtual void PostInit() {};

	UFUNCTION(BlueprintCallable)
	void InitBuilding();

	UFUNCTION(BlueprintCallable)
	virtual void OpenUI();

	UFUNCTION(BlueprintCallable)
	bool LevelUp(); // 비용 체크는 외부(경제 시스템)

	UFUNCTION(BlueprintCallable)
	void Preview(bool isPressKey);

	UFUNCTION(BlueprintCallable)
	void ImportStateData(int32 lv); // 빌딩 스테이트가 사용.(데이터 넘겨주기.)

	UFUNCTION(BlueprintCallable)
	void SaveStageData();

	virtual void PostSave(FBuildingStateInfo* Info) {};
};
