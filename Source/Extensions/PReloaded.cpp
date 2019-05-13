#include <Extension.h>
#include <Log.h>

#include "PReloaded.h"

using namespace std;

//
// All InitExtensions*()/FinishExtensions*() are called by engine.
//
// If at least one extension has been configured for given target,
// InitExtensions*()/FinishExtensions*() pair MUST be declared.
//

#if defined(FOCLASSIC_CLIENT)

void InitExtensionsClient() // used by engine
{
	WriteLog("Initializing extensions\n");
}

void FinishExtensionsClient() // used by engine
{
	WriteLog("Finishing extensions\n");

	delete (FOC::Client*)Extension::Map[FOC_CLIENT];
}

#elif defined(FOCLASSIC_MAPPER)

void InitExtensionsMapper() // used by engine
{
	WriteLog("Initializing extensions\n");
}

void FinishExtensionsMapper() // used by engine
{
	WriteLog("Finishing extensions\n");
}

#elif defined(FOCLASSIC_SERVER)

void InitExtensionsServer() // used by engine
{
	WriteLog("Initializing extensions\n");
}

void FinishExtensionsServer() // used by engine
{
	WriteLog("Finishing extensions\n");
}

#endif
