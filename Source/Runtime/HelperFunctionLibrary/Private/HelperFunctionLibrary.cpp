// Fill out your copyright notice in the Description page of Project Settings.

#include "HelperFunctionLibrary.h"
#include "Logging.h"

#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FHelperFunctionLibrary"

void FHelperFunctionLibrary::StartupModule()
{
}

void FHelperFunctionLibrary::ShutdownModule()
{
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FHelperFunctionLibrary, HelperFunctionLibrary);