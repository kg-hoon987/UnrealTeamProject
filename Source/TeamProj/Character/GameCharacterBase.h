// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Status/StatusStruct.h"
#include "../Skill/SkillStruct.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "../Building/BuildingStateComponent.h"
#include "../AICon/GameUnitBase.h"
#include "../Interface/WidgetInterface.h"
#include "../Interface/BuildingEffectInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameCharacterBase.generated.h"

class UStatusComponent;
class USkillComponent;
class UDamageComponent;
class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class ABuildingBase;

UENUM()
enum class ECameraMove : uint8
{
	Up,
	Down,
	Left,
	Right
};

UCLASS()
class TEAMPROJ_API AGameCharacterBase : public ACharacter,  public IGenericTeamAgentInterface, public IBuildingEffectInterface, public ILaboratoryTickHealInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* WeaponRight;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<USkeletalMeshComponent*> Parts;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* HealthScene;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* HealthWidget;

	


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkillComponent* SkillComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UDamageComponent* DamageComp;
public:
	AGameCharacterBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStatusComponent* StatusComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FGLBuildingEF> MyGlEf;

	UFUNCTION()
	virtual void  LaboratoryTickHealEf(bool onTickHeal, int32 Heal) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	bool isTickHeal = false;
	UPROPERTY()
	int32 TickHeal{};
	float HealTickTime{};

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void DeadTeamFilter() { Team = ETeam::NoTeam; GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));  }
	
	void AttachWeapon(FClassConfig pStat, UStaticMesh* pWeaponLeft, UStaticMesh* pWeaponRight) const;

	void AttachParts(USkeletalMesh* pChest, TArray<USkeletalMesh*> Parts) const;

	UFUNCTION()
	void WRAP_BuildingEfInterface();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "e-InitCharcter"))
	virtual void InitCharInfo(EClassType ClassType, FClassConfig StatID, TArray<FSkillConfig> SkillIDs, int32 SkillLevel, 
		USkeletalMesh* pChest, TArray<USkeletalMesh*> pParts, UStaticMesh* pWeaponLeft, UStaticMesh* pWeaponRight, TSubclassOf<UAnimInstance> pAnimInstance, FVector CharScale);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
	ETeam Team = ETeam::Player;

	virtual FGenericTeamId GetGenericTeamId() const override
	{
		return FGenericTeamId(static_cast<uint8>(Team));
	}

	

	UFUNCTION()
	void SetDied();

	//Niagara
public:
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* HealNiagara{};
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* HitNiagara{};
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* FireNiagara{};
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* BloodNiagara{};
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PoisonNiagara{};
	UPROPERTY(EditAnywhere)
	UNiagaraSystem* IcedNiagara{};


};



UCLASS()
class TEAMPROJ_API APlayerCharacter : public AGameCharacterBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* Dragon;
	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm{};
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera{};

	//줌인 + 자유시점ㅇㅇ
	float DesiredArmLength;
	UPROPERTY(EditAnywhere, Category = "Zoom")
	float MinArmLength;
	UPROPERTY(EditAnywhere, Category = "Zoom")
	float MaxArmLength;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	class USphereComponent* OverlapSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* CurrencyColl;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USphereComponent* DefaultAttackRadius;

	bool isOverppedBuilding;
	ABuildingBase* BuildingInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UItemComponent* ItemComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* HoldTabWidgetScene;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* HoldTabWidget;

	//엣지스크롤 화면 가생이에 마우스가면 이도오오오오옹
protected:
	void SetAliveState(bool bAlive);
	bool DeadStart = false;
	float RespawnDelay = 10.f;
	
public:
	void DiedPlayer();
	void GenPlayer();


public:
	APlayerCharacter();
	ABuildingBase* GetBuilding() { return BuildingInfo; }

	UFUNCTION()
	void SetHoldTabWidget(UUserWidget* Widget);
	UFUNCTION()
	void UnLookHoldTabWidget() { HoldTabWidget->SetVisibility(false); }
	UFUNCTION()
	void LookHoldTabWidget() { HoldTabWidget->SetVisibility(true); }


protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnDefaultAttackOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnDefaultAttackOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	virtual void Tick(float DeltaTime) override;

	virtual void InitCharInfo(EClassType ClassType, FClassConfig StatID, TArray<FSkillConfig> SkillIDs, int32 SkillLevel,
		USkeletalMesh* pChest, TArray<USkeletalMesh*> pParts, UStaticMesh* pWeaponLeft, UStaticMesh* pWeaponRight, TSubclassOf<UAnimInstance> pAnimInstance, FVector CharScale) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//카메라 이동 + 줌인
	bool IsDetach;
	void DetachCamera();
	void AttachCamera();
	void ZoomInCamera(float v);

	//스크린 엣지 스크롤
	float CameraSpeed;
	void DirectionCameraMove(float DeltaTime);
	void CameraMove(ECameraMove CameraWay,float DeltaTime);

	FVector Min;
	FVector Max;


protected:
	bool bDefaultAttack= false;

	UFUNCTION()
	void SettingDefaultAttack();

};



UCLASS()
class TEAMPROJ_API AEnemyCharacter : public AGameCharacterBase
{
	GENERATED_BODY()

	protected:

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	// 지연된 퍼크 적용을 위한 타이머
	FTimerHandle DelayedPerkApplicationTimer;

	// 퍼크 적용 함수들
	void ApplyPerksToNewEnemy();
	void ApplyPerksAfterInit();
};


UCLASS()
class TEAMPROJ_API AAllyCharacter : public AGameCharacterBase, public IWidgetInterface
{
	GENERATED_BODY()

	protected:

public:
	AAllyCharacter();
	int BarrackUnitID;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void SelectUnit_Implementation();
	virtual void DeSelectUnit_Implementation();
	virtual void Move_Implementation(FVector2D movePos);

private:
	// 지연된 퍼크 적용을 위한 타이머
	FTimerHandle DelayedPerkApplicationTimer;

	// 퍼크 적용 함수들
	void ApplyPerksToNewAlly();
	void ApplyPerksAfterInit();
};



UCLASS()
class TEAMPROJ_API ABossCharacter : public AGameCharacterBase
{
	GENERATED_BODY()

	protected:

public:
	ABossCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};