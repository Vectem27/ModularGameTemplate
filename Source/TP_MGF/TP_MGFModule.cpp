#include "Modules/ModuleManager.h"

/**
 * FTP_MGFModule
 */
class FTP_MGFModule : public FDefaultGameModuleImpl
{
	virtual void StartupModule() override
	{
	}

	virtual void ShutdownModule() override
	{
	}
};


IMPLEMENT_PRIMARY_GAME_MODULE(FTP_MGFModule, TP_MGF, "TP_MGF" );
