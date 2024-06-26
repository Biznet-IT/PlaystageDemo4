#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "CamPawnSaverBPLibrary.generated.h"
USTRUCT(BlueprintType)
struct FCameraPawnData {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    FString CameraName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float LocationX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float LocationY;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float LocationZ;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float RotationX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float RotationY;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float RotationZ;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float Duration;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float TimeInit;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float TimeEnd;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SimpleJSONManager")
    float CurrentFocalLength;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SimpleJSONManager")
    float ManualFocus;

    FCameraPawnData(FString CameraName = "DefaultName", float LocationX = 0, float LocationY = 0, float LocationZ = 0, float RotationX = 0, float RotationY = 0, float RotationZ = 0, float Duration = 0, float TimeInit = 0, float TimeEnd = 0, float CurrentFocalLength = 0, float ManualFocus = 0)
    {
        this->CameraName = CameraName;
        this->LocationX = LocationX;
        this->LocationY = LocationY;
        this->LocationZ = LocationZ;
        this->RotationX = RotationX;
        this->RotationY = RotationY;
        this->RotationZ = RotationZ;
        this->Duration = Duration;
        this->TimeInit = TimeInit;
        this->TimeEnd = TimeEnd;
        this->CurrentFocalLength = CurrentFocalLength;
        this->ManualFocus = ManualFocus;
    }
};

UCLASS()
class UCamPawnSaverBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Save Camera Pawn Data", Keywords = "CamPawnSaver save camera pawn data"), Category = "CamPawnSaverTesting")
    static void SaveCameraPawnData(FString FileName, TArray<FCameraPawnData> Data);

    UFUNCTION(BlueprintCallable, meta = (DisplayName = "Load Camera Pawn Data", Keywords = "CamPawnSaver load camera pawn data"), Category = "CamPawnSaverTesting")
    static TArray<FCameraPawnData> LoadCameraPawnData(FString FileName);
};
