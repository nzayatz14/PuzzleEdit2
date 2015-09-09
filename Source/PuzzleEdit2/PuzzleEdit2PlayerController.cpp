// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzleEdit2.h"
#include "PuzzleEdit2PlayerController.h"

APuzzleEdit2PlayerController::APuzzleEdit2PlayerController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableTouchEvents = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}
