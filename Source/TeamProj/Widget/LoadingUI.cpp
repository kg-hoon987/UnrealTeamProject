// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingUI.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "../MyGameInstance.h"

void ULoadingUI::OnLevelLoaded()
{
	targetPercent = 1.f;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("OnLevelLoaded() Called!"));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Setting targetPercent to 1.0"));
}

void ULoadingUI::NativeConstruct()
{
	Super::NativeConstruct();
	Bar->SetPercent(0.f);
	PercentText->SetText(FText::FromString(TEXT("(0%)")));
	LoadingText->SetText(FText::FromString(TEXT("Loading")));
	DotTimer = 0.f;
	BorderTimer = 0.f;
	
	// 보더 초기 opacity 설정
	if (Border1) Border1->SetRenderOpacity(0.2f);
	if (Border2) Border2->SetRenderOpacity(0.2f);
	if (Border3) Border3->SetRenderOpacity(0.2f);
	if (Border4) Border4->SetRenderOpacity(0.2f);
	
	//디버깅: 레벨명 확인
	FString LevelName = GameInst->GetTargetLevelName().ToString();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, 
		FString::Printf(TEXT("Loading Level: %s"), *LevelName));
	
	//비동기 로딩
	FLoadPackageAsyncDelegate LoadedDelegate;
	LoadedDelegate.BindUFunction(this, FName("OnLevelLoaded"));
	LoadPackageAsync(LevelName, LoadedDelegate);
	targetPercent = 0.8f;  // 80%까지 증가 (뒤에서 비동기 돌아가는거 확인해쓰 -> 비동기로딩 먼저 쏘고 게이지 올라가는거임))
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Async Loading Started"));
}

void ULoadingUI::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);
	if (curPercent < 1.f)
	{
		// 2초에 걸쳐서 80%까지 Clamp로 부드럽게 ㄱㄱ
		float ProgressSpeed = 0.8f / 2.0f;
		curPercent = FMath::Clamp(curPercent + DeltaTime * ProgressSpeed, 0.f, targetPercent);
		Bar->SetPercent(curPercent);
		
		// PercentText 업뎃하고
		int32 PercentValue = FMath::RoundToInt(curPercent * 100);
		FString PercentString = FString::Printf(TEXT("(%d%%)"), PercentValue);
		PercentText->SetText(FText::FromString(PercentString));
		
		// 0.3초 마다 Loading . . . 점 깜빡
		DotTimer += DeltaTime;
		if (DotTimer >= 0.3f)
		{
			DotTimer = 0.f;
			
			static int32 DotCount = 0;
			DotCount = (DotCount + 1) % 4;  // 0, 1, 2, 3 순환
			
			FString Dots = TEXT("");
			for (int32 i = 0; i < DotCount; i++)
			{
				Dots += TEXT(". ");
			}
			
			FString LoadingString = FString::Printf(TEXT("Loading %s"), *Dots);
			LoadingText->SetText(FText::FromString(LoadingString));
		}
		
		// 0.3초 마다 보더 회전 ㄱ
		BorderTimer += DeltaTime;
		if (BorderTimer >= 0.3f)
		{
			BorderTimer = 0.f;
			
			static int32 BrightBorderIndex = 0;
			int32 PreviousBorderIndex = (BrightBorderIndex - 1 + 4) % 4;  // 이전 보더 인덱스
			BrightBorderIndex = (BrightBorderIndex + 1) % 4;
			
			if (Border1) Border1->SetRenderOpacity(0.2f);
			if (Border2) Border2->SetRenderOpacity(0.2f);
			if (Border3) Border3->SetRenderOpacity(0.2f);
			if (Border4) Border4->SetRenderOpacity(0.2f);
			
			// 이전 보더를 중간 밝기로 설정
			switch (PreviousBorderIndex)
			{
			case 0:  // 좌상
				if (Border1) Border2->SetRenderOpacity(0.6f);
				break;
			case 1:  // 우상
				if (Border2) Border3->SetRenderOpacity(0.6f);
				break;
			case 2:  // 우하
				if (Border3) Border4->SetRenderOpacity(0.6f);
				break;
			case 3:  // 좌하
				if (Border4) Border1->SetRenderOpacity(0.6f);
				break;
			}
			
			// 현재 밝은 보더만 밝게 설정 (시계방향)
			switch (BrightBorderIndex)
			{
			case 0:  // 좌상
				if (Border1) Border1->SetRenderOpacity(1.0f);
				break;
			case 1:  // 우상
				if (Border2) Border2->SetRenderOpacity(1.0f);
				break;
			case 2:  // 우하
				if (Border3) Border3->SetRenderOpacity(1.0f);
				break;
			case 3:  // 좌하
				if (Border4) Border4->SetRenderOpacity(1.0f);
				break;
			}
		}
		
		static float LastDebugTime = 0.f;
		if (GetWorld()->GetTimeSeconds() - LastDebugTime > 1.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::White, 
				FString::Printf(TEXT("Progress: %.1f%% (Target: %.1f%%)"), curPercent * 100, targetPercent * 100));
			LastDebugTime = GetWorld()->GetTimeSeconds();
		}
		
		static float StartTime = GetWorld()->GetTimeSeconds();
		if (GetWorld()->GetTimeSeconds() - StartTime > 5.f && targetPercent < 1.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("Force Complete After 5 Seconds"));
			targetPercent = 1.f;
		}
		
		if (FMath::IsNearlyEqual(curPercent, 1.f))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Opening Target Level!"));
			UGameplayStatics::OpenLevel(this, GameInst->GetTargetLevelName());
		}
	}
}

bool ULoadingUI::Initialize()
{
	if (!Super::Initialize()) return false;	

	return true;
}

