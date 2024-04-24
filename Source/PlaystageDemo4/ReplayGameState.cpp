// Copyright 2024 Biznet It

#pragma once

#include "GameFramework/GameState.h"
#include "Engine/World.h"
#include "EngineGlobals.h"
#include "IHeadMountedDisplay.h"
#include "IXRTrackingSystem.h"
#include "ReplayGameInstance.h"
#include "ReplayGameState.h"


//match has started
void AReplayGameState::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted();

    //Start recorder
    UReplayGameInstance* GI = GetWorld() != NULL ? GetWorld()->GetGameInstance<UReplayGameInstance>() : NULL;
    if (GI)
    {
        if (HasAuthority())
        {
            if (bAutoRecordServer)
            {
                GetWorld()->GetGameInstance()->StartRecordingReplay(FDateTime::Now().ToString(), GetWorld()->GetMapName());
            }
        }
        else
        {
            if (bAutoRecordClient)
            {
                GetWorld()->GetGameInstance()->StartRecordingReplay(FDateTime::Now().ToString(), GetWorld()->GetMapName());
            }
        }
    }
}

void AReplayGameState::HandleMatchHasEnded()
{
    Super::HandleMatchHasEnded();

    //Stop recorder
    UReplayGameInstance* GI = GetWorld() != NULL ? GetWorld()->GetGameInstance<UReplayGameInstance>() : NULL;
    if (GI)
    {
        GetWorld()->GetGameInstance()->StopRecordingReplay();
    }
}
