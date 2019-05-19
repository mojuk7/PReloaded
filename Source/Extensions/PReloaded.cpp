#include <Extension.h>
#include <Log.h>

#include "PReloaded.h"

#include "AS/AS.h"
#include "Parameters/Parameters.h"

#if defined(FOCLASSIC_SERVER)
#include "Utils/PUtils.h"
#endif

using namespace std;

//
// Following functions are called by engine.
//   InitExtensionsClient(), FinishExtensionsClient(),
//   InitExtensionsMapper(), FinishExtensionsMapper(),
//   InitExtensionsServer(), FinishExtensionsServer(),
//
// If at least one extension has been configured for given target,
// InitExtensions*()/FinishExtensions*() pair MUST be declared.
//

static void InitExtensionsGeneric() // all targets
{
	Extension::Map["AS"] = new P::AS();
}

static void FinishExtensionsGeneric() // all targets
{
	delete (P::AS*)Extension::Map["AS"];
}

#if defined(FOCLASSIC_CLIENT)

void InitExtensionsClient()
{
	WriteLog("Initializing extensions\n");

	Extension::Map["client_parameters"] = new P::Parameters();

	InitExtensionsGeneric();
	Extension::RunEventDebug = true;
}

void FinishExtensionsClient()
{
	WriteLog("Finishing extensions\n");

	delete (P::Parameters*)Extension::Map["client_parameters"];

	FinishExtensionsGeneric();
}

#elif defined(FOCLASSIC_MAPPER)

void InitExtensionsMapper()
{
	WriteLog("Initializing extensions\n");

	InitExtensionsGeneric();
	Extension::RunEventDebug = true;
}

void FinishExtensionsMapper()
{
	WriteLog("Finishing extensions\n");

	FinishExtensionsGeneric();
}

#elif defined(FOCLASSIC_SERVER)

void InitExtensionsServer()
{
	WriteLog("Initializing extensions\n");

	Extension::Map["parameters"] = new P::Parameters();
	Extension::Map["Utils"] = new P::Utils();

	InitExtensionsGeneric();
	Extension::RunEventDebug = true;
}

void FinishExtensionsServer()
{
	WriteLog("Finishing extensions\n");

	delete (P::Parameters*)Extension::Map["parameters"];
	delete (P::Utils*)Extension::Map["Utils"];

	FinishExtensionsGeneric();
}

#endif
