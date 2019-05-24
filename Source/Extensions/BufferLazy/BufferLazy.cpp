/*
 *
 * WHINE TEAM
 * We Had Ini, Now Engine
 *
 */

#include <string>

#include <angelscript.h>
#include <scriptstring.h>

#include <Log.h>
#include <Script.h>

#if defined(FOCLASSIC_SERVER)
#include <Critter.h>
#endif

#include "BufferLazy.h"
#include "PReloaded.h"

using namespace std;

// There's a blood on this code.

bool ParseLocalScriptName( const ScriptString& scriptfunc, string& module, string& function )
{
	int pos = scriptfunc.c_std_str().find_first_of( "@" );

	if( pos > 0 )
	{
		string moduleName = scriptfunc.c_std_str().substr( 0, pos );
		string functionDecl = "void ";
		functionDecl += scriptfunc.c_std_str().substr( pos + 1 );
#if defined(FOCLASSIC_CLIENT)
		functionDecl += "(int,int,int,string@,int[]@)";
#elif defined(FOCLASSIC_SERVER)
		functionDecl += "(Critter&,int,int,int,string@,int[]@)";
#endif

		module = moduleName;
		function = functionDecl;

		return( true );
	}
	else
		return( false );
}

static bool Global_IsLocalScript( const ScriptString& scriptfunc )
{
	string module;
	string function;

	if( ParseLocalScriptName( scriptfunc, module, function ))
	{
		asIScriptModule* as_module = asGetActiveContext()->GetEngine()->GetModule( module.c_str(), asGM_ONLY_IF_EXISTS );
		if( !as_module )
			return( false );

		asIScriptFunction* as_function = as_module->GetFunctionByDecl( function.c_str() );
		if( as_function )
			return( true );
	}

	return( false );
}

#if defined(FOCLASSIC_CLIENT)
static void Global_RunLocalScript( const ScriptString& scriptfunc, int p0, int p1, int p2, const ScriptString* p3, ScriptArray* p4 )
#elif defined(FOCLASSIC_SERVER)
static void Critter_RunLocalScript( Critter& cr, const ScriptString& scriptfunc, int p0, int p1, int p2, const ScriptString* p3, ScriptArray* p4 )
#endif
{
	if( Global_IsLocalScript( scriptfunc ))
	{
		string module, function;

		if( ParseLocalScriptName( scriptfunc, module, function ))
		{
			int bindId = Script::Bind( module.c_str(), function.c_str(), NULL, true, false );
			if( bindId > 0 && Script::PrepareContext( bindId, _FUNC_, "ExtensionCall" ))
			{
				#if defined(FOCLASSIC_SERVER)
				Script::SetArgObject( &cr );
				#endif
				Script::SetArgUInt( p0 );
				Script::SetArgUInt( p1 );
				Script::SetArgUInt( p2 );
				Script::SetArgObject( (void*)p3 );
				Script::SetArgObject( p4 );
				Script::RunPrepared();
			}
			else
			{
				WriteLog( "RunLocalScript : cannot bind module<%s> function<%s>\n",
					module.c_str(), function.c_str() );
			}
		}
		else
		{
			WriteLog( "RunLocalScript : invalid function name<%s>\n", scriptfunc.c_str() );
		}
	}
	else
	{
		WriteLog( "RunLocalScript : function<%s> not found\n", scriptfunc.c_str() );
	}
}

P::BufferLazy::BufferLazy() : Extension()
{}

size_t P::BufferLazy::GetFunctionAddress(const std::string & name)
{
	GET_ADDRESS(Global_IsLocalScript);

	#if defined(FOCLASSIC_CLIENT)
	GET_ADDRESS(Global_RunLocalScript);
	#elif defined(FOCLASSIC_SERVER)
	GET_ADDRESS(Critter_RunLocalScript);
	#endif

	return 0;
}
