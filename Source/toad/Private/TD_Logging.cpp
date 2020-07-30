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

void TD_Logging::LogDefault(char * PtrChar)
{
	LogString(FString(PtrChar));
}

void TD_Logging::LogDefault(bool Bool)
{
	LogString(Bool ? "true" : "false");
}

void TD_Logging::LogDefault(int32 Int)
{
	LogString(FString::FromInt(Int));
}

void TD_Logging::LogDefault(float Float)
{
	LogString(FString::SanitizeFloat(Float));
}

void TD_Logging::LogString(FString Str)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, Str);
}
