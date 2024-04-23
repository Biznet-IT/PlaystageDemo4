// Copyright 2023 Biznet It


#include "ReplayPlayerController.h"
#include "ReplayPlayerState.h"
#include "GameFramework/PlayerInput.h"
#include "Interfaces/NetworkPredictionInterface.h"
#include "GameMapsSettings.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/World.h"


AReplayPlayerController::AReplayPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bShowMouseCursor = true;
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bShouldPerformFullTickWhenPaused = true;

    bCanPossessWithoutAuthority = true;
}

//Setup some default Input Bindings for ReplayPlayerController since config access is off limits for plugins
void InitializeReplayPlayerControllerInputBindings(UPlayerInput* PlayerInput)
{
    static bool bReplayBindingsAdded = false;
    if (!bReplayBindingsAdded)
    {
        bReplayBindingsAdded = true;

        //Call up Menu Placeholder
        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ReplayInGameMenu", EKeys::Escape));

        //Increase the Replay Play Rate
        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ReplayIncreasePlayback", EKeys::MouseScrollUp));
        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ReplayIncreasePlayback", EKeys::Gamepad_FaceButton_Left));

        //Decrease the Replay Play Rate
        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ReplayDecreasePlayback", EKeys::MouseScrollDown));
        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ReplayDecreasePlayback", EKeys::Gamepad_FaceButton_Right));

        //Take Screenshot 
        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("ReplayScreenShot", EKeys::Gamepad_RightShoulder));
    }
}

void AReplayPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Replay Player Controller Input
    InputComponent->BindAction("ReplayInGameMenu", IE_Pressed, this, &AReplayPlayerController::OnToggleInGameMenu);

    InputComponent->BindAction("ReplayIncreasePlayback", IE_Pressed, this, &AReplayPlayerController::OnIncreasePlaybackSpeed);
    InputComponent->BindAction("ReplayDecreasePlayback", IE_Pressed, this, &AReplayPlayerController::OnDecreasePlaybackSpeed);
    InputComponent->BindAction("ReplayScreenShot", IE_Pressed, this, &AReplayPlayerController::OnTakeScreenShot);
}

void AReplayPlayerController::InitPlayerState()
{
    Super::InitPlayerState();

    //Newly Created PlayerState needs know it is a demo player controller
    AReplayPlayerState* pPS = Cast<AReplayPlayerState>(PlayerState);
    if (pPS)
    {
        pPS->bIsDemoSpectator = true;
    }
}

void AReplayPlayerController::BP_ToggleInGameMenu()
{
}

void AReplayPlayerController::OnTakeScreenShot()
{
    ConsoleCommand(TEXT("HighResShot 1"));
    //Send a Blueprint Event to Play Sound or other effects
}

void AReplayPlayerController::ChangeState(FName NewState)
{
    Super::ChangeState(NewState);
}

void AReplayPlayerController::OnPossess(APawn* PawnToPossess)
{
    //Special case for our pawn actor since we are technically a client not an authority

    if (PawnToPossess != NULL)
    {
        if (GetPawn() && GetPawn() != PawnToPossess)
        {
            UnPossess();
        }

        if (PawnToPossess->Controller != NULL)
        {
            PawnToPossess->Controller->UnPossess();
        }

        PawnToPossess->PossessedBy(this);

        // update rotation to match possessed pawn's rotation
        SetControlRotation(PawnToPossess->GetActorRotation());

        SetPawn(PawnToPossess);
        check(GetPawn() != NULL);

        if (GetPawn() != NULL)
        {
            GetPawn()->SetActorTickEnabled(true);
        }

        // get pawn, cast to INetworkPredictionInterface, get pawn, movement component and reset prediction data
        INetworkPredictionInterface* NetworkPredictionInterface = GetPawn() ? Cast<INetworkPredictionInterface, UPawnMovementComponent>(GetPawn()->GetMovementComponent()) : NULL;
        if (NetworkPredictionInterface)
        {
            NetworkPredictionInterface->ResetPredictionData_Server();
        }


        //INetworkPredictionInterface* NetworkPredictionInterface = GetPawn() ? Cast<INetworkPredictionInterface>(GetPawn()->GetMovementComponent()) : NULL;
        //if (NetworkPredictionInterface)
        //{
          //  NetworkPredictionInterface->ResetPredictionData_Server();
        //}

        AcknowledgedPawn = NULL;

        // Local PCs will have the Restart() triggered right away in ClientRestart (via PawnClientRestart()), but the server should call Restart() locally for remote PCs.
        // We're really just trying to avoid calling Restart() multiple times.
        if (!IsLocalPlayerController())
        {
            GetPawn()->Restart();
        }


        if (IsLocalPlayerController())
        {
            //Locally set possesions need to be tracked
            //PawnLocallySet = GetPawn();
            ClientRestart(GetPawn());

        }

        ChangeState(NAME_Playing);
        if (bAutoManageActiveCameraTarget)
        {
            AutoManageActiveCameraTarget(GetPawn());
            ResetCameraMode();
        }

        //Removed 4.22, taken over by OnNewPawn
        //UpdateNavigationComponents();
    }
}

