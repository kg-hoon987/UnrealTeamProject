// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUD.h"
#include "../Interface/WidgetInterface.h"

void UGameHUD::NativeConstruct()
{
	Super::NativeConstruct();
	SkillSlot->SetVisibility(ESlateVisibility::Hidden);
	if (CntStatusBtn)
	{
		CntStatusBtn->OnClicked.AddDynamic(this, &UGameHUD::OnCntStatusClicked);
		CntStatusBtn->OnHovered.AddDynamic(this, &UGameHUD::OnCntStatusBtnHovered);
		CntStatusBtn->OnUnhovered.AddDynamic(this, &UGameHUD::OnCntStatusBtnUnhovered);
	}
}

void UGameHUD::OnCntStatusBtnHovered()
{
	
	CntStatus->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}


void UGameHUD::OnCntStatusBtnUnhovered()
{

	CntStatus->SetVisibility(ESlateVisibility::Hidden);
}

void UGameHUD::OnCntStatusClicked()
{
	

}

void UGameHUD::SetSkillSlotVisibility()
{
	SkillSlot->SetVisibility(ESlateVisibility::Visible);
}

void UGameHUD::ChangeToMorning()
{
	if (CntGameStage && CntGameStage->GetClass()->ImplementsInterface(UHUDWidgetInterface::StaticClass()))
	{
		IHUDWidgetInterface::Execute_RoundChangeToNight(CntGameStage);
	}
}

void UGameHUD::ChangeToNight()
{
	if (CntGameStage && CntGameStage->GetClass()->ImplementsInterface(UHUDWidgetInterface::StaticClass()))
	{
		IHUDWidgetInterface::Execute_RoundChangeToMorning(CntGameStage);
	}
}
