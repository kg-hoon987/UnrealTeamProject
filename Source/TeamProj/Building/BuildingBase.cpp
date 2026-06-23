// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingBase.h"
#include "../MyGamePlayerState.h"
#include "BuildingProductionComponent.h"
#include "../Character/GameCharacterBase.h"
#include "Perception/AISense_Sight.h"
#include "BuildingStruct.h"
#include "Kismet/GameplayStatics.h"
#include "../Widget/BuildingHealthBarWidget.h"
#include "../Status/StatusComponent.h"
#include "../Damage/DamageComponent.h"
#include "../Stage/RoundComponent.h"
#include "BuildingStateComponent.h"

// Sets default values
ABuildingBase::ABuildingBase()
{
 	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	SetRootComponent(Root);
	DefaultMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DefaultMesh"));
	DefaultMesh->SetupAttachment(Root);
	DefaultMesh->SetRelativeLocation(FVector(0.f, -220.f, 0.f));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetVisibility(false);

	// 이새끼 안보임..
	Boxcollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollusion"));
	Boxcollision->SetCollisionObjectType(ECC_Pawn);
	Boxcollision->SetupAttachment(Root);
	Boxcollision->SetVisibility(true);
	Boxcollision->SetHiddenInGame(true);
	Boxcollision->SetGenerateOverlapEvents(true);
	Boxcollision->SetCollisionProfileName(TEXT("Trigger")); //오버랩 채널 만들어야할듯.

	StatusComp = CreateDefaultSubobject<UStatusComponent>(TEXT("StatusComp"));
	DamageComp = CreateDefaultSubobject<UDamageComponent>(TEXT("DamageComp"));
	// 건물 기본 : HP / 사망만 사용 ->InitFromBaseStat에 주입

	// 자원 수급용 자식에서 꽂기
	//ProductionComp = CreateDefaultSubobject<UBuildingProductionComponent>(TEXT("ProductionComp"))



	//HDH추가

	HealthScene = CreateDefaultSubobject<USceneComponent>(TEXT("HealthScene"));
	HealthScene->SetupAttachment(RootComponent); 
	HealthScene->SetRelativeLocation(FVector(0.f, -20.f, 250.f));

	HealthWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	HealthWidget->SetupAttachment(HealthScene);
	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidget->SetDrawSize(FVector2D(200.f, 60.f));
	HealthWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HealthWidget->SetVisibility(false);

	static ConstructorHelpers::FClassFinder<UUserWidget>HU(TEXT("/Game/Widget/W_BuildingStatusBar.W_BuildingStatusBar_C"));
	if (HU.Succeeded())
	{
		HealthUIClass = HU.Class;
	}

	Stimuli = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli"));
    Stimuli->bAutoRegister = true; // BeginPlay에서 자동등록
    Stimuli->RegisterForSense(UAISense_Sight::StaticClass());

	Boxcollision->ComponentTags.AddUnique("Building");
    //Tags.AddUnique("Building");
	//HDH추가
}

// Called when the game starts or when spawned
void ABuildingBase::BeginPlay()
{
	Super::BeginPlay();

	if (!bisDoorWall)
	{
		UStaticMesh* MeshAsset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(), nullptr, TEXT("/Game/Building/Mesh/Laboratory/SubBuilding/SM_Laboratory_Workplace.SM_Laboratory_Workplace")));
		if (MeshAsset)
			DefaultMesh->SetStaticMesh(MeshAsset);
	}

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{

			if (!bisDoorWall)
				RoundComp->OnWarStart.AddDynamic(this, &ABuildingBase::SaveStageData);

			RoundComp->OnRoundEnd.AddDynamic(this, &ABuildingBase::OnBuildingDied);
			RoundComp->OnWarStart.AddDynamic(this, &ABuildingBase::WRAP_BuildingEfInterface);
		
		}
		if (UBuildingStateComponent* buildingStateCom = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
		{
			buildingStateCom->OnActivate_Building.AddDynamic(this, &ABuildingBase::WRAP_BuildingEfInterface);
		}
	}

	InitBuilding();
}


const FBuildingLevelData* ABuildingBase::GetRow(int32 InLevel) const
{

	const FName RowName(*FString::FromInt(InLevel));
	const FBuildingLevelData* Row = DataTableAsset->FindRow<FBuildingLevelData>(RowName, TEXT("GetRow"));
	if (!Row)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No Row for Level %d"), *GetName(), InLevel);
	}
	return Row;
}

