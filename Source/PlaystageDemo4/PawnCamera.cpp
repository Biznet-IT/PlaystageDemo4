// Copyright 2024 Biznet It


#include "PawnCamera.h"
#include "Camera/CameraComponent.h"
//#include "Components/CineCameraComponent.h"
//#include "CinematicCamera/Public/CineCameraComponent.h"
//#include "CinematicCamera/CineCameraSettings.h"
//include cine camera settings
//#include "CinematicCamera/Public/CineCameraSettings.h"
//#include "CinematicCamera/Public/CineCameraSettings.generated.cpp"
#include "Components/SceneCaptureComponent2D.h"

//Constructor
APawnCamera::APawnCamera()
{
    //Set this pawn to call Tick() every frame. Turn this off to improve performance if you don't need it.
   // PrimaryActorTick.bCanEverTick = false;

    /* Camera boom */
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
    SpringArmComp->SetupAttachment(RootComponent);
    SpringArmComp->bUsePawnControlRotation = true;
    SpringArmComp->TargetArmLength = 0.f;

    /* Camera */
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false;

    /* Scene Capture Component */
    SceneCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
    SceneCaptureComponent->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

    //Set the camera duration
    CameraDuration = 2.0f;
}

void APawnCamera::BeginPlay()
{
    Super::BeginPlay();
}

//void APawnCamera::Tick(float DeltaTime)
//{
  //  Super::Tick(DeltaTime);
//}
