// Copyright 2023 Biznet It

#include "ReplayGameInstance.h"
#include "PlaystageDemo4.h"
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "Engine/DemoNetDriver.h"
#include "Engine/LocalPlayer.h"
#include "HighResScreenshot.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/WorldSettings.h"
#include "Engine/World.h"
#include "UnrealClient.h"
#include "Misc/Paths.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "Misc/NetworkVersion.h"
#include "Containers/Ticker.h"
#include <GameMapsSettings.h>

UReplayGameInstance::UReplayGameInstance()
{
    
}

void UReplayGameInstance::Init()
{
    //Register delegate for ticker callback
    TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UReplayGameInstance::Tick));
    
    Super::Init();

    ReplayStreamer = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();
    
    bRunQueueReplay = false;
}

void UReplayGameInstance::Shutdown()
{
    //Unregister ticker delegate
    FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
    Super::Shutdown();
}

//Gather all available replays
void UReplayGameInstance::BuildReplayList()
{
    ReplayList.Empty();
    if (ReplayStreamer.IsValid())
    {
        bUpdatingReplayList = true;
        ReplayStreamer->EnumerateStreams(FNetworkVersion::GetReplayVersion(), 0, FString(), TArray<FString>(), FEnumerateStreamsCallback::CreateUObject(this, &UReplayGameInstance::OnEnumerateStreamsComplete));
    }
}

//Called after ReplaySTreamer EnumerateStreams is complete
void UReplayGameInstance::OnEnumerateStreamsComplete(const FEnumerateStreamsResult& Streams)
{
    if (Streams.WasSuccessful())
    {
        if (Streams.FoundStreams.Num())
        {
            for (const auto& StreamInfo : Streams.FoundStreams)
            {
                float SizeInKb = StreamInfo.SizeInBytes / 1024.0f;

                //Parse info into a struct
                FS_ReplayEntry NewReplayEntry;
                NewReplayEntry.StreamInfo = StreamInfo;
                NewReplayEntry.StreamInfo.Name = StreamInfo.Name;
                NewReplayEntry.FriendlyName = StreamInfo.FriendlyName;
                //UTC time
                NewReplayEntry.Date = StreamInfo.Timestamp.ToString(TEXT("%m/%d/%Y %h:%M %A"));

                //Calculate Time since played
                FTimespan TimeSpan = FDateTime::Now() - StreamInfo.Timestamp;
                if (TimeSpan.GetDays())
                {
                    NewReplayEntry.TimeSpan = FString::Printf(TEXT("%d Days Ago"), TimeSpan.GetDays());
                }
                else if (TimeSpan.GetHours())
                {
                    NewReplayEntry.TimeSpan = FString::Printf(TEXT("%d Hours Ago"), TimeSpan.GetHours());
                }
                else if (TimeSpan.GetMinutes())
                {
                    NewReplayEntry.TimeSpan = FString::Printf(TEXT("%d Minutes Ago"), TimeSpan.GetMinutes());
                }
                else
                {
                    NewReplayEntry.TimeSpan = FString::Printf(TEXT("%d Seconds Ago"), TimeSpan.GetSeconds());
                }

                NewReplayEntry.Size = SizeInKb >= 1024.0f ? FString::Printf(TEXT("%.2f MB"), SizeInKb / 1024.0f) : FString::Printf(TEXT("%.2f KB"), SizeInKb);
                NewReplayEntry.bIsLive = StreamInfo.bIsLive;
                NewReplayEntry.NumViewers = StreamInfo.NumViewers;
                NewReplayEntry.NumHours = StreamInfo.LengthInMS / (1000 * 3600);
                NewReplayEntry.NumMinutes = (StreamInfo.LengthInMS % (1000 * 60)) - NewReplayEntry.NumHours * 3600;
                NewReplayEntry.NumSeconds = (StreamInfo.LengthInMS / 1000) % 60;

                ReplayList.Add(NewReplayEntry);
            }

            //Sort the replay list by date
            struct FCompareDateTime
            {
                FORCEINLINE bool operator()(const FS_ReplayEntry& A, const FS_ReplayEntry& B) const
                {
                    return A.StreamInfo.Timestamp.GetTicks() > B.StreamInfo.Timestamp.GetTicks();
                }
            };
            Sort(ReplayList.GetData(), ReplayList.Num(), FCompareDateTime());
        }
    }
    else
    {
        //const UEnum* ResultStateEnum = FindObject(UEnum::StaticClass(), TEXT("EReplayStreamerResult"));
        //const UEnum* ResultStateEnum = FindObject<UEnum>(StaticClass(), TEXT("EReplayStreamerResult"));
        //const UEnum* ResultStateEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EReplayStreamerResult"));

        const UEnum* ResultStateEnum = FindObject<UEnum>(nullptr, TEXT("EReplayStreamerResult"));

        //const UEnum* ResultStateEnum = FindObject<UEnum>(UEngineTypes::ConvertToObjectType(TEXT("/Script/YourModuleName")), TEXT("EReplayStreamerResult"));
        //UE_LOG(LogPlaystageDemo4, Warning, TEXT("EnumerateStreams failed with result %s"), *ResultStateEnum->GetNameStringByIndex((int32)Streams.Result));
    }

    OnBuiltReplayListFinished();
}

