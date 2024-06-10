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

TArray<FCameraPawnData> UCamPawnSaverBPLibrary::LoadCameraPawnData(FString FileName)
{
    FString SaveDirectory = FPaths::ProjectSavedDir() + TEXT("CameraPawnData/");
    FString SaveFile = SaveDirectory + FileName + TEXT(".bin");

    FString DataString;
    FFileHelper::LoadFileToString(DataString, *SaveFile);

    TArray<FCameraPawnData> Data = TArray<FCameraPawnData>();

    // decrypt data

    TArray<FString> DataArray;
    DataString.ParseIntoArray(DataArray, TEXT("\n"), true);

    for (int i = 0; i < DataArray.Num(); i += 2)
    {
        FCameraPawnData CameraPawnData = FCameraPawnData();
        CameraPawnData.CameraName = DataArray[i];

        TArray<FString> DataArray2;
        DataArray[i + 1].ParseIntoArray(DataArray2, TEXT(";"), true);

        CameraPawnData.LocationX = FCString::Atof(*DataArray2[0]);
        CameraPawnData.LocationY = FCString::Atof(*DataArray2[1]);
        CameraPawnData.LocationZ = FCString::Atof(*DataArray2[2]);
        CameraPawnData.RotationX = FCString::Atof(*DataArray2[3]);
        CameraPawnData.RotationY = FCString::Atof(*DataArray2[4]);
        CameraPawnData.RotationZ = FCString::Atof(*DataArray2[5]);
        CameraPawnData.Duration = FCString::Atof(*DataArray2[6]);
        CameraPawnData.TimeInit = FCString::Atof(*DataArray2[7]);
        CameraPawnData.TimeEnd = FCString::Atof(*DataArray2[8]);
        CameraPawnData.CurrentFocalLength = FCString::Atof(*DataArray2[9]);
        CameraPawnData.ManualFocus = FCString::Atof(*DataArray2[10]);

        Data.Add(CameraPawnData);
    }

    return Data;
}
