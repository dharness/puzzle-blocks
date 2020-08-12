#include "toadEditor.h"
#include "EditorModeRegistry.h"
#include "AnimNodeEditMode.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Textures/SlateIcon.h"
#include "TD_HandIKEditMode.h"
#include "TD_AnimEditModes.h"

void FToadEditorModule::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FTD_HandIKEditMode>(TD_AnimNodeEditModes::TDHandIK, FText::FromString("TD_HandIKEditor"), FSlateIcon(), false);
}

void FToadEditorModule::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(TD_AnimNodeEditModes::TDHandIK);
}

IMPLEMENT_GAME_MODULE(FToadEditorModule, toadEditor);