// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class TOAD_API TD_Logging
{
public:
	TD_Logging();
	~TD_Logging();

	static void LogDefault(FVector Vector);
	static void LogDefault(FString Str);
	static void LogString(FString Str);
};
