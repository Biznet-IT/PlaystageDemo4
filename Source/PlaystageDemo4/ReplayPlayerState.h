// Copyright 2024 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ReplayPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO4_API AReplayPlayerState : public APlayerState
{
	GENERATED_BODY()

    public:

        //Flag to indicate if we are a demo recording spectator, also needs to be replicated
        UPROPERTY(Replicated, BlueprintReadWrite, Category = "ReplayPlayer")
        bool bIsDemoSpectator = false;

        //In a demo, we should not be messing with sessions
        virtual void RegisterPlayerWithSession(bool bWasFromInvite) override {}

        //In a demo, we should not be messing with sessions
        virtual void UnregisterPlayerWithSession() override {}
};