//Called when the replay list is finished building
void UReplayGameInstance::OnBuiltReplayListFinished()
{
    bUpdatingReplayList = false;

    //Activate Event Dispatcher for any c++ or blueprints assignements, used for UI
    ED_OnBuildReplayListFinished.Broadcast();
}

//Report back the current percentage of time played and total time
float UReplayGameInstance::ReplayPercentage()
{
    UWorld* CurrentWorld = GetWorld();
    if (CurrentWorld == nullptr)
    {
        //UE_LOG(LogPlaystageDemo4, Warning, TEXT("ReplayPercentage: CurrentWorld is null"));
        return 0.0f;
    }
    UDemoNetDriver* DemoNetDriver = CurrentWorld->GetDemoNetDriver();
    if (DemoNetDriver == nullptr)
    {
        //UE_LOG(LogPlaystageDemo4, Warning, TEXT("ReplayPercentage: DemoNetDriver is null"));
        return 0.0f;
    }

    return DemoNetDriver->GetDemoCurrentTime() / DemoNetDriver->GetDemoTotalTime();
}

//Get Replay Total Time
float UReplayGameInstance::ReplayTotalTime()
{
    if (GetWorld() && GetWorld()->GetDemoNetDriver())
    {
        return GetWorld()->GetDemoNetDriver()->GetDemoTotalTime();
    }
    return 0.0f;
}

//Get Current Replay Time
float UReplayGameInstance::ReplayCurrentTime()
{
    if (GetWorld() && GetWorld()->GetDemoNetDriver())
    {
        return GetWorld()->GetDemoNetDriver()->GetDemoCurrentTime();
    }
    return 0.0f;
}

//Get Replay Total Frames
int32 UReplayGameInstance::ReplayTotalTimeFrame()
{
    if (GetWorld() && GetWorld()->GetDemoNetDriver())
    {
        //return GetWorld()->GetDemoNetDriver()->ReplayTotalFrames;
    }
    return 0;
}

//Get Current Replay Frame
int32 UReplayGameInstance::ReplayCurrentFrame()
{
    if (GetWorld() && GetWorld()->GetDemoNetDriver())
    {
        return GetWorld()->GetDemoNetDriver()->GetDemoFrameNum();
    }
    return 0;
}

//Get Current Replay Time Dilation
float UReplayGameInstance::ReplayTimeDilation()
{
    if (GetWorld() && GetWorld()->GetWorldSettings())
    {
        return GetWorld()->GetWorldSettings()->DemoPlayTimeDilation;
    }
    return 0.0f;
}

//Go to specific time in the replay playback
void UReplayGameInstance::GoToReplayTimeInSeconds(float Time)
{
    bool bIsPaused = IsReplayPaused();
    if (GetWorld() && GetWorld()->GetDemoNetDriver())
    {
        GetWorld()->GetDemoNetDriver()->GotoTimeInSeconds(Time);
        IsReplayPaused();
        if (bIsPaused)
        {
            //create a timer callback in .1 second to let the replay move and then re pause it
            GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RePauseReplay);
            GetWorld()->GetTimerManager().SetTimer(TimerHandle_RePauseReplay, this, &UReplayGameInstance::ResumeReplay, 0.1f, false);
        }
    }
}

