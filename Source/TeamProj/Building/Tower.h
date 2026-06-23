#pragma once

#include "CoreMinimal.h"
#include "BuildingBase.h"
#include "Tower.generated.h"

class USphereComponent;
/**
 *
 */
UCLASS()
class TEAMPROJ_API ATower : public ABuildingBase
{
	GENERATED_BODY()

	public:
	ATower();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereCom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* RotScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* ProjectileScene;

	void InitRadius(float radius);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DataTable Path")
	FString Path = "/Game/Data/Building/DT_Tower.DT_Tower";

	virtual void BeginPlay() override;

	virtual void PreInit() override;
	virtual void PostInit() override;

	AActor* FilteringEnemy(AActor* act);

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void OpenUI() override;
};