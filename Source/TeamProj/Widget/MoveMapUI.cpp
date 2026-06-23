#include "MoveMapUI.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "Engine/DataTable.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../MyGameInstance.h"
#include "Engine/AssetManager.h"
#include "UObject/ConstructorHelpers.h"
#include "../MyGamePlayerState.h"  //추가3
#include "../Perk/PerkComponent.h"  //추가3

UMoveMapUI::UMoveMapUI(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 초기값 설정
	SelectedMapType = TEXT("None");
	CurrentSelectedMapType = TEXT("None");
	TargetStageID = NAME_None;
	StageDataTable = nullptr;
}

UMoveMapUI::UMoveMapUI(const FObjectInitializer& ObjectInitializer, const FName& InStageID)
	: Super(ObjectInitializer)
{
	// 초기값 설정
	SelectedMapType = TEXT("None");
	CurrentSelectedMapType = TEXT("None");
	TargetStageID = InStageID;
	StageDataTable = nullptr;
}

void UMoveMapUI::NativeConstruct()
{
	Super::NativeConstruct();

	// 버튼 이벤트 바인딩
	if (HardMapButton)
	{
		HardMapButton->OnClicked.AddDynamic(this, &UMoveMapUI::OnHardMapButtonClicked);
	}

	if (NormalMapButton)
	{
		NormalMapButton->OnClicked.AddDynamic(this, &UMoveMapUI::OnNormalMapButtonClicked);
	}

	if (OKBtn)
	{
		OKBtn->OnClicked.AddDynamic(this, &UMoveMapUI::OnOKButtonClicked);
	}

	// 초기 텍스트 설정
	UpdateCurrentMapInfo();
	UpdateSelectedMapText();

	// 초기 텍스트 설정은 UpdateStageInfoDisplay에서 처리

	// 데이터테이블 자동 로드
	LoadStageDataTable();

	// StageID가 설정되어 있으면 자동으로 데이터 로드
	if (TargetStageID != NAME_None)
	{
		LoadStageData(TargetStageID);
	}
}

void UMoveMapUI::OnHardMapButtonClicked()
{
	CurrentSelectedMapType = TEXT("Hard");
	SelectedMapType = TEXT("Hard");
	
	// SelectedMapText를 빨간색으로 설정하고 "하드" 텍스트 표시
	if (SelectedMapText)
	{
		SelectedMapText->SetText(FText::FromString(TEXT("하드")));
		SetSelectedMapTextColor(FLinearColor::Red);
	}
}

void UMoveMapUI::OnNormalMapButtonClicked()
{
	CurrentSelectedMapType = TEXT("Normal");
	SelectedMapType = TEXT("Normal");
	
	// SelectedMapText를 노란색으로 설정하고 "노멀" 텍스트 표시
	if (SelectedMapText)
	{
		SelectedMapText->SetText(FText::FromString(TEXT("노멀")));
		SetSelectedMapTextColor(FLinearColor::Yellow);
	}
}

void UMoveMapUI::OnOKButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("OKBtn 클릭됨"));
	
	if (CurrentSelectedMapType == TEXT("None"))
	{
		UE_LOG(LogTemp, Warning, TEXT("맵 타입이 선택되지 않음"));
		return;
	}

	// LevelAsset 유효성 검사 및 강제 로드 시도
	if (!CurrentStageInfo.LevelAsset.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelAsset이 유효하지 않음 - 강제 로드 시도"));
		
		// 레벨 에셋 강제 로드 시도
		CurrentStageInfo.LevelAsset.LoadSynchronous();
		
		if (!CurrentStageInfo.LevelAsset.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("레벨 에셋 로드 실패"));
			return;
		}
		
		UE_LOG(LogTemp, Log, TEXT("레벨 에셋 로드 성공"));
	}

	// 하드 난이도 선택 시 EnemyStat 퍼크 활성화
	if (CurrentSelectedMapType == TEXT("Hard"))  //추가3
	{
		ActivateHardModePerk();
	}

	// 레벨 이동 실행
	TSoftObjectPtr<UWorld> LevelAsset = CurrentStageInfo.LevelAsset;
	FString LevelName = LevelAsset.GetAssetName();
	UE_LOG(LogTemp, Log, TEXT("레벨 이동 시도: %s"), *LevelName);

	// MyGameInstance의 OpenLevel 함수 사용
	UMyGameInstance* GameInstance = Cast<UMyGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		UE_LOG(LogTemp, Log, TEXT("MyGameInstance를 통한 레벨 이동"));
		GameInstance->OpenLevel(LevelAsset);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("직접 레벨 이동"));
		UGameplayStatics::OpenLevel(this, FName(*LevelName));
	}
}

void UMoveMapUI::UpdateSelectedMapText()
{
	if (SelectedMapText)
	{
		FString DisplayText;
		if (CurrentSelectedMapType == TEXT("Normal"))
		{
			DisplayText = TEXT("선택된 맵: 일반 맵");
		}
		else if (CurrentSelectedMapType == TEXT("Hard"))
		{
			DisplayText = TEXT("선택된 맵: 하드 맵");
		}
		else
		{
			DisplayText = TEXT("선택된 맵: 없음");
		}
		SelectedMapText->SetText(FText::FromString(DisplayText));
	}
}