//Set the Scrub State to record Pause and Unpause after scrubbing
void UReplayGameInstance::SetScrubReplaySate(bool IsEnabled)
{
    if (IsEnabled)
    {
        bIsReplayPausedAfterScrub = IsReplayPaused();
    }
    else
    {
        if (bIsReplayPausedAfterScrub)
        {
            //Create a timer callback in .1 second to let the replay move and then re pause it
            GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RePauseReplay);
            GetWorld()->GetTimerManager().SetTimer(TimerHandle_RePauseReplay, this, &UReplayGameInstance::ResumeReplay, 0.1f, false);
        }
    }
}

//Scrub to specific time of the replay playback
void UReplayGameInstance::ScrubReplayTimeInSeconds(float Time)
{
        //Paused state already handed by calling ScrubReplayState()
        if (GetWorld() && GetWorld()->GetDemoNetDriver())
        {
            GetWorld()->GetDemoNetDriver()->GotoTimeInSeconds(Time);
        }
}

//Step Ahead in time by defined step size
void UReplayGameInstance::StepForward()
{
    GoToReplayTimeInSeconds(ReplayCurrentTime() + StepSize);
}

//Step Back in time by defined step size
void UReplayGameInstance::StepBackward()
{
    GoToReplayTimeInSeconds(ReplayCurrentTime() - StepSize);
}

//ONly called from a movement in time
void UReplayGameInstance::ResumeReplay()
{
    //Reset timer
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle_RePauseReplay);

    PauseReplay(true);
}

//Pause or Unpause the current playing replay
void UReplayGameInstance::PauseReplay(bool IsPaused, APlayerController* PlayerOwner)
{
    //Check if already in requested state, exit
    if (IsPaused == IsReplayPaused()) return;

    if (GetWorld() && GetWorld()->GetDemoNetDriver() && GetWorld()->GetDemoNetDriver()->ServerConnection && GetWorld()->GetDemoNetDriver()->ServerConnection->OwningActor)
    {
        PlayerOwner = Cast<APlayerController>(GetWorld()->GetDemoNetDriver()->ServerConnection->OwningActor);
    }

    if (!PlayerOwner)
    {
        PlayerOwner = GetFirstLocalPlayerController(GetWorld());
    }

    if (PlayerOwner && PlayerOwner->GetWorld() && PlayerOwner->GetWorld()->GetWorldSettings())
    {
        static auto CV_MotionBlur = IConsoleManager::Get().FindConsoleVariable(TEXT("r.MotionBlurQuality"));
        static auto CV_AntiAliasing = IConsoleManager::Get().FindConsoleVariable(TEXT("r.DefaultFeature.AntiAliasing"));

        if (IsPaused)
        {
            //Turn off motion blur and AA during paused state
            if (CV_MotionBlur && CV_AntiAliasing)
            {
                //Cache off normal settings
                CachedMotionBlur = CV_MotionBlur->GetInt();
                CachedAntiAliasing = CV_AntiAliasing->GetInt();

                CV_MotionBlur->Set(0);
                CV_AntiAliasing->Set(0);
            }
            PlayerOwner->GetWorld()->GetWorldSettings()->SetPauserPlayerState(PlayerOwner->PlayerState);
        }
        else
        {
            if (CV_MotionBlur && CV_AntiAliasing)
            {
                //Restore normal settings
                CV_MotionBlur->Set(CachedMotionBlur);
                CV_AntiAliasing->Set(CachedAntiAliasing);
            }
            PlayerOwner->GetWorld()->GetWorldSettings()->SetPauserPlayerState(nullptr);
        }
        //Activate Event Dispatcher for any c++ or blueprints assignements, used for UI
        ED_OnReplayPauseChanged.Broadcast(IsPaused);
    }
}

//Is the current replay in a paused state
bool UReplayGameInstance::IsReplayPaused()
{
    return GetWorld()->GetWorldSettings()->GetPauserPlayerState() != nullptr;
}

