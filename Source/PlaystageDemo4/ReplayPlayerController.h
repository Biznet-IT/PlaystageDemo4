// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ReplayPlayerState.h"
#include "ReplayPlayerController.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EVRMoveMode : uint8
{
    FLY         UMETA(DisplayName = "Fly Mode"),
    TELEPORT    UMETA(DisplayName = "Teleport Mode"),
    MOTION      UMETA(DisplayName = "Motion Controller Mode"),
    HYBRID      UMETA(DisplayName = "Hybrid Mode")
};

UCLASS(config = Game)
class PLAYSTAGEDEMO4_API AReplayPlayerController : public APlayerController
{
public:
    GENERATED_UCLASS_BODY()

    virtual void SetupInputComponent() override;
    virtual void InitPlayerState() override;

    //Change from spectating, to playing, etc
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    virtual void ChangeState(FName NewState) override;

    //Pawn possession
    virtual void OnPossess(APawn* aPawn) override;
    
    //Take over pawn for VR mode
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    virtual void TakeOverVRPawn(APawn* aPawn);

	//Release pawn for VR mode
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    virtual void ReleaseVRPawn();

    void OnToggleInGameMenu();

    //Increase PlayRate
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    void OnIncreasePlaybackSpeed();

    //Decrease PlayRate
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    void OnDecreasePlaybackSpeed();

    //Get Current Playback Rate
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    float GetPlaybackSpeed();

    //Available Playback Rates
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReplayPlayerController")
    TArray<float> ReplayPlaybackSpeeds;

    //Current Playback setting
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ReplayPlayerController")
    int32 PlaybackSpeed = 2;

    //Bring up the in-game menu
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    void BP_ToggleInGameMenu();

    //Take Screenshot
    UFUNCTION(BlueprintCallable, Category = "ReplayPlayerController")
    void OnTakeScreenShot();

protected:
    virtual void BeginPlay() override;
};
