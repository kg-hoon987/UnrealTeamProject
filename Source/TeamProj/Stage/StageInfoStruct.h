#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "StageInfoStruct.generated.h"

/**
 * 스테이지 정보를 담는 구조체
 * 데이터테이블에서 사용됩니다.
 */
USTRUCT(BlueprintType)
struct TEAMPROJ_API FStageInfoStruct : public FTableRowBase
{
	GENERATED_BODY()

	// 기본 생성자
	FStageInfoStruct()
	{
		StageID = NAME_None;
		StageName = TEXT("");
		LevelAsset = nullptr;
		MapDescription = TEXT("");
		NormalDescription = TEXT("");
		HardDescription = TEXT("");
	}

	// 스테이지 고유 ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Info")
	FName StageID;

	// 스테이지 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Info")
	FString StageName;

	// 레벨 에셋
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Info")
	TSoftObjectPtr<UWorld> LevelAsset;

	// 맵 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Info")
	FString MapDescription;

	// 일반 난이도 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Info")
	FString NormalDescription;

	// 하드 난이도 설명
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Info")
	FString HardDescription;

	// 구조체 비교 연산자 오버로드
	bool operator==(const FStageInfoStruct& Other) const
	{
		return StageID == Other.StageID;
	}

	// 구조체 해시 함수
	friend uint32 GetTypeHash(const FStageInfoStruct& Struct)
	{
		return GetTypeHash(Struct.StageID);
	}
};
