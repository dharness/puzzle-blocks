#include "TD_Logging.h"
#include "Engine.h"


TD_Logging::TD_Logging() {}
TD_Logging::~TD_Logging() {}

void TD_Logging::LogDefault(FVector Vector)
{
	LogString(Vector.ToString());
}

void TD_Logging::LogDefault(FString Str)
{
	LogString(Str);
}

void TD_Logging::LogString(FString Str)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, Str);
}
