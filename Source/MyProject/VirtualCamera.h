// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "WeightedValue.h"
#include "CameraDescriptor.h"
#include "VirtualCamera.generated.h"

class ADynamicPlayerController;

/**
 * 
 */
UCLASS()
class MYPROJECT_API AVirtualCamera : public AActor
{
	GENERATED_BODY()
	
public:
	AVirtualCamera(const FObjectInitializer& objectInitializer);
	AVirtualCamera();

	static FVector GetAveragePosition(AActor* actor);
	void TickIn(float DeltaTime);
	void TickOut(float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent)
	void OnCameraRemoved();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	FCameraDescriptor cameraDesc;
};