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
	static void LogDefault(char* PtrChar);
	static void LogDefault(bool Bool);
	static void LogDefault(int32 Int);
	static void LogString(FString Str);
};
