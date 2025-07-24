// Fill out your copyright notice in the Description page of Project Settings.

#include "EditorHelpers.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FEditorHelpers"

void FEditorHelpers::StartupModule()
{
}

void FEditorHelpers::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEditorHelpers, EditorHelpers);