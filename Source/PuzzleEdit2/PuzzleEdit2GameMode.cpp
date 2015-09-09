// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "PuzzleEdit2.h"
#include "PuzzleEdit2GameMode.h"
#include "PuzzleEdit2PlayerController.h"

APuzzleEdit2GameMode::APuzzleEdit2GameMode()
{
	// no pawn by default
	DefaultPawnClass = NULL;
	// use our own player controller class
	PlayerControllerClass = APuzzleEdit2PlayerController::StaticClass();
}
