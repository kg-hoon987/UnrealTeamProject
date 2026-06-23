#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Engine/DataTable.h"
#include "../Stage/StageInfoStruct.h"
#include "MoveMapUI.generated.h"

/**
 * 맵 이동 UI 위젯 클래스
 */
UCLASS()
class TEAMPROJ_API UMoveMapUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UMoveMapUI(const FObjectInitializer& ObjectInitializer);

	// StageID를 받는 생성자 오버로드
	UMoveMapUI(const FObjectInitializer& ObjectInitializer, const FName& InStageID);

protected:
	virtual void NativeConstruct() override;

	// 텍스트 위젯들
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CurrentMapInfo;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HardMapText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NormalMapText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SelectedMapText;

	// 버튼 위젯들
	UPROPERTY(meta = (BindWidget))
	class UButton* HardMapButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* NormalMapButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* OKBtn;

	// 버튼 이벤트 핸들러들
	UFUNCTION()
	void OnHardMapButtonClicked();

	UFUNCTION()
	void OnNormalMapButtonClicked();

	UFUNCTION()
	void OnOKButtonClicked();

	// 현재 선택된 맵 타입
	UPROPERTY(BlueprintReadOnly, Category = "Map Selection")
	FString SelectedMapType;

	// 현재 선택된 맵 타입 (문자열로 관리)
	UPROPERTY(BlueprintReadOnly, Category = "Map Selection")
	FString CurrentSelectedMapType;

	// 데이터테이블 관련
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data Table")
	class UDataTable* StageDataTable;

	UPROPERTY(BlueprintReadOnly, Category = "Stage Data")
	FStageInfoStruct CurrentStageInfo;

	UPROPERTY(BlueprintReadOnly, Category = "Stage Data")
	FName TargetStageID;

	// 데이터테이블에서 스테이지 정보를 가져오는 함수
	UFUNCTION(BlueprintCallable, Category = "Stage Data")
	void LoadStageData(const FName& StageID);

	UFUNCTION(BlueprintCallable, Category = "Stage Data")
	void UpdateStageInfoDisplay();

	// StageID를 설정하는 함수 (액터에서 호출용)
	UFUNCTION(BlueprintCallable, Category = "Stage Data")
	void SetStageID(const FName& InStageID);

	// 데이터테이블을 자동으로 로드하는 함수
	void LoadStageDataTable();

	// 하드 모드 퍼크 활성화 함수
	void ActivateHardModePerk();  //추가3

private:
	// 텍스트 업데이트 함수들
	void UpdateSelectedMapText();
	void UpdateCurrentMapInfo();
	void SetSelectedMapTextColor(const FLinearColor& Color);
};
