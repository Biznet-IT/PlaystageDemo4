// Copyright 2023 Biznet It

#include "PlaystageDemo4.h"
#include "Runtime/NetworkReplayStreaming/NullNetworkReplayStreaming/Public/NullNetworkReplayStreaming.h"
#include "Misc/NetworkVersion.h"
#include "ReplayGameInstance.h"

UReplayGameInstance::UReplayGameInstance()
{
	//RecordingName = "MyReplay";
	//FriendlyRecordingName = "My Replay";
}

void UReplayGameInstance::StartRecording(FString ReplayName, FString FriendlyName)
{
	StartRecordingReplay(ReplayName, FriendlyName);
}

void UReplayGameInstance::StopRecording()
{
	StopRecordingReplay();
}

void UReplayGameInstance::StartReplay(FString ReplayName)
{
	PlayReplay(ReplayName);
}

void UReplayGameInstance::FindReplays()
{
	if (EnumerateStreamsPtr.Get())
	{
		const TArray<FString> extra;
		const FEnumerateStreamsCallback callback = OnEnumerateStreamsComplete;
		EnumerateStreamsPtr.Get()->EnumerateStreams(FNetworkReplayVersion(), int32(), FString(), extra, callback);
	}
}

void UReplayGameInstance::RenameReplay(const FString& ReplayName, const FString& NewFriendlyReplayName)
{
	//Get File Info
	FNullReplayInfo Info;

	const FString DemoPath = FPaths::Combine(*FPaths::GameAgnosticSavedDir(), TEXT("Demos/"));
	const FString StreamDirectory = FPaths::Combine(*DemoPath, *ReplayName);
	const FString StreamFullBaseFilename = FPaths::Combine(*StreamDirectory, *ReplayName);
	const FString InfoFilename = StreamFullBaseFilename + TEXT(".replayinfo");

	TUniquePtr &InfoFileArchive(IFileManager::Get().CreateFileReader(*InfoFilename));

	if (InfoFileArchive.IsValidIndex() && InfoFileArchive->TotalSize() != 0)
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
		EnumerateStreamsPtr.Get()->DeleteFinishedStream(ReplayName, OnDeleteFinishedStreamCompleteDelegate);
	}
}

void UReplayGameInstance::Init()
{
	Super::Init();

	// create a ReplayStreamer for FindReplasys() and DeleteReplay(..)
	EnumerateStreamsPtr = FNetworkReplayStreaming::Get().GetFactory().CreateReplayStreamer();
	// Link FindReplays() delegate to function
	OnEnumerateStreamsCompleteDelegate = FOnEnumerateStreamsComplete::CreateUObject(this, &UReplayGameInstance::OnDeleteFinishedStreamComplete);
	// Link DeleteReplay() delegate to function
	OnDeleteFinishedStreamCompleteDelegate = FOnDeleteFinishedStreamComplete::CreateUObject(this, &UReplayGameInstance::OnDeleteFinishedStreamComplete);

	OnEnumerateStreamsCallbackDelegate = FEnumerateStreamsCallback::CreateUObject(this, &UReplayGameInstance::OnEnumerateStreamsCallback)
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

void UReplayGameInstance::OnEnumerateStreamsCallback(const TArray<FNetworkReplayStreamInfo, FDefaultAllocator>& StreamInfos)
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

void UReplayGameInstance::OnDeleteFinishedStreamComplete(const bool bDeleteSucceeded)
{
	FindReplays();
}
