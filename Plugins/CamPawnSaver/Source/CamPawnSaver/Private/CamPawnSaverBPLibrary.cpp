// Copyright Epic Games, Inc. All Rights Reserved.

#include "CamPawnSaverBPLibrary.h"
#include "CamPawnSaver.h"

UCamPawnSaverBPLibrary::UCamPawnSaverBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

void UCamPawnSaverBPLibrary::SaveCameraPawnData(FString FileName, TArray<FCameraPawnData> Data)
{
    FString FullDataString = "";

    // FullDataString "CameraName\nLocationX;LocationY;LocationZ;RotationX;RotationY;RotationZ;Duration;TimeInit;TimeEnd;CurrentFocalLength;ManualFocus\nCameraName2\n etc"
    for (int i = 0; i < Data.Num(); i++)
    {
        FullDataString += Data[i].CameraName + "\n";
        FullDataString += FString::SanitizeFloat(Data[i].LocationX) + ";" + FString::SanitizeFloat(Data[i].LocationY) + ";" + FString::SanitizeFloat(Data[i].LocationZ) + ";" + FString::SanitizeFloat(Data[i].RotationX) + ";" + FString::SanitizeFloat(Data[i].RotationY) + ";" + FString::SanitizeFloat(Data[i].RotationZ) + ";" + FString::SanitizeFloat(Data[i].Duration) + ";" + FString::SanitizeFloat(Data[i].TimeInit) + ";" + FString::SanitizeFloat(Data[i].TimeEnd) + ";" + FString::SanitizeFloat(Data[i].CurrentFocalLength) + ";" + FString::SanitizeFloat(Data[i].ManualFocus) + "\n";
    }

    // encrypt data

    FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("CameraPawnData/");
    FString SaveFile = SaveDirectory + FileName + TEXT(".bin");

    FFileHelper::SaveStringToFile(FString(FullDataString), *SaveFile);
}

/*static TArray<FCameraPawnData> LoadCameraPawnData(FString FileName)
{
    FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("CameraPawnData/");
    FString SaveFile = SaveDirectory + FileName + TEXT(".bin");

    FString DataString;
    FFileHelper::LoadFileToString(DataString, *SaveFile);

    TArray<FCameraPawnData> Data;

    for (int i = 0; i < sizeof(DataString); i += sizeof(FCameraPawnData))
    {
        FCameraPawnData CameraPawnData = FCameraPawnData();
        memcpy(&CameraPawnData, DataString.GetCharArray().GetData() + i, sizeof(FCameraPawnData));

        Data.Add(CameraPawnData);
    }

    return Data;
}*/
