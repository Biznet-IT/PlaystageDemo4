// Copyright 2023 Biznet It

#include "ReplayGameInstance.h"
#include "PlaystageDemo4.h"
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "Misc/NetworkVersion.h"
#include <JsonObjectConverter.h>

UReplayGameInstance::UReplayGameInstance()
{
	//ReplayName = "MyReplay";
	//FriendlyName = "My Replay";
}

void UReplayGameInstance::StartRecording(FString ReplayName, FString FriendlyName)
{
	StartRecordingReplay(ReplayName, FriendlyName);
}

void UReplayGameInstance::StopRecording()
{
	StopRecordingReplay();
}

void UReplayGameInstance::StartReplay( FString ReplayName)
{
	PlayReplay(ReplayName);
}

void UReplayGameInstance::FindReplays()
{
	if (EnumerateStreamsPtr.Get())
	{
		const TArray<FString> extra;
		const FEnumerateStreamsCallback callback = OnEnumerateStreamsCallbackDelegate;
		EnumerateStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(), int32(), FString(), extra, callback);
	}
}

void UReplayGameInstance::RenameReplay(const FString ReplayName, const FString NewFriendlyReplayName)
{
	//Get File Info
	FNullReplayInfo Info;

	const FString DemoPath = FPaths::Combine(*FPaths::GameAgnosticSavedDir(), TEXT("Demos/"));
	const FString StreamDirectory = FPaths::Combine(*DemoPath, *ReplayName);
	const FString StreamFullBaseFilename = FPaths::Combine(*StreamDirectory, *ReplayName);
	const FString InfoFilename = StreamFullBaseFilename + TEXT(".replayinfo");

	TUniquePtr<FArchive> InfoFileArchive(IFileManager::Get().CreateFileReader(*InfoFilename));

	if (InfoFileArchive.IsValid() && InfoFileArchive->TotalSize() != 0)
	{ 
		FString JsonString;
		*InfoFileArchive << JsonString;

		Info.FromJson(JsonString);
		Info.bIsValid = true;

		InfoFileArchive->Close();
	}

	// Set FriendlyName
	Info.FriendlyName = NewFriendlyReplayName;

	// Write File Info
	TUniquePtr<FArchive> ReplayInfoFileAr(IFileManager::Get().CreateFileWriter(*InfoFilename));

	if (ReplayInfoFileAr.IsValid())
	{
		FString JsonString = Info.ToJson();
		*ReplayInfoFileAr << JsonString;

		ReplayInfoFileAr->Close();
	}
}

void UReplayGameInstance::DeleteReplay(const FString& ReplayName)
{
	if (EnumerateStreamsPtr.Get())
	{
	
		const FDeleteFinishedStreamCallback &call = OnDeleteFinishedStreamCallbackDelegate;
		EnumerateStreamsPtr.Get()->DeleteFinishedStream(ReplayName, call);

	}
}


void UReplayGameInstance::Init()
{
	Super::Init();

	// create a ReplayStreamer for FindReplasys() and DeleteReplay(..)
	EnumerateStreamsPtr = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();

	OnEnumerateStreamsCallbackDelegate = FEnumerateStreamsCallback::CreateUObject(this, &UReplayGameInstance::OnEnumerateStreamsCallback);
	
	OnDeleteFinishedStreamCallbackDelegate = FDeleteFinishedStreamCallback::CreateUObject(this, &UReplayGameInstance::OnDeleteFinishedStreamCallback);

}

void UReplayGameInstance::OnEnumerateStreamsComplete(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator> &StreamInfos)
{
	TArray<FS_ReplayInfo> AllReplays;
	for (FNetworkReplayStreamInfo StreamInfo : StreamInfos)
	{
		if (!StreamInfo.bIsLive)
		{
			AllReplays.Add(FS_ReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
		}
	}
	BP_OnFindReplaysComplete(AllReplays);
}


void UReplayGameInstance::OnEnumerateStreamsCallback(const FEnumerateStreamsResult &StreamInfos)
{
	TArray<FS_ReplayInfo> AllReplays;
	for (const FNetworkReplayStreamInfo& StreamInfo : StreamInfos.FoundStreams)
	{
		if (!StreamInfo.bIsLive)
		{
			AllReplays.Add(FS_ReplayInfo(StreamInfo.Name, StreamInfo.FriendlyName, StreamInfo.Timestamp, StreamInfo.LengthInMS));
		}
	}
	BP_OnFindReplaysComplete(AllReplays);
}


void UReplayGameInstance::AddCameraToArray(const FS_ReplayCameraInfo CameraInfo, const FString ReplayName)
{
    // Add Camera to Array
    for (FS_ReplayCameraArray CamArray : CameraArray)
    {
        if (CamArray.ReplayName == ReplayName)
        {
            CamArray.CameraArray.Add(CameraInfo);
            return;
        }
    }

    //CameraArray.CameraArray.Add(CameraInfo);
}

void UReplayGameInstance::RemoveCameraFromArray(const int Index, const FString ReplayName)
{
    // Remove Camera from Array
    for (FS_ReplayCameraArray CamArray : CameraArray)
    {
        if (CamArray.ReplayName == ReplayName)
        {
            CamArray.CameraArray.RemoveAt(Index);
            return;
        }
    }
    //WCameraArray.CameraArray.RemoveAt(Index);
}

void UReplayGameInstance::OnDeleteFinishedStreamCallback(const FDeleteFinishedStreamResult &res)
{
	FindReplays();
}



void UReplayGameInstance::OverwriteReplay(const FString& ReplayName)
{
    if (EnumerateStreamsPtr.Get())
    {
        const FDeleteFinishedStreamCallback& call = OnDeleteFinishedStreamCallbackDelegate;
        EnumerateStreamsPtr.Get()->DeleteFinishedStream(ReplayName, call);
    }
}

