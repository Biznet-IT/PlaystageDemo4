// Copyright 2024 Biznet It

#include "ReplayPlayerState.h"
#include "Net/UnrealNetwork.h"


void AReplayPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AReplayPlayerState, bIsDemoSpectator);
}
