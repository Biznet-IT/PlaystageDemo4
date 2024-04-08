// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkReplayStreaming.h"
#include "ReplayGameInstance.generated.h"
/**
 * 
 */
UCLASS()
class PLAYSTAGEDEMO4_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UReplayGameInstance();
	
	UPROPERTY(EditDefaultsOnly, Category = "Replays")
	FString RecordingName;

	UPROPERTY(EditDefaultsOnly, Category = "Replays")
	FString FriendlyRecordingName;

	UFUNCTION(BlueprintCallable, Category = "Replays")
	void StartRecording();

	UFUNCTION(BlueprintCallable, Category = "Replays")
	void StopRecording();

	UFUNCTION(BlueprintCallable, Category = "Replays")
	void StartReplay();
};
