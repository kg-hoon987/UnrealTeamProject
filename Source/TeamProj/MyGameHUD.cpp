// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameHUD.h"
#include "MyGameController.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Character/GameCharacterBase.h"

AMyGameHUD::AMyGameHUD()
{
	StartMousePos.Normalize();
	CurrentMousePos.Normalize();
	isDrawing = false;
	SelectUnits.Empty();
	SelectUnitsInRect.Empty();
}

void AMyGameHUD::DrawHUD()
{
	Super::DrawHUD();


	if (isDrawing)
	{
		DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.4f), StartMousePos.X, StartMousePos.Y, CurrentMousePos.X - StartMousePos.X, CurrentMousePos.Y - StartMousePos.Y);

		GetActorsInSelectionRectangle(AAllyCharacter::StaticClass(), StartMousePos, CurrentMousePos, SelectUnitsInRect, false, false);


		for (int i = 0; i < SelectUnitsInRect.Num(); i++)
		{
			if (IWidgetInterface* IF = Cast<IWidgetInterface>(SelectUnitsInRect[i]))
			{
				IF->SelectUnit_Implementation();
				SelectUnits.AddUnique(SelectUnitsInRect[i]);

			}
		}
		for (int i = 0; i < SelectUnits.Num(); i++)
		{
			if (SelectUnitsInRect.Find(SelectUnits[i]) == -1)
			{
				if (IWidgetInterface* IF = Cast<IWidgetInterface>(SelectUnits[i]))
				{
					IF->DeSelectUnit_Implementation();
					SelectUnits.RemoveAt(i);
				}
			}
		}
	}

}

void AMyGameHUD::DragPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("DragStart"));
	isDrawing = true;
	if (AMyGameController* Controller = Cast<AMyGameController>(GetOwningPlayerController()))
	{
		float MouseX, MouseY;
		if (Controller->GetMousePosition(MouseX, MouseY))
		{
			StartMousePos = FVector2D(MouseX, MouseY);
		}
	}
}

void AMyGameHUD::DragReleased_Implementation()
{
	isDrawing = false;
}

void AMyGameHUD::DragHold_Implementation()
{
	if (AMyGameController* Controller = Cast<AMyGameController>(GetOwningPlayerController()))
	{
		float MouseX, MouseY;
		if (Controller->GetMousePosition(MouseX, MouseY))
		{
			CurrentMousePos = FVector2D(MouseX, MouseY);
		}
	}
}

void AMyGameHUD::Move_Implementation(FVector2D movePos)
{
	for (int i = 0; i < SelectUnits.Num(); i++)
	{
		if (IWidgetInterface* IF = Cast<IWidgetInterface>(SelectUnits[i]))
		{
			IF->Move_Implementation(movePos);
		}
	}
}
