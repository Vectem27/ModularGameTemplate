#include "Modules/ModuleManager.h"

/**
 * FModularGameFrameworkGameModule
 */
class FModularGameFrameworkGameModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};


IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ModularGameFramework, "ModularGameFramework" );
