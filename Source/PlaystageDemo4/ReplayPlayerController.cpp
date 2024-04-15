// Copyright 2023 Biznet It


#include "ReplayPlayerController.h"
#include "Engine/World.h"
#include "Engine/DemoNetDriver.h"


AReplayPlayerController::AReplayPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;
}

bool AReplayPlayerController::SetCurrentReplayPausedState(bool bDoPause)
{
	AWorldSettings* WorldSettings = GetWorldSettings();
	//Set MotionBlur off and Anti Aliasing to FXAA in order to bypass the pause-bug of both
	//static const auto CVarAA = IConsoleManager::Get().FindConsoleVariable(TEXT("DefaultFeature.AntiAliasing"));

	//static const auto CVarMB = IConsoleManager::Get().FindConsoleVariable(TEXT("DefaultFeature.MotionBlur"));

	if (bDoPause)
	{
		//PreviousAASetting = CVarAA->GetInt();
		//PreviousMBSetting = CVarMB->GetInt();

		// Set MotionBlur to OFF, Anti-Aliasing to FXAA
		//CVarAA->Set(1);
		//CVarMB->Set(0);

		WorldSettings->SetPauserPlayerState(PlayerState);
		return true;
	}
	// Rest MotionBlur and AA
	//CVarAA->Set(PreviousAASetting);
	//CVarMB->Set(PreviousMBSetting);

	WorldSettings->SetPauserPlayerState(NULL);
	return false;
}

int32 AReplayPlayerController::GetCurrentReplayTotalTimeInSeconds() const
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			return GetWorld()->GetDemoNetDriver()->GetDemoTotalTime();
		}
	}
	return 0.f;
}

int32 AReplayPlayerController::GetCurrentReplayCurrentTimeInSeconds() const
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			return GetWorld()->GetDemoNetDriver()->GetDemoCurrentTime();
		}
	}
	return 0.f;
}

void AReplayPlayerController::SetCurrentReplayTimeToSeconds(int32 Seconds)
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			GetWorld()->GetDemoNetDriver()->GotoTimeInSeconds(Seconds);
		}
	}
}

void AReplayPlayerController::SetCurrentReplayPlayRate(float PlayRate)
{
	if (GetWorld())
	{
		if (GetWorld()->GetDemoNetDriver())
		{
			GetWorld()->GetWorldSettings()->DemoPlayTimeDilation = PlayRate;
		}
	}
}


