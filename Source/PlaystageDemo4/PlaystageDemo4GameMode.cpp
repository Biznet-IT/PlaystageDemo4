// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaystageDemo4GameMode.h"
#include "PlaystageDemo4Character.h"
#include "UObject/ConstructorHelpers.h"

APlaystageDemo4GameMode::APlaystageDemo4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
