// Copyright Epic Games, Inc. All Rights Reserved.

#include "DMXProxyRuntimeModule.h"
#include "DMXProxySettings.h"

#if WITH_EDITOR
#include "ISettingsModule.h"
#endif

IMPLEMENT_MODULE( FDMXProxyRuntimeModule, DMXProxyRuntime );

#define LOCTEXT_NAMESPACE "DMXProxyRuntimeModule"


void FDMXProxyRuntimeModule::StartupModule()
{

#if WITH_EDITOR
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	// Register DMX Protocol global settings
	if (SettingsModule != nullptr)
	{	
		SettingsModule->RegisterSettings("Project", "Plugins", "DMX Proxy",
			LOCTEXT("ProjectSettings_Label", "DMX Proxy"),
			LOCTEXT("ProjectSettings_Description", "Configure DMX Proxy global settings"),
			GetMutableDefault<UDMXProxySettings>()
		);
	}
#endif // WITH_EDITOR

}

void FDMXProxyRuntimeModule::ShutdownModule()
{
#if WITH_EDITOR
	// Unregister DMX Proxy global settings
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "DMX Proxy");
	}
#endif // WITH_EDITOR
}

#undef LOCTEXT_NAMESPACE
