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
USTRUCT(BlueprintType)
struct FS_ReplayCameraInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    FString CameraName;

    UPROPERTY(BlueprintReadOnly)
    int32 CameraDuration;

    UPROPERTY(BlueprintReadOnly)
    FVector CameraLocation;

    UPROPERTY(BlueprintReadOnly)
    FRotator CameraRotation;

    FS_ReplayCameraInfo(FString NewCameraName, int32 NewCameraDuration, FVector NewCameraLocation, FRotator NewCameraRotation)
    {
        CameraName = NewCameraName;
        CameraDuration = NewCameraDuration;
        CameraLocation = NewCameraLocation;
        CameraRotation = NewCameraRotation;
    }

    FS_ReplayCameraInfo()
    {
        CameraName = "Camera";
        CameraDuration = 2;
        CameraLocation = FVector(0, 0, 0);
        CameraRotation = FRotator(0, 0, 0);

    }
};
USTRUCT(BlueprintType)  
struct FS_ReplayCameraArray
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly)
    TArray<FS_ReplayCameraInfo> CameraArray;

    UPROPERTY(BlueprintReadOnly)
    FString ReplayName;


    FS_ReplayCameraArray(TArray<FS_ReplayCameraInfo> NewCameraArray)
    {
        CameraArray = NewCameraArray;
    }

    FS_ReplayCameraArray()
    {
        CameraArray = TArray<FS_ReplayCameraInfo>();
    }

    void Add(FS_ReplayCameraInfo CameraInfo)
    {
        CameraArray.Add(CameraInfo);
    }

    FS_ReplayCameraArray(FS_ReplayCameraArray& NewCameraArray)
    {
        CameraArray = NewCameraArray.CameraArray;
    }

    int Length() const
    {
        return CameraArray.Num();
    }

    void RemoveAt(int Index)
    {
        CameraArray.RemoveAt(Index);
    }
};
UCLASS()
class PLAYSTAGEDEMO4_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UReplayGameInstance();

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
	void RenameReplay(const FString ReplayName, const FString NewFriendlyReplayName);

	/** Delete a previously recorded replay*/
	UFUNCTION(BlueprintCallable, Category = "Replays")
	void DeleteReplay( const FString& ReplayName);

    /* Overwrite a previously recorde replay */
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void OverwriteReplay(const FString& ReplayName);

    /* Stop the replay and go back*/
    //UFUNCTION(BlueprintCallable, Category = "Replays")
    //void StopReplay();

    /* Save Camera to array*/
    //UFUNCTION(BlueprintCallable, Category = "Replays")
    //void SaveCamera(const FString& CameraName, const int32 CameraDuration, const FVector CameraLocation, const FRotator CameraRotation);

    /* add to array */
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void AddCameraToArray(const FS_ReplayCameraInfo CameraInfo, const FString ReplayName);

    /* Remove from array */
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void RemoveCameraFromArray(const int Index, const FString ReplayName);


    /* Declare Camera Array */
    UPROPERTY(BlueprintReadOnly)
    TArray<FS_ReplayCameraArray> CameraArray;


	virtual void Init() override;



private:

	// for FindReplays()
	TSharedPtr<INetworkReplayStreamer> EnumerateStreamsPtr;
	FOnEnumerateStreamsComplete OnEnumerateStreamsCompleteDelegate;

	//void OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator> StreamInfos);
	void OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator>& StreamInfos);

	FEnumerateStreamsCallback OnEnumerateStreamsCallbackDelegate;
	void OnEnumerateStreamsCallback(const FEnumerateStreamsResult& res);

	// for DeleteReplays(..)
	FOnDeleteFinishedStreamComplete OnDeleteFinishedStreamCompleteDelegate;
	void OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded);

	FDeleteFinishedStreamCallback OnDeleteFinishedStreamCallbackDelegate;
	void OnDeleteFinishedStreamCallback(const FDeleteFinishedStreamResult& res);

	FDeleteFinishedStreamResult OnDeleteFinishedStreamResultDelegate;
	void OnDeleteFinishedStreamResult(const bool bDeleteSuceeded);
	//FDeleteFinishedStreamCallback

   



protected:
	UFUNCTION(BlueprintImplementableEvent, Category= "Replays")
	void BP_OnFindReplaysComplete(const TArray<FS_ReplayInfo> &AllReplays);
};
