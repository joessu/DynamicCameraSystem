// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "VirtualCamera.h"
#include "CameraDescriptor.h"
#include "DynamicPlayerController.h"

ADynamicPlayerController::ADynamicPlayerController(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {
	VirtualCameras.Reserve(NUM_CAMERA_CHUNK);
}

void ADynamicPlayerController::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) {
	FCameraDescriptor cameraDesc;
	TArray<AVirtualCamera*> pendingDeactivations;

	for (auto* virtualCamera : VirtualCameras) {
		if (virtualCamera->cameraDesc.active && !virtualCamera->cameraDesc.deactivate) {
			if (virtualCamera->cameraDesc.weight < 1.0) {
				virtualCamera->TickIn(DeltaTime);
			}
			else {
				continue;
			}
		}
		if (virtualCamera->cameraDesc.deactivate) {
			if (virtualCamera->cameraDesc.weight > 0) {
				virtualCamera->TickOut(DeltaTime);
			}
			if (virtualCamera->cameraDesc.weight <= 0) {
				virtualCamera->cameraDesc.active = false;
				virtualCamera->cameraDesc.deactivate = false;
				virtualCamera->cameraDesc.weight = 0;
				pendingDeactivations.Add(virtualCamera);
			}
		}

	}

	for (auto* virtualCamera : pendingDeactivations) {
		virtualCamera->OnCameraRemoved();
		VirtualCameras.Remove(virtualCamera);
	}

	if (VirtualCameras.Num() < 1 || theTarget == nullptr || !GEngine) {
		if (theTarget == nullptr) {
			ERROR("Camera has no target");
		}
		OutResult.FOV = 90;
		OutResult.Location = FVector::ZeroVector;
		OutResult.Rotation = FRotator::ZeroRotator;
		return;
	}

	//average all active cameras
	cameraDesc = GetAverageCamera();
	
	float halfFov = cameraDesc.Fov / 2.0;
	const FIntPoint Screen = GEngine->GameViewport->Viewport->GetSizeXY();
	float horFov = halfFov * (float)Screen.X / (float)Screen.Y;
	float maxAzimuth = horFov * cameraDesc.MaxHorizontalOffset;
	float minAzimuth = horFov * cameraDesc.MinHorizontalOffset;
	float minElevation = halfFov * cameraDesc.MinVertOffset;
	float maxElevation = halfFov * cameraDesc.MaxVertOffset;

	
	FVector toTarget = theTarget->GetActorLocation() - PlayerCameraManager->GetCameraLocation();
	FQuat targetRot = FRotationMatrix::MakeFromX(toTarget).ToQuat();
	float curYaw = targetRot.Euler().Z;
	float curPitch = targetRot.Euler().Y;
	float curAzimuth = targetRot.Euler().Z - PlayerCameraManager->GetCameraRotation().Euler().Z;
	float curElevation = targetRot.Euler().Y - PlayerCameraManager->GetCameraRotation().Euler().Y;
	float desiredAzimuth = GetBoundedAngle(curAzimuth, maxAzimuth, minAzimuth);
	float desiredElevation = GetBoundedAngle(curElevation, maxElevation, minElevation);
	float desiredYaw = GetBoundedAngle(curYaw - desiredAzimuth, cameraDesc.MaxYaw, cameraDesc.MinYaw);
	float desiredPitch = GetBoundedAngle(curPitch - desiredElevation, cameraDesc.MaxPitch, cameraDesc.MinPitch);
	float desiredDistance = GetBounded(toTarget.Size(), cameraDesc.MaxDistance, cameraDesc.MinDistance);

	FVector targetToCam =
		FQuat::MakeFromEuler(FVector(0, desiredPitch + desiredElevation, desiredYaw + desiredAzimuth)) * FVector::ForwardVector;
	cameraDesc.Position = theTarget->GetActorLocation() - (targetToCam * desiredDistance);
	cameraDesc.Rotation = FQuat::MakeFromEuler(FVector(cameraDesc.Roll, desiredPitch, desiredYaw));

	//set camera
	OutResult.Location = cameraDesc.Position;
	OutResult.Rotation = FRotator(cameraDesc.Rotation);
	OutResult.FOV = cameraDesc.Fov;
}

void ADynamicPlayerController::Activate(AActor *NewVirtualCamera, FViewTargetTransitionParams TransitionParams) {
	AVirtualCamera* virtualCamera = (AVirtualCamera*)NewVirtualCamera;
	virtualCamera->cameraDesc.active = true;
	VirtualCameras.Add(virtualCamera); //(AVirtualCamera*)NewVirtualCamera;
}

void ADynamicPlayerController::Deactivate(AActor* VirtualCamera, FViewTargetTransitionParams TransitionParams) {
	AVirtualCamera* virtualCamera = (AVirtualCamera*)VirtualCamera;
	virtualCamera->cameraDesc.deactivate = true;
}

