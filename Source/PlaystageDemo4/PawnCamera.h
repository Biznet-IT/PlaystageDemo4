// Copyright 2024 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
//#include "CinematicCamera/CineCameraComponent.h"
//#include "CineCameraComponent.h"
//#include "CinematicCamera/CineCameraSettings.generated.h"
//#include "CinematicCamera/Public/CineCameraSettings.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/DefaultPawn.h"
#include "PawnCamera.generated.h"

/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO4_API APawnCamera : public ADefaultPawn
{
	GENERATED_BODY()

public:

    //Constructor

    APawnCamera();

protected:
    //Called when the game starts
    virtual void BeginPlay() override;

public:
    //virtual void Tick(float DeltaTime) override;

    //Called to bind functionality to input
    //virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    /* Camera Boom */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;
    /* Camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;
    /* Scene Capture Component 2D */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USceneCaptureComponent2D* SceneCaptureComponent;

    /*
    //add scene capture 2d component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    class USceneCaptureComponent2D* SceneCaptureComponent;

    //add a bool that says if it Is Selected
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    bool bIsSelected = false;
    */
    //add a float called CameraDuration
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    int32 CameraDuration;
    
};
