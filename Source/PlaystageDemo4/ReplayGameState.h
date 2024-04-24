// Copyright 2024 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "ReplayGameState.generated.h"

/*
Gamestate that can be used to AutoRecord Server & Client Demos
*/
UCLASS()
class PLAYSTAGEDEMO4_API AReplayGameState : public AGameState
{
	GENERATED_BODY()
	
public:

    //Server AutoRecord games when they start
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReplaySettings")
    bool bAutoRecordServer = true;

    //Client AutoRecord games when they start
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReplaySettings")
    bool bAutoRecordClient = false;

protected:
    //Called when the state transitions to InProgress, called on server & client
    virtual void HandleMatchHasStarted() override;

    //Called when the state transitions to WaitingPostMatch, called on server & client
    virtual void HandleMatchHasEnded() override;
};