void ADynamicPlayerController::AttachTo(AActor* target) {
	SCREEN(FColor::Red, "Target actor is::: %s", *target->GetName());
	theTarget = target;
}

float ADynamicPlayerController::AverageAngles(TArray<WeightedFloat> angles) {
	TArray<WeightedVector> vectors;
	for (auto& angle : angles) {
		WeightedVector v;
		v.value = FVector2D(FMath::Cos(FMath::DegreesToRadians(angle.value)), FMath::Sin(FMath::DegreesToRadians(angle.value)));
		v.weight = angle.weight;
		vectors.Add(v);
	}

	FVector2D avgVector = FVector2D(0, 0);
	for (auto& vector : vectors) {
		avgVector += vector.value * vector.weight;
	}
	return FMath::RadiansToDegrees(FMath::Atan2(avgVector.Y, avgVector.X));
}

FCameraDescriptor ADynamicPlayerController::GetAverageCamera() {
	FCameraDescriptor avgCam;
	float totalWeight = 0;

	TArray<WeightedFloat> minYaws;
	TArray<WeightedFloat> maxYaws;

	TArray<WeightedFloat> minPitches;
	TArray<WeightedFloat> maxPitches;

	TArray<WeightedFloat> Rolls;
	
	for (auto* camera : VirtualCameras)
	{
		FCameraDescriptor& cam = camera->cameraDesc;
		const float weight = cam.weight;
		if (weight == 0)  {
			ERROR("Averaging a 0 weight camera");
			continue;
		}
		totalWeight += weight;
		avgCam.MinDistance += cam.MinDistance * weight;
		avgCam.MaxDistance += cam.MaxDistance * weight;
		avgCam.Fov += cam.Fov * weight;
		avgCam.MinHorizontalOffset += cam.MinHorizontalOffset * weight;
		avgCam.MaxHorizontalOffset += cam.MaxHorizontalOffset * weight;
		avgCam.MinVertOffset += cam.MinVertOffset * weight;
		avgCam.MaxVertOffset += cam.MaxVertOffset * weight;

		WeightedFloat minYaw = {cam.MinYaw, weight};
		minYaws.Add(minYaw);
		WeightedFloat maxYaw = { cam.MaxYaw, weight };
		maxYaws.Add(maxYaw);
		WeightedFloat minPitch = { cam.MinPitch, weight };
		minPitches.Add(minPitch);
		WeightedFloat maxPitch = { cam.MaxPitch, weight };
		maxPitches.Add(maxPitch);
		WeightedFloat roll = { cam.Roll, weight };
		Rolls.Add(roll);
	}

	if (totalWeight == 0) {
		ERROR("Total 0 Weight, Not Good");
		totalWeight = 1;
	}
	avgCam.MinYaw = AverageAngles(minYaws);
	avgCam.MaxYaw = AverageAngles(maxYaws);
	avgCam.MinPitch = AverageAngles(minPitches);
	avgCam.MaxPitch = AverageAngles(maxPitches);
	avgCam.Roll = AverageAngles(Rolls);
	avgCam.MinDistance /= totalWeight;
	avgCam.MaxDistance /= totalWeight;
	avgCam.Fov /= totalWeight;
	avgCam.MinHorizontalOffset /= totalWeight;
	avgCam.MaxHorizontalOffset /= totalWeight;
	avgCam.MinVertOffset /= totalWeight;
	avgCam.MaxVertOffset /= totalWeight;

	return avgCam;

}

float ADynamicPlayerController::ConstrainAngle(float angle)
{
	angle = FMath::Fmod(angle, 360.0f);
	if (angle < 0)
	{
		return angle + 360.0f;
	}
	return angle;
}

float ADynamicPlayerController::GetBounded(float current, float Max, float Min) {
	if (current > Max)
	{
		return Max;
	}
	else if (current < Min)
	{
		return Min;
	}
	else
	{
		return current;
	}
}

float ADynamicPlayerController::GetBoundedAngle(float current, float Max, float Min)
{
	// TODO: this could probably use some serious optimization
	float min = ConstrainAngle(Min);
	float max = ConstrainAngle(Max);
	current = ConstrainAngle(current);
	if (max == min)
	{
		return max;
	}
	if (max > min)
	{
		if ((current > min) && (current < max))
		{
			return current;
		}
		else
		{
			if (current < max)
			{
				current += 360.0f;
			}
			float up = current - max;
			if (min < current)
			{
				min += 360.0f;
			}
			float down = min - current;
			if (up < down)
			{
				return max;
			}
			else
			{
				return min;
			}
		}
	}
	else
	{
		if ((current < max) || (current > min))
		{
			return current;
		}
		else
		{
			float up = min - current;
			float down = current - max;
			if (up > down)
			{
				return max;
			}
			else
			{
				return min;
			}
		}
	}
}

//FVirtualCamera ADynamicCameraManager::GetCamera() {
//	return theVirtualCamera->GetCamera(this, theTarget);
//}