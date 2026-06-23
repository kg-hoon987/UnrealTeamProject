 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NiagaraSystem.h"
#include "BuildingStruct.h"
#include "BuildingStateComponent.generated.h"

USTRUCT(BlueprintType)
struct FGLBuildingEF
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite) FName SourceBuilding = "";
	UPROPERTY(EditAnywhere, BlueprintReadWrite) EBuildingEffectType Type = EBuildingEffectType::None;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) TArray<ETeam> targetTeams{};
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool hasStatEf = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) FStatModifier StatEf {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite) bool OnOffEf = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int32 Damage {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float Radius{};
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivate_Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivate_Ally);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivate_Enemy);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActivate_Building);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEAMPROJ_API UBuildingStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildingStateComponent();

	UPROPERTY(BlueprintReadOnly)
	AActor* MainCastle{};

	UPROPERTY(BlueprintReadOnly)
	TArray<AActor*> Buildings{};

	UPROPERTY(BlueprintAssignable) FOnActivate_Player OnActivate_Player;
	UPROPERTY(BlueprintAssignable) FOnActivate_Ally OnActivate_Ally;
	UPROPERTY(BlueprintAssignable) FOnActivate_Enemy OnActivate_Enemy;
	UPROPERTY(BlueprintAssignable) FOnActivate_Building OnActivate_Building;

	//add:각건물, 스폰:나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FBuildingStateInfo> AllBuildingStateInfo;

	//효과등록,제거:각컴포, 효과발동:나 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FGLBuildingEF> GlobalBuildingEf;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FGLBuildingEF> GlobalBuildingEf_Player;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FGLBuildingEF> GlobalBuildingEf_Ally;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FGLBuildingEF> GlobalBuildingEf_Building;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) TArray<FGLBuildingEF> GlobalBuildingEf_Enemy;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 GLEfNum;

	void RegisterGlobalEf(FGLBuildingEF ef);

	void UnRegisterGlobalEf(FGLBuildingEF ef, FName actor);

	TArray<FGLBuildingEF> GetGlobalEfList(ETeam team);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	TArray<UNiagaraSystem*> ExplosionFXs;   // 에디터에서 세팅 or DataAsset 경유

	TArray<UNiagaraSystem*> GetExplosionFX() const { return ExplosionFXs; }

private:
	TMap<ETeam, TArray<FGLBuildingEF>*> TeamMap = {
				{ETeam::Player,   &GlobalBuildingEf_Player},
				{ETeam::Ally,     &GlobalBuildingEf_Ally},
				{ETeam::Building, &GlobalBuildingEf_Building},
				{ETeam::Enemy,    &GlobalBuildingEf_Enemy}
	};

	//이거 라운드 종료에 바인드 해야함.
	UFUNCTION()
	void SpawnBuldings();

	void GlobalEfBroadcast(ETeam team);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
