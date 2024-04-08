// Copyright 2023 Biznet It


#include "ReplayPlayerController.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"

void AReplayPlayerController::RestartRecording()
{
	if (UWorld* World = GetWorld())
	{
		/*if (UDemoNetDriver* DemoDriver = World->DemoNetDriver)*/
		if (UDemoNetDriver* DemoDriver = World->GetDemoNetDriver())
		{
			DemoDriver->GotoTimeInSeconds(0.f);
		}
	}
}
