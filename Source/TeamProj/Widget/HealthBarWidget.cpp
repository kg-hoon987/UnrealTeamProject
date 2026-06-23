// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"
#include "../Character/GameCharacterBase.h"
#include "../Damage/DamageComponent.h"
#include "../Status/StatusComponent.h"


void UHealthBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UpBuffDesp->SetVisibility(ESlateVisibility::Hidden);
	DownBuffDesp->SetVisibility(ESlateVisibility::Hidden);

}

void UHealthBarWidget::Init(AGameCharacterBase* pClass)
{
	if (!pClass) return;
	
	Class = pClass;

	StartRefreshTimer();
}

void UHealthBarWidget::StartRefreshTimer()
{
	if (!GetWorld()) return;
	
	GetWorld()->GetTimerManager().SetTimer(
		BuffRefreshTimer, this, &UHealthBarWidget::RefreshBuffs, 0.2f, true, 0.f);

}
void UHealthBarWidget::RefreshBuffs()
{
	if (!UpBuff || !DownBuff || !BuffWidgetClass) return;

	if (!Class) return;
	
	UDamageComponent * DmgComp = Class->FindComponentByClass<UDamageComponent>();
	if (!DmgComp) return;
	
	const TArray<FActiveBuff> CurBuffs = DmgComp->GetActiveBuffs();
	const TArray<FActiveStatusEffect> CurSEs = DmgComp->GetActiveStatusEffects();


	TSet<FName> UpKeys; //버프
	TSet<FName> DownKeys; //디버프랑 상태이솽


	for (const FActiveBuff& B : CurBuffs)
	{
		
		const FName Key = B.BuffInfo.SkillBuffName;
		const bool bDeBuff = (B.BuffInfo.BuffType == ESkillBuffType::DeBuff);

		UUniformGridPanel* Panel = bDeBuff ? DownBuff : UpBuff;
		TMap<FName, UBuffEffect*>& Map = bDeBuff ? DownBuffMap : UpBuffMap;
		TSet<FName>& KeepSet = bDeBuff ? DownKeys : UpKeys;
		KeepSet.Add(Key);

		UBuffEffect** FoundPtr = Map.Find(Key);
		if (!FoundPtr || !IsValid(*FoundPtr))
		{
			UBuffEffect* W = CreateWidget<UBuffEffect>(this, BuffWidgetClass);
			if (!W) continue;
			
			W->AddToViewport();
			if (Panel) Panel->AddChild(W);

			W->Setup(Key, B.BuffInfo.Icon, B.RemainingDuration > 0.f ? B.RemainingDuration : B.BuffInfo.Duration, bDeBuff);
			W->UpdateRemaining(B.RemainingDuration);

			const int32 Index = Map.Num();
			PlaceInGrid(Panel, Index, W);
			Map.Add(Key, W);

		}
		else
		{
			UBuffEffect* W = *FoundPtr;
			if (!IsValid(W)) return;
			W->UpdateRemaining(B.RemainingDuration);
		}
	}

	for (const FActiveStatusEffect& E : CurSEs)
	{
		const FName Key = E.EffectData.SkillEffectName;
		DownKeys.Add(Key);
		UBuffEffect** FoundPtr = DownBuffMap.Find(Key);
		if (!FoundPtr || !IsValid(*FoundPtr))
		{
			UBuffEffect* W = CreateWidget<UBuffEffect>(this, BuffWidgetClass);
			if (!W) continue;

			if (DownBuff)
			{
			
				DownBuff->AddChild(W);
			}
			const float Total = E.RemainingDuration > 0.f ? E.RemainingDuration : E.EffectData.Duration;
			W->Setup(Key,E.EffectData.Icon, Total, true);

			W->UpdateRemaining(E.RemainingDuration);

			const int32 Index = DownBuffMap.Num();
			PlaceInGrid(DownBuff, Index, W);
			DownBuffMap.Add(Key, W);
		}
		else
		{
			UBuffEffect* W = *FoundPtr;
			if (!IsValid(W)) continue;
			W->UpdateRemaining(E.RemainingDuration);
		}
	}

	//UpBuff 제거
	TArray<FName> ToRemoveUpBuff;
	for (const auto& KVP : UpBuffMap)
		if (!UpKeys.Contains(KVP.Key)) ToRemoveUpBuff.Add(KVP.Key);

	for (const FName& Key : ToRemoveUpBuff)
	{
		if (UBuffEffect* W = UpBuffMap.FindRef(Key))
		{
			if (UpBuff && W->IsInViewport()) UpBuff->RemoveChild(W);
			W->RemoveFromParent();
		}
		UpBuffMap.Remove(Key);
	}
	RebuildGrid(UpBuff, UpBuffMap);


	TArray<FName> ToRemoveDownBuff;
	for (const auto& KVP : DownBuffMap)
		if (!DownKeys.Contains(KVP.Key)) ToRemoveDownBuff.Add(KVP.Key);

	for (const FName& Key : ToRemoveDownBuff)
	{
		if (UBuffEffect* W = DownBuffMap.FindRef(Key))
		{
			if (DownBuff && W->IsInViewport()) DownBuff->RemoveChild(W);
			W->RemoveFromParent();
		}
		DownBuffMap.Remove(Key);
	}
	RebuildGrid(DownBuff, DownBuffMap);

}
void UHealthBarWidget::RebuildGrid(UUniformGridPanel* Panel, const TMap<FName, UBuffEffect*>& Map)
{
	if (!Panel) return;
	int32 Index = 0;
	for (const auto& KVP : Map)
	{
		if (UBuffEffect* W = KVP.Value)
		{
			PlaceInGrid(Panel, Index, W);
			++Index;
		}
	}

	if (UUniformGridPanel* P = Cast<UUniformGridPanel>(Panel))
	{
		if (P == UpBuff)
			if (Index == 0)
				UpBuffDesp->SetVisibility(ESlateVisibility::Hidden);
			else
				UpBuffDesp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (P == DownBuff)
			if (Index == 0)
				DownBuffDesp->SetVisibility(ESlateVisibility::Hidden);
			else
				DownBuffDesp->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	}
}

int32 UHealthBarWidget::PlaceInGrid(UUniformGridPanel* Panel, int32 Index, UBuffEffect* Widget)
{
	if (!Panel || !Widget) return Index;
	const int32 Cols = FMath::Max(1, MaxColumns);
	const int32 Row = Index / Cols;
	const int32 Col = Index % Cols;

	if (UUniformGridSlot* pSlot = Cast<UUniformGridSlot>(Widget->Slot))
	{
		
		pSlot->SetRow(Row);
		pSlot->SetColumn(Col);
	}
	else
	{
		Panel->AddChildToUniformGrid(Widget, Row, Col);

	}

	return Index;
}

