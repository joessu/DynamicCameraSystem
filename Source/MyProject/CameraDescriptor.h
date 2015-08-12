#pragma once

#include "MyProject.h"
#include "CameraDescriptor.generated.h"

USTRUCT(BlueprintType)
struct FCameraDescriptor 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MaxYaw = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MinYaw = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MaxPitch = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MinPitch = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MinDistance = 0.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MaxDistance = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MaxHorizontalOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MinHorizontalOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MinVertOffset = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MaxVertOffset = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float MaxWeight = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float FadeInRate = 1.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float FadeOutRate = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float Roll = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VirtualCamera")
	float Fov = 0;

	bool active = false;
	bool deactivate = false;
	float weight = 0;

	FVector Position = FVector::ZeroVector;
	FQuat Rotation = FQuat::Identity;
};