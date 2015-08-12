// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef __MYPROJECT_H__
#define __MYPROJECT_H__

#include "Engine.h"

#define ERROR(fmt, ...)			UE_LOG(LogTemp, Error, TEXT(fmt), __VA_ARGS__);
#define WARNING(fmt, ...)		UE_LOG(LogTemp, Warning, TEXT(fmt), __VA_ARGS__);
#define LOG(fmt, ...)			UE_LOG(LogTemp, Log, TEXT(fmt), __VA_ARGS__);
#define SCREEN(color, fmt, ...)	GEngine->AddOnScreenDebugMessage(-1, 5.f, color, FString::Printf(TEXT(fmt), __VA_ARGS__));

UENUM(BlueprintType)
enum class ECameraBlendMode : uint8 {
	CutTo UMETA(DisplayName = "CutTo"),
	Timed UMETA(DisplayName = "Timed"),
	Proximity UMETA(DisplayName = "Proximity"),
	Dominant UMETA(DisplayName = "Dominant")
};

#endif
