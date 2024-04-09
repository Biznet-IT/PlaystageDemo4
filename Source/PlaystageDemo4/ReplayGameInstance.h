// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetworkReplayStreaming.h"
#include "ReplayGameInstance.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct FS_ReplayInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString ReplayName;

	UPROPERTY(BlueprintReadOnly)
	FString FriendlyName;

	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;

	UPROPERTY(BlueprintReadOnly)
	int32 LenghtInMS;

	UPROPERTY(BlueprintReadOnly)
	bool bIsValid;

	FS_ReplayInfo(FString NewName, FString NewFriendlyName, FDateTime NewTimestamp, int32 NewLenghtInMS)
	{
		ReplayName = NewName;
		FriendlyName = NewFriendlyName;
		Timestamp = NewTimestamp;
		LenghtInMS = NewLenghtInMS;
		bIsValid = true;
	}

	FS_ReplayInfo()
	{
		ReplayName = "Replay";
		FriendlyName = "Replay";
		Timestamp = FDateTime::MinValue();
		LenghtInMS = 0;
		bIsValid = false;
	}
};
UCLASS()
class PLAYSTAGEDEMO4_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UReplayGameInstance();
	
	//UPROPERTY(EditDefaultsOnly, Category = "Replays")
	//FString RecordingName;

	//UPROPERTY(EditDefaultsOnly, Category = "Replays")
	//FString FriendlyRecordingName;

	UFUNCTION(BlueprintCallable, Category = "Replays")
	void StartRecording(FString ReplayName, FString FriendlyName);

	UFUNCTION(BlueprintCallable, Category = "Replays")
	void StopRecording();

	UFUNCTION(BlueprintCallable, Category = "Replays")
	void StartReplay(FString ReplayName);

	/**  Start looking for/finding replays on the hard drive*/
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void FindReplays();

	/* Apply a new custom name to the replay (for UI only)*/
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName);

	/** Delete a previously recorded replay*/
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void DeleteReplay(const FString& ReplayName);

	virtual void Init() override;

private:

	// for FindReplays()
	TSharedPtr<INetworkReplayStreamer> EnumerateStreamsPtr;
	//TSharedPtr<FOnDeleteFinishedStreamComplete> EnumerateStreamsPtr;
	//TSharedPtr EnumerateStreamsPtr;
	FOnEnumerateStreamsComplete OnEnumerateStreamsCompleteDelegate;

	//void OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator> StreamInfos);
	void OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator>& StreamInfos);

	FEnumerateStreamsCallback OnEnumerateStreamsCallbackDelegate;
	void OnEnumerateStreamsCallback(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator>& StreamInfos);

	// for DeleteReplays(..)
	FOnDeleteFinishedStreamComplete OnDeleteFinishedStreamCompleteDelegate;

	void OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category= "Replays")
	void BP_OnFindReplaysComplete(const TArray<FS_ReplayInfo> &AllReplays);
};