void ABuildingBase::ApplyRow(const FBuildingLevelData& Row)
{
	//풀피유징ㅇ 
    StatusComp->SetBaseStat(Row.BuildingStat, true);
	
	overlapoffset = Row.ovelapOffset;
	UpdateVisualForLevel(Row.meshRef);
	
	//효과 발동. 컴포넌트에 신호주기?
	ProductionComp->InitBuildingEffect(Row);

	PostApplyRow();
}

void ABuildingBase::UpdateVisualForLevel(const FString& Ref)
{
	if (Ref.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("MeshRef not set"));
		return;
	}
	
	UStaticMesh* MeshAsset = Cast<UStaticMesh>(StaticLoadObject(UStaticMesh::StaticClass(),nullptr,*Ref));

	if (MeshAsset && !bisDoorWallManager)
	{
		Mesh->SetStaticMesh(MeshAsset);
		if (!bIsBuilt && CurrentLevel >= 1)
		{
			if (Stimuli)
			{
				SomethingInBound();
				Mesh->SetVisibility(true);
				Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				Stimuli->RegisterForSense(UAISense_Sight::StaticClass());
				Stimuli->RegisterWithPerceptionSystem();
				DefaultMesh->SetVisibility(false);
			}
		}
		if (CurrentLevel > 0)
		{
			FBoxSphereBounds bound = Mesh->GetStaticMesh()->GetBounds();
			if (!bisDoorWall)
				Boxcollision->SetWorldLocation(bound.Origin + GetActorLocation());
			else
				Boxcollision->SetWorldLocation(GetActorLocation() + (bound.BoxExtent.X * GetActorForwardVector()) + (bound.BoxExtent.Z * GetActorUpVector()));
			Boxcollision->SetBoxExtent(bound.BoxExtent + FVector(overlapoffset, overlapoffset, 0.f));
		}
		else
			Boxcollision->SetBoxExtent(Boxcollision->GetScaledBoxExtent() + FVector(overlapoffset, overlapoffset, 0.f));
	}
}

void ABuildingBase::OnReceiveAnyDamage(float Amount)
{
	//StatusComp->TakeDamage(Amount);
	DamageComp->CalculateDamage(this,Amount);
}

void ABuildingBase::OnBuildingDied() // isDead로 변경 해야함
//지금 껀 아마 다른애가 죽었을때 효과 제어하려고 한듯.
{
	bIsDestroyed = true;

	// 충돌/타겟팅 비활성화
	SetActorEnableCollision(false);

	if (bisDoorWallManager)
		Destroy();

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			if (RoundComp->RoundState == ERoundState::WarStart && !bisDoorWallManager)
				ProductionComp->OnExplosion();
		}
	}

	Destroy();
}

void ABuildingBase::SetDataTable(FString path)
{
	if (!path.IsEmpty())
	{
		UObject* dt = StaticLoadObject(UDataTable::StaticClass(), nullptr, *path);
		if(dt)
			DataTableAsset = Cast<UDataTable>(dt);
		else
			UE_LOG(LogTemp, Warning, TEXT("DataTable Path is Wrong : [%s]"), *path);
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("DataTable Path is Empty : [%s]"), *path);
}

void ABuildingBase::OnInteraction()
{
	// 건물 상호작용 -> 플레이어 한테 던지기
	/*플레이어가 오버랩한 건물을 인식하고 플레이어가 신호를 건물에게 주는 방식으로 해결*/
	//플레이어가 오버랩하면 이거 부르면 될듯.
	// UI에 데이터 정보 박아주기.

}


void ABuildingBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (Pawn)
	{
		if (URoundComponent* RoundComp = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->RoundCom)
		{
			if (!bisDoorWall)
				RoundComp->OnWarStart.RemoveDynamic(this, &ABuildingBase::SaveStageData);
			RoundComp->OnWarStart.RemoveDynamic(this, &ABuildingBase::WRAP_BuildingEfInterface);
			RoundComp->OnRoundEnd.RemoveDynamic(this, &ABuildingBase::OnBuildingDied);
			
		}
		if (UBuildingStateComponent* buildingStateCom = Cast<AMyGamePlayerState>(Pawn->GetPlayerState())->BuildingCom)
		{
			buildingStateCom->OnActivate_Building.RemoveDynamic(this, &ABuildingBase::WRAP_BuildingEfInterface);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABuildingBase::SomethingInBound()
{
	TArray<FOverlapResult> Overlaps;

	// 쿼리 파라미터 (자기 자신 제외)
	FCollisionQueryParams Params(NAME_None, false, this);

	// 박스 크기
	FBoxSphereBounds bound = Mesh->GetStaticMesh()->GetBounds();
	FVector BoxExtent(bound.BoxExtent);
	FCollisionObjectQueryParams ObjectQuery;
	ObjectQuery.AddObjectTypesToQuery(ECC_Pawn);

	bool bHasOverlap = GetWorld()->OverlapMultiByObjectType(
		Overlaps,
		GetActorLocation(),
		FQuat::Identity,               // 회전
		ObjectQuery,
		FCollisionShape::MakeBox(BoxExtent),
		Params
	);
	

	TArray<AActor*> CoveredActors;
	AActor* Ac = nullptr;
	if (bHasOverlap)
	{
		for (auto& Result : Overlaps)
		{
			if (AActor* Actor = Result.GetActor())
			{
				if (Actor->IsA(AGameCharacterBase::StaticClass()))
				{
					CoveredActors.AddUnique(Actor);
					
				}
			}
		}
	}

	if (CoveredActors.Num() > 0)
	{
		
		for (AActor* charcter : CoveredActors)
		{
			charcter->SetActorLocation(bound.Origin + GetActorLocation() - FVector(bound.BoxExtent.X + overlapoffset - 0.5f, 0.f, 0.f));
		}
	}
}

void ABuildingBase::WRAP_BuildingEfInterface()
{
	InitMyGlEf(this, this, Team);
}



void ABuildingBase::InitBuilding()
{
	PreInit();		// 상속받은 애들에서 구현
	const FBuildingLevelData* Row = GetRow(CurrentLevel);
	if (Row)
	{
		StatusComp->InitFromBaseStat(Row->BuildingStat, EStatusOwnerType::Building, true);
		ApplyRow(*Row);
		if (HealthUIClass)
		{
			HealthUI = CreateWidget<UUserWidget>(GetWorld(), HealthUIClass);
			if (HealthUI)
			{
				if (UBuildingHealthBarWidget* HW = Cast<UBuildingHealthBarWidget>(HealthUI))
				{
					HW->SetBuilding(this);
				}
				HealthWidget->SetWidget(HealthUI);
				//HealthUI->AddToViewport();

			}
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelData is None!"));
	}
	PostInit();		// 상속받은 애들에서 구현.
}

void ABuildingBase::OpenUI()
{
	//UI 띄우기 이거 병영이랑 상점 같은 경우에는 다르기 때문에 고민해봐야할듯.
}

bool ABuildingBase::LevelUp()
{
	// 레벨업은 위젯에서 호출하는 것으로.
	

	const FBuildingLevelData* Next = GetRow(CurrentLevel + 1);
	if (!Next)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] No next level from %d"), *GetName(), CurrentLevel);
		return false;
	}

	CurrentLevel = Next->Level;

	// 베이스 스탯 교체(HP 비율 유지)
	StatusComp->SetBaseStat(Next->BuildingStat,true);
	StatusComp->FullHPUpdate();

	
	// 새 레벨 효과 재등록
	ApplyRow(*Next);
	HealthWidget->SetVisibility(true);
	Boxcollision->SetCollisionProfileName(TEXT("Trigger"));
	return true;
}

void ABuildingBase::Preview(bool isPressKey)
{
	if (CurrentLevel > 1) return;

	Mesh->SetVisibility(isPressKey);
}

void ABuildingBase::ImportStateData(int32 lv)
{
	CurrentLevel = lv;
	if (lv > 0)
	{
		bIsBuilt = true;
		if (!bisDoorWallManager)
		{
			SomethingInBound();
			Mesh->SetVisibility(true);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		}
		
	}

	InitBuilding();
}

void ABuildingBase::SaveStageData()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	AMyGamePlayerState* Ps = Cast<AMyGamePlayerState>(Pawn->GetPlayerState());
	UBuildingStateComponent* BsCompo = Ps->BuildingCom;

	FBuildingStateInfo myInfo;
	myInfo.BuildingClass = GetClass();
	myInfo.Level = CurrentLevel;
	myInfo.Location = GetActorLocation();
	myInfo.Rotation = GetActorRotation();

	PostSave(&myInfo);

	BsCompo->AllBuildingStateInfo.Add(myInfo);
}



