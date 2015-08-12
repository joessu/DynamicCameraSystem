// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "WeightedValue.h"
#include "CameraDescriptor.h"
#include "DynamicPlayerController.generated.h"

class AVirtualCamera;

//Initial Camera Array Size
#define NUM_CAMERA_CHUNK 50

/**
 * 
 */
UCLASS()
class MYPROJECT_API ADynamicPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ADynamicPlayerController(const FObjectInitializer& objectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = VirtualCamera)
	void Activate(AActor * NewVirtualCamera, FViewTargetTransitionParams TransitionParams);

	UFUNCTION(BlueprintCallable, Category = VirtualCamera)
	void Deactivate(AActor* VirtualCamera, FViewTargetTransitionParams TransitionParams);

	//FVirtualCamera GetCamera();

	UFUNCTION(BlueprintCallable, Category = VirtualCamera)
	void AttachTo(AActor* target);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	TArray<AVirtualCamera*> VirtualCameras;

	AActor* theTarget;

	FCameraDescriptor GetAverageCamera();

	float GetBounded(float current, float Max, float Min);
	float ConstrainAngle(float angle);
	float GetBoundedAngle(float current, float Max, float Min);
	float AverageAngles(TArray<WeightedFloat> angles);

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;
};
