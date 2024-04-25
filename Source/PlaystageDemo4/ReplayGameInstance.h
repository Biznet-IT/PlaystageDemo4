// Copyright 2023 Biznet It

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateMacros.h"
#include "Engine/GameInstance.h"
#include "NetworkReplayStreaming.h"
#include "ReplayGameInstance.generated.h"

//Bindable blueprint delegateFOnReplayBuildListDelegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReplayBuildListDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnReplayPauseChangedDelegate, bool, bIsPaused);

//Meta Object dor Replay Files
USTRUCT(BlueprintType)
struct FS_ReplayEntry
{
	GENERATED_USTRUCT_BODY()

    FNetworkReplayStreamInfo StreamInfo;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
	FString ReplayName;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
	FString FriendlyName;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    FString Date;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    FString TimeSpan;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    FString Size;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    int32 ResultsIndex = 0;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    bool bIsLive = false;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    int32 NumViewers = 0;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    int32 NumHours = 0;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    int32 NumMinutes = 0;

    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    int32 NumSeconds = 0;

    FS_ReplayEntry()
    {

    }

};
UCLASS(config = Game)
class PLAYSTAGEDEMO4_API UReplayGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UReplayGameInstance();
	
	//Get Current Replay Percentage
    UFUNCTION(BlueprintCallable, Category = "Replays")
    float ReplayPercentage();

    //Get Current Replay Total Time
    UFUNCTION(BlueprintCallable, Category = "Replays")
    float ReplayTotalTime();

    //Get Current Time of the Replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    float ReplayCurrentTime();

    //Get Current Replay Total Time in Frames
    UFUNCTION(BlueprintCallable, Category = "Replays")
    int32 ReplayTotalTimeFrame();

    //Get Current Replay Time in Frames
    UFUNCTION(BlueprintCallable, Category = "Replays")
    int32 ReplayCurrentFrame();

    //Get Current Replay time dilation
    UFUNCTION(BlueprintCallable, Category = "Replays")
    float ReplayTimeDilation();

    //Go to Specific Time in the Replay playback
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void GoToReplayTimeInSeconds(float Time);

    //Enable Scrubbing
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void SetScrubReplaySate(bool IsEnabled);

    //Scrub replay playback
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void ScrubReplayTimeInSeconds(float Time);

    //Repause after scrubbing
    UPROPERTY(BlueprintReadWrite, Category = "Replays")
    bool bIsReplayPausedAfterScrub = false;

    //Step forwards in time
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StepForward();

    //Step backwards in time
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StepBackward();

    //Amount of step forward or backwards
    UPROPERTY(BlueprintReadWrite, Category = "Replays")
    float StepSize = 1.f;
    
    //Pause current playing replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PauseReplay(bool IsPaused = true, APlayerController* PlayerOwner = nullptr);

    //Is the replay in a paused state
    UFUNCTION(BlueprintCallable, Category = "Replays")
    bool IsReplayPaused();

    //Resume current playing replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void ResumeReplay();

    FTimerHandle TimerHandle_RePauseReplay;

    //Event Dispatcher for UMG or other Replay accessors when paused state changes, used to update umg images
    UPROPERTY(BlueprintAssignable, Category = "Replays")
    FOnReplayPauseChangedDelegate ED_OnReplayPauseChanged;

    /** Stop current playing replay*/
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StopReplay();

    //Play Replay from the Start
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PlayReplayFromStart(const FString ReplayName, float StartTime = 0.0f);

    //Queue Replay to Play
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void PlayQueueReplay();

    FString QueueReplayName;
    bool bRunQueueReplay;

    //Play Replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StartReplay(const FString ReplayName);

    //DeleteReplay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void DeleteReplay(const FString& ReplayName);

    //Called by delegate when the replay streaming intergace has finished deleting
    void OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult Result);
    //void OnDeleteFinishedStreamComplete(bool bDeleteSucceeded);

    //Get the path to the last screenshot for easy access
    UFUNCTION(BlueprintCallable, Category = "Replays")
    const FString GetLastScreenshotPath();

    //Start capturing movie for current playing replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StartCaptureMovie();

    //Stop capturing movie for current playing replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    void StopCaptureMovie();

    //Start recording a replay with the given custon name and friendly name
    UFUNCTION(BlueprintCallable, Category = "Replays")
    virtual void StartRecordingReplayBP(const FString InName = TEXT(""), const FString FriendlyName = TEXT(""));

    //Stop recording the current replay
    UFUNCTION(BlueprintCallable, Category = "Replays")
    virtual void StopRecordingReplayBP();

    //~~~~~~~~~~~~~~~~~~Streamer Section

    // Init Replay Streamer
	virtual void Init() override;
    virtual void Shutdown() override;

    //Build a list of replays
    UFUNCTION(BlueprintCallable, Category = "ReplayStreamer")
    virtual void BuildReplayList();

    //Called when we get results from the replay streaming interface

    void OnEnumerateStreamsComplete(const FEnumerateStreamsResult& Streams);

    //Called when replay list built is finished
    void OnBuiltReplayListFinished();

    //Event Dispatcher for UMG or other Replay accessors when replay list is built
    UPROPERTY(BlueprintAssignable, Category = "ReplayStreamer")
    FOnReplayBuildListDelegate ED_OnBuildReplayListFinished;

    //Network Replay Streaming Interface
    TSharedPtr<INetworkReplayStreamer> ReplayStreamer;

    //Wether we are currently building a replay list or not
    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    bool bUpdatingReplayList = false;

    //List of replays we have access to
    UPROPERTY(BlueprintReadWrite, Category = "ReplayStreamer")
    TArray<FS_ReplayEntry> ReplayList;

private:

    int32 CachedMotionBlur;
    int32 CachedAntiAliasing;

    bool Tick(float DeltaSeconds);
    FTSTicker::FDelegateHandle TickDelegateHandle;

    //For DeleteReplays(...)
    FOnDeleteFinishedStreamComplete OnDeleteFinishedStreamCompleteDelegate;
    void OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded);

    FDeleteFinishedStreamCallback OnDeleteFinishedStreamCompleteCallbackDelegate;
    void OnDeleteFinishedStreamCompleteCallback(const FDeleteFinishedStreamResult Result);

    FDeleteFinishedStreamResult OnDeleteFinishedStreamCompleteResultDelegate;
    void OnDeleteFinishedStreamResult(const bool bDeleteSucceeded);
};
