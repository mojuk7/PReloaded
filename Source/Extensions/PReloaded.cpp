#include <Extension.h>
#include <Log.h>

#include "PReloaded.h"

#include "Parameters/Parameters.h"

using namespace std;

//
// All InitExtensions*()/FinishExtensions*() are called by engine.
//
// If at least one extension has been configured for given target,
// InitExtensions*()/FinishExtensions*() pair MUST be declared.
//

#if defined(FOCLASSIC_CLIENT)

void InitExtensionsClient()
{
	WriteLog("Initializing extensions\n");

	Extension::Map["client_parameters"] = new P::Parameters();

	Extension::RunEventDebug = true;
}

void FinishExtensionsClient()
{
	WriteLog("Finishing extensions\n");

	delete (P::Parameters*)Extension::Map["client_parameters"];
}

#elif defined(FOCLASSIC_MAPPER)

void InitExtensionsMapper()
{
	WriteLog("Initializing extensions\n");

	Extension::RunEventDebug = true;
}

void FinishExtensionsMapper()
{
	WriteLog("Finishing extensions\n");
}

#elif defined(FOCLASSIC_SERVER)

void InitExtensionsServer()
{
	WriteLog("Initializing extensions\n");

	Extension::Map["parameters"] = new P::Parameters();

	Extension::RunEventDebug = true;
}

void FinishExtensionsServer()
{
	WriteLog("Finishing extensions\n");

	delete (P::Parameters*)Extension::Map["parameters"];
}

#endif
