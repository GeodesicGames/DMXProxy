// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "IDMXProxyRuntimeModule.h"

/**   */
class FDMXProxyRuntimeModule 
	: public IDMXProxyRuntimeModule
{
public:

	//~ Begin IModuleInterface implementation
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~ End IModuleInterface implementation

};
