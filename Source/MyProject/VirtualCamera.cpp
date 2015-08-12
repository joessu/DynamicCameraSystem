// Fill out your copyright notice in the Description page of Project Settings.

#include "MyProject.h"
#include "VirtualCamera.h"

AVirtualCamera::AVirtualCamera(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer) {


}

void AVirtualCamera::TickIn(float DeltaTime) {
		//TODO weight differently for different blend modes
	
	if (cameraDesc.weight < 1.0f)
		{
			cameraDesc.weight += cameraDesc.FadeInRate * DeltaTime;
		}
		else {
			//make sure we cap at 1
			cameraDesc.weight = 1.0f;
		}
}

void AVirtualCamera::TickOut(float DeltaTime) {
	//TODO weight differently for different blend modes
	if (cameraDesc.weight > 0)
	{
		cameraDesc.weight -= cameraDesc.FadeOutRate * DeltaTime;
	}
	else {
		//make sure we cap at 1
		cameraDesc.weight = 0.0f;
	}
}

FVector AVirtualCamera::GetAveragePosition(AActor* actor) {
	//implement
	return actor->GetActorLocation();
}

