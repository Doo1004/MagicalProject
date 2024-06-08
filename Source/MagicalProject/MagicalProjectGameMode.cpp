// Copyright Epic Games, Inc. All Rights Reserved.

#include "MagicalProjectGameMode.h"
#include "MagicalProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMagicalProjectGameMode::AMagicalProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
		DefaultPawnClass = PlayerPawnBPClass.Class;
}