// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

void ACustomPlayerController::ShowCursor(bool _bShow)
{
	bShowMouseCursor = _bShow;
	
	if (_bShow)
	{
		SetInputMode(FInputModeGameAndUI());
		int32 ViewportSizeX, ViewportSizeY;	
		GetViewportSize(ViewportSizeX, ViewportSizeY);	
		SetMouseLocation(ViewportSizeX / 1.5, ViewportSizeY / 1.5);	
	}
	else
		SetInputMode(FInputModeGameOnly());
}