void UMoveMapUI::UpdateCurrentMapInfo()
{
	if (CurrentMapInfo)
	{
		CurrentMapInfo->SetText(FText::FromString(TEXT("현재 맵 정보: 메인 맵")));
	}
}

void UMoveMapUI::SetSelectedMapTextColor(const FLinearColor& Color)
{
	if (SelectedMapText)
	{
		SelectedMapText->SetColorAndOpacity(Color);
	}
}

void UMoveMapUI::LoadStageData(const FName& StageID)
{
	UE_LOG(LogTemp, Log, TEXT("LoadStageData 호출: %s"), *StageID.ToString());
	
	if (!StageDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("StageDataTable이 null입니다"));
		return;
	}

	// 데이터테이블에서 스테이지 정보를 찾습니다
	FStageInfoStruct* FoundStage = StageDataTable->FindRow<FStageInfoStruct>(StageID, TEXT(""));
	
	if (FoundStage)
	{
		CurrentStageInfo = *FoundStage;
		UE_LOG(LogTemp, Log, TEXT("스테이지 데이터 로드 성공: %s"), *CurrentStageInfo.StageName);
		UE_LOG(LogTemp, Log, TEXT("LevelAsset 유효성: %s"), CurrentStageInfo.LevelAsset.IsValid() ? TEXT("유효") : TEXT("무효"));
		UpdateStageInfoDisplay();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("StageID '%s'를 찾을 수 없습니다"), *StageID.ToString());
	}
}

void UMoveMapUI::UpdateStageInfoDisplay()
{
	// CurrentMapInfo에 스테이지 이름과 맵 설명 표시
	if (CurrentMapInfo)
	{
		FString DisplayText = FString::Printf(TEXT("%s\n%s"), 
			*CurrentStageInfo.StageName,
			*CurrentStageInfo.MapDescription);
		CurrentMapInfo->SetText(FText::FromString(DisplayText));
	}

	// NormalMapText에 NormalDescription 표시
	if (NormalMapText)
	{
		NormalMapText->SetText(FText::FromString(CurrentStageInfo.NormalDescription));
	}

	// HardMapText에 HardDescription 표시
	if (HardMapText)
	{
		HardMapText->SetText(FText::FromString(CurrentStageInfo.HardDescription));
	}

	// 선택된 난이도에 따라 SelectedMapText 업데이트
	if (SelectedMapText)
	{
		FString Description;
		FLinearColor TextColor;
		
		if (CurrentSelectedMapType == TEXT("Normal"))
		{
			Description = CurrentStageInfo.NormalDescription;
			TextColor = FLinearColor::Yellow;
		}
		else if (CurrentSelectedMapType == TEXT("Hard"))
		{
			Description = CurrentStageInfo.HardDescription;
			TextColor = FLinearColor::Red;
		}
		else
		{
			Description = TEXT("난이도 선택");
			TextColor = FLinearColor::White;
		}
		
		SelectedMapText->SetText(FText::FromString(Description));
		SetSelectedMapTextColor(TextColor);
	}
}

void UMoveMapUI::SetStageID(const FName& InStageID)
{
	TargetStageID = InStageID;
	
	// StageID가 설정되면 즉시 데이터 로드
	if (TargetStageID != NAME_None)
	{
		LoadStageData(TargetStageID);
	}
}

void UMoveMapUI::LoadStageDataTable()  //수정3 통째로 변동
{
	FString DataTablePath = TEXT("/Game/Data/Stage/DT_StageInfo.DT_StageInfo");
	
	UDataTable* LoadedDataTable = LoadObject<UDataTable>(nullptr, *DataTablePath);
	
	if (!LoadedDataTable)
	{
		FSoftObjectPath SoftPath(DataTablePath);
		LoadedDataTable = Cast<UDataTable>(SoftPath.TryLoad());
	}
	
	if (LoadedDataTable)
	{
		StageDataTable = LoadedDataTable;
		UE_LOG(LogTemp, Log, TEXT("Stage DataTable 로드 성공: %s"), *DataTablePath);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Stage DataTable 로드 실패: %s"), *DataTablePath);
	}
}

void UMoveMapUI::ActivateHardModePerk()  //추가3 통쨰로 변동
{
	UE_LOG(LogTemp, Log, TEXT("하드 모드 퍼크 활성화 시도"));
	
	// 플레이어 상태 가져오기
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerPawn을 찾을 수 없습니다"));
		return;
	}
	
	// MyGamePlayerState 가져오기
	AMyGamePlayerState* PlayerState = Cast<AMyGamePlayerState>(PlayerPawn->GetPlayerState());
	if (!PlayerState)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyGamePlayerState를 찾을 수 없습니다"));
		return;
	}
	
	// PerkComponent 가져오기
	if (!PlayerState->PerkComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("PerkComponent를 찾을 수 없습니다"));
		return;
	}
	
	// EnemyStat 퍼크 활성화
	PlayerState->PerkComp->ActivatePerkByID(FName("EnemyStat"));
	UE_LOG(LogTemp, Log, TEXT("EnemyStat 퍼크 활성화 완료"));
}