void AReplayPlayerController::TakeOverVRPawn(APawn* aPawn)
{
    //Special case for our pawn actor since we are technically a client not an authority

    if (aPawn) return;

    Possess(aPawn);

    //Set the time dilation
    float InvertedTimeRate = 1.f / GetWorldSettings()->DemoPlayTimeDilation;
    aPawn->CustomTimeDilation = InvertedTimeRate;

    //makes the replay ignnore this actor when scrubbing
    aPawn->bNetStartup = true;

    //Make all atached children ignore scrubbing
    TArray<AActor*> PawnChildren;
    aPawn->GetAllChildActors(PawnChildren);
    if (PawnChildren.Num())
    {
        for (int32 i = 0; i < PawnChildren.Num(); i++)
        {
            if (PawnChildren[i])
            {
                PawnChildren[i]->bNetStartup = true;
            }
        }
    }
}

void AReplayPlayerController::ReleaseVRPawn()
{
    UnPossess();
    ChangeState(NAME_Spectating);
}

void AReplayPlayerController::OnToggleInGameMenu()
{
    //Send a Blueprint Event to bring up the in-game menu
    BP_ToggleInGameMenu();
}

void AReplayPlayerController::OnIncreasePlaybackSpeed()
{
    if (!ReplayPlaybackSpeeds.Num())
    {
        //UE_LOG(LogDemoPlayer, Warning, TEXT("ADemoPlayerController::OnIncreasePlaybackSpeed: PlayBackSpeeds not set in DemoSpectator"));
        return;
    }

    PlaybackSpeed = FMath::Clamp(PlaybackSpeed + 1, 0, ReplayPlaybackSpeeds.Num() - 1);

    GetWorldSettings()->DemoPlayTimeDilation = ReplayPlaybackSpeeds[PlaybackSpeed];

    //Set local pawn rate to be constant between any time dilation of the recording
    float InvertedTimeRate = 1.f / GetWorldSettings()->DemoPlayTimeDilation;
    if (GetPawn())
    {
        GetPawn()->CustomTimeDilation = InvertedTimeRate;
    }
}

void AReplayPlayerController::OnDecreasePlaybackSpeed()
{
    if (!ReplayPlaybackSpeeds.Num())
    {
        //UE_LOG(LogDemoPlayer, Warning, TEXT("ADemoPlayerController::OnDecreasePlaybackSpeed: PlayBackSpeeds not set in DemoSpectator"));
        return;
    }

    PlaybackSpeed = FMath::Clamp(PlaybackSpeed - 1, 0, ReplayPlaybackSpeeds.Num() - 1);

    GetWorldSettings()->DemoPlayTimeDilation = ReplayPlaybackSpeeds[PlaybackSpeed];

    //Set local pawn rate to be constant between any time dilation of the recording
    float InvertedTimeRate = 1.f / GetWorldSettings()->DemoPlayTimeDilation;
    if (GetPawn())
    {
        GetPawn()->CustomTimeDilation = InvertedTimeRate;
    }
}

float AReplayPlayerController::GetPlaybackSpeed()
{
    return GetWorldSettings()->DemoPlayTimeDilation;
}

void AReplayPlayerController::BeginPlay()
{
    Super::BeginPlay();
}
