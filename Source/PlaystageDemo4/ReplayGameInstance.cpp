// Copyright 2023 Biznet It


#include "ReplayGameInstance.h"

UReplayGameInstance::UReplayGameInstance()
{
	RecordingName = "MyReplay";
	FriendlyRecordingName = "My Replay";
}

void UReplayGameInstance::StartRecording()
{
	StartRecordingReplay(RecordingName, FriendlyRecordingName);
}

void UReplayGameInstance::StopRecording()
{

}

void UReplayGameInstance::StartReplay()
{
	PlayReplay(RecordingName, nullptr);
}