//Go to specific time in the replay playback
void UReplayGameInstance::StopReplay()
{
    if (GetWorld() && GetWorld()->GetDemoNetDriver())
    {
        GetWorld()->GetDemoNetDriver()->StopDemo();
    }

    //Reload to Project Default Game Map
    
    //const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
    //if (GameMapsSettings)
    //{
      //  const FString DefaultMap = GameMapsSettings->GetGameDefaultMap();
       // UGameplayStatics::OpenLevel(this, FName(*DefaultMap), true);
    //}
}

//Play Replay from the Start
void UReplayGameInstance::PlayReplayFromStart(const FString ReplayName, float StartTime)
{
    //Make sure we arent paused
    if (IsReplayPaused())
    {
        PauseReplay(false);
    }

    //Check for replay, used to fiz timing issues, calling from tick
    QueueReplayName = ReplayName;
    if (StartTime > 0.0f)
    {
        FTimerHandle TimerHandle_PlayReplay;
        GetWorld()->GetTimerManager().SetTimer(TimerHandle_PlayReplay, this, &UReplayGameInstance::PlayQueueReplay, StartTime, false);
        return;
    }

    //Play the replay
    PlayReplay(ReplayName);
}

//A delayed replay play call
void UReplayGameInstance::PlayQueueReplay()
{
    //In the next tick, play the replay
    bRunQueueReplay = true;
}

//Delete requested replay by name
void UReplayGameInstance::DeleteReplay(const FString ReplayName)
{
    if (ReplayStreamer.IsValid())
    {
        ReplayStreamer->DeleteFinishedStream(ReplayName, FDeleteFinishedStreamCallback::CreateUObject(this, &UReplayGameInstance::OnDeleteFinishedStreamComplete));

    }
}

void UReplayGameInstance::OnDeleteFinishedStreamComplete(const FDeleteFinishedStreamResult Result)
{
    //Refresh the replay list
    BuildReplayList();

    //Check for errors
    if (!Result.WasSuccessful())
    {
        const UEnum* ResultStateEnum = FindObject<UEnum>(nullptr, TEXT("EReplayStreamerResult"));
        //const UEnum* ResultStateEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EReplayStreamerResult"));
        //const UEnum* ResultStateEnum = FindObject<UEnum>(StaticClass(), TEXT("EReplayStreamerResult"));
        //UE_LOG(LogPlaystageDemo4, Warning, TEXT("DeleteFinishedStream failed with result %s"), *ResultStateEnum->GetNameStringByIndex((int32)Result.Result));
    }
}

const FString UReplayGameInstance::GetLastScreenshotPath()
{
    const FString CachedScreenshotName = FScreenshotRequest::GetFilename();
    return FPaths::ConvertRelativePathToFull(CachedScreenshotName);
}

//Start capturing movie for current playing replay
void UReplayGameInstance::StartCaptureMovie()
{
    //if (GetWorld() && GetWorld()->GetDemoNetDriver())
    //{
      //  GetWorld()->GetDemoNetDriver()->StartCaptureMovie();
    //}
}

//Start recording a replay with the given custon name and friendly name
void UReplayGameInstance::StartRecordingReplayBP(const FString InName, const FString FriendlyName)
{
    FString OutName = InName;
    FString OutFriendlyName = FriendlyName;
    //If no name is given, use the current date and time
    if (OutName.IsEmpty())
    {
        OutName = FDateTime::Now().ToString(TEXT("%m-%d-%Y_%h-%m-%s"));
    }

    //If no friendly name is given then default to Map name
    if (OutFriendlyName.IsEmpty())
    {
        OutFriendlyName = GetWorld()->GetMapName();
    }
    StartRecordingReplay(OutName, OutFriendlyName);
}

//Stop recording the current replay
void UReplayGameInstance::StopRecordingReplayBP()
{
    StopRecordingReplay();
}

//Stop capturing movie for current playing replay
void UReplayGameInstance::StopCaptureMovie()
{
    //if (GetWorld() && GetWorld()->GetDemoNetDriver())
    //{
      //  GetWorld()->GetDemoNetDriver()->StopCaptureMovie();
    //}
}

bool UReplayGameInstance::Tick(float DeltaTime)
{
    //Check for replay play request
    if (bRunQueueReplay)
    {
        bRunQueueReplay = false;
        PlayReplay(QueueReplayName);
    }

    return true;
}
