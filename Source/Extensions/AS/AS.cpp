/*
 * Shared between Utils.dll, client_parameters.dll, MapperUtils.dll
 * AngelScript related only!
 */

//#include "common.h"
#include <string>
//#include <sstream>

#include <angelscript.h>

#include <Log.h>
#include <Script.h>

#include "AS.h"

using namespace std;

namespace ScriptUtils
{

bool ParseScript(const string& script, string& module, string& func)
{
	int pos = script.find("@");
	if(pos == string::npos)
	{
		asIScriptContext* ctx = asGetActiveContext();
		if(!ctx) return false;
		asIScriptFunction* f = ctx->GetFunction();
		if(!f) return false;
		module = string(f->GetModuleName());
		func = script;
		return true;
	}
	module = script.substr(0, pos);
	func = script.substr(pos + 1);
	return true;
}

}

#ifdef FOCLASSIC_SERVER

uint GetGenericFunctionBind(const ScriptString& scriptfunc, bool temp)
{
	string module;
	string func;
	ScriptUtils::ParseScript(scriptfunc.c_std_str(), module, func);
	func = "int " + func + "(IObject@)";

	uint bind_id = Script::Bind(module.c_str(), func.c_str(), NULL, temp, false);
	if(!bind_id)
		WriteLog("Bind function fail: <%s> in module <%s>.\n", func.c_str(), module.c_str());

	return bind_id;
}
int CallGenericFunctionById(uint bind_id, void* object)
{
	if(!Script::PrepareContext(bind_id,_FUNC_,"ExtensionCall"))
	{
		WriteLog("Failed to prepare context, bind id<%d>\n", bind_id);
		return -2;
	}

	Script::SetArgObject(object);
	if(Script::RunPrepared())
		return Script::GetReturnedUInt();

	return -3;
}

class ScriptFuncBind
{
private:
	uint bind_id;

public:
	ScriptFuncBind() : bind_id(0) {}
	bool IsValid() const
	{
		return bind_id > 0;
	}

	int Call(void* object) const
	{
		if(!IsValid())
		{
			return -1;
			WriteLog("Tried to call an invalid function.\n");
		}
		return CallGenericFunctionById(bind_id, object);
	}

	bool SetFunction(const ScriptString& scriptfunc)
	{
		bind_id = GetGenericFunctionBind(scriptfunc, false);
		if(!bind_id) WriteLog("Trying to bind a nonexistent function% :%s.", scriptfunc.c_str());

		return IsValid();
	}
};

void ScriptFuncBind_Construct(void *memory)
{
	new(memory) ScriptFuncBind();
}

void ScriptFuncBind_Destruct(void *memory)
{
	((ScriptFuncBind*)memory)->~ScriptFuncBind();
}

int Global_CallGenericFunction(const ScriptString& scriptfunc, void* object)
{
	uint bind_id = GetGenericFunctionBind(scriptfunc, true);
	if(!bind_id)
		return -1;

	return CallGenericFunctionById(bind_id, object);
}

void Global_RunScript(const ScriptString& scriptfunc, int p0, int p1, int p2, const ScriptString& p3)
{
	string module;
	string func;
	ScriptUtils::ParseScript(scriptfunc.c_std_str(), module, func);
	func = "void " + func + "(Critter&,int,int,int)";

	int bind_id = Script::Bind(module.c_str(), func.c_str(), NULL, true, false );
	if(!bind_id)
	{
		WriteLog("Bind function fail: <%s> in module <%s>.\n", func.c_str(), module.c_str());
		return;
	}

	if(!Script::PrepareContext(bind_id, _FUNC_, "ExtensionCall" ))
	{
		WriteLog("Failed to prepare context: <%s> in module <%s>.\n", func.c_str(), module.c_str());
		return;
	}

	Script::SetArgUInt(p0);
	Script::SetArgUInt(p1);
	Script::SetArgUInt(p2);
	Script::SetArgObject((void*)&p3);
	Script::RunPrepared();
}

#endif // FOCLASSIC_SERVER

uint Global_GetCallstack(ScriptArray* modules, ScriptArray* names, ScriptArray* lines, ScriptArray* columns)
{
	asIScriptContext* ctx = asGetActiveContext();
	if(!ctx) return NULL;

    int                      line, column;
    const asIScriptFunction* func;
    uint                     stack_size = ctx->GetCallstackSize(), size = 0;

	vector<ScriptString*> modules_, names_;
	vector<uint> lines_, columns_;

	for(uint i = 0; i < stack_size; i++)
    {
        func = ctx->GetFunction(i);
        line = ctx->GetLineNumber(i, &column);
        if(func)
		{
			modules_.push_back(new ScriptString(func->GetModuleName()));
			names_.push_back(new ScriptString(func->GetName()));
			lines_.push_back((uint)line);
			columns_.push_back((uint)column);
			size++;
		}
    }

	if (modules)
	{
		modules->Resize(0);
		Script::AppendVectorToArrayRef(modules_, modules);
	}
	if (names)
	{
		names->Resize(0);
		Script::AppendVectorToArrayRef(names_, names);
	}
	if (lines)
	{
		lines->Resize(0);
		Script::AppendVectorToArray(lines_, lines);
	}
	if (columns)
	{
		columns->Resize(0);
		Script::AppendVectorToArray(columns_, columns);
	}

	return size;
}

bool Global_GetCurrentModule( ScriptString& moduleName )
{
	asIScriptContext* ctx = asGetActiveContext();
	if( !ctx )
		return( false );

	asIScriptFunction* function = ctx->GetFunction();
	if( !function )
		return( false );

	moduleName = function->GetModuleName();

	return( true );
}

bool Global_GetCurrentFunction( ScriptString& moduleName, ScriptString& functionName, ScriptString& declaration, bool includeNamespace = false )
{
	asIScriptContext* ctx = asGetActiveContext();
	if( !ctx )
		return( false );

	asIScriptFunction* function = ctx->GetFunction();
	if( !function )
		return( false );

	moduleName = function->GetModuleName();
	functionName = function->GetName();
	declaration = function->GetDeclaration( true, includeNamespace );

	return( true );
}

uint Global_GetEnumList_module( ScriptString& moduleName, ScriptArray* names )
{
	names->Resize(0);

	asIScriptModule* module = asGetActiveContext()->GetEngine()->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( 0 );

	uint count = module->GetEnumCount();
	if( count <= 0 )
		return( count );

	vector<ScriptString*> names_;
	for( uint e=0; e<count; e++ )
	{
		int enumTypeId;
		const char* enumName = module->GetEnumByIndex( e, &enumTypeId );
		names_.push_back(new ScriptString(enumName));
	}



	return( count );
}

uint Global_GetEnumList( ScriptArray* names )
{
	ScriptString moduleName;
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_GetEnumList_module( moduleName, names ));
	}

	return( 0 );
}

uint Global_GetEnum_module( ScriptString& moduleName, ScriptString& enumName, ScriptArray* names, ScriptArray* values )
{
	asIScriptModule* module = asGetActiveContext()->GetEngine()->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( 0 );

	for( int e=0, elen=module->GetEnumCount(); e<elen; e++ )
	{
		int enumTypeId;
		const char* eName = module->GetEnumByIndex( e, &enumTypeId );

		if (!strcmp(enumName.c_str(), eName))
		{
			int count = module->GetEnumValueCount(enumTypeId);
			if (count <= 0)
				return(0);

			vector<ScriptString*> names_;
			vector<int> values_;

			for (int v = 0; v < count; v++)
			{
				int outValue;
				const char* valueName = module->GetEnumValueByIndex(enumTypeId, v, &outValue);

				names_.push_back(new ScriptString(valueName));
				values_.push_back(outValue);
			}

			if( names)
			{
				names->Resize(0);
				Script::AppendVectorToArrayRef(names_, names);
			}

			if( values )
			{
				values->Resize(0);
				Script::AppendVectorToArray(values_, values);
			}

			return( count );
		}
	}

	return( 0 );
}

uint Global_GetEnum( ScriptString& enumName, ScriptArray* names, ScriptArray* values )
{
	ScriptString moduleName;
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_GetEnum_module( moduleName, enumName, names, values ));
	}

	return( 0 );
}

bool Global_EnumContains_module( ScriptString& moduleName, ScriptString& enumName, int value )
{
	asIScriptModule* module = asGetActiveContext()->GetEngine()->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( false );

	for( int e=0, elen=module->GetEnumCount(); e<elen; e++ )
	{
		int enumTypeId;
		const char* eName = module->GetEnumByIndex( e, &enumTypeId );

		if( !strcmp(enumName.c_str(), eName) )
		{
			int count = module->GetEnumValueCount( enumTypeId );
			if( count <= 0 )
				return( false );

			for( int v=0; v<count; v++ )
			{
				int outValue;
				module->GetEnumValueByIndex( enumTypeId, v, &outValue );

				if( outValue == value )
					return( true );
			}
		}
	}

	return( false );
}

bool Global_EnumContains( ScriptString& enumName, int value )
{
	ScriptString moduleName;
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_EnumContains_module( moduleName, enumName, value ));
	}

	return( false );
}

uint Global_GetEnumValueCount_module( ScriptString& moduleName, ScriptString& enumName )
{
	asIScriptModule* module = asGetActiveContext()->GetEngine()->GetModule( moduleName.c_str(), asGM_ONLY_IF_EXISTS );
	if( !module )
		return( 0 );

	for( int e=0, elen=module->GetEnumCount(); e<elen; e++ )
	{
		int enumTypeId;
		const char* eName = module->GetEnumByIndex( e, &enumTypeId );

		if( !strcmp(enumName.c_str(), eName) )
		{
			int count = module->GetEnumValueCount( enumTypeId );
			if( count <= 0 )
				return( 0 );
			return( count );
		}
	}

	return( 0 );
}

uint Global_GetEnumValueCount( ScriptString& enumName )
{
	ScriptString moduleName;
	if( Global_GetCurrentModule( moduleName ))
	{
		return( Global_GetEnumValueCount_module( moduleName, enumName ));
	}

	return( 0 );
}

void RegisterAngelScriptExtensions(asIScriptEngine* engine)
{
	const char* fail = "Failed to register";

	if(engine->RegisterGlobalFunction("uint GetCallstack(string@[]@+ modules, string@[]@+ names, uint[]@+ lines, uint[]@+ columns)", asFUNCTION(Global_GetCallstack), asCALL_CDECL) < 0)
		WriteLog("%s GetCallstack()\n", fail);
	if(engine->RegisterGlobalFunction("bool GetCurrentModule(string& moduleName)", asFUNCTION(Global_GetCurrentModule), asCALL_CDECL) < 0)
		WriteLog("%s GetCurrentModule()\n", fail);
	if(engine->RegisterGlobalFunction("bool GetCurrentFunction(string& moduleName, string& functionName, string& declaration, bool includeNamespace = false)", asFUNCTION(Global_GetCurrentFunction), asCALL_CDECL) < 0)
		WriteLog("%s GetCurrentFunction()\n", fail);
	if(engine->RegisterGlobalFunction("uint GetEnum(string& enumName, string@[]@+ names, int[]@+ values)", asFUNCTION(Global_GetEnum), asCALL_CDECL) < 0)
		WriteLog("%s GetEnum()\n", fail);
	if(engine->RegisterGlobalFunction("uint GetEnum(string& moduleName, string& enumName, string@[]@+ names, int[]@+ values)", asFUNCTION(Global_GetEnum_module), asCALL_CDECL) < 0)
		WriteLog("%s GetEnum(moduleName)\n", fail);
	if(engine->RegisterGlobalFunction("uint GetEnumList(string@[]@+ names)", asFUNCTION(Global_GetEnumList), asCALL_CDECL) < 0)
		WriteLog("%s GetEnumList()\n", fail);
	if(engine->RegisterGlobalFunction("uint GetEnumList(string& moduleName, string@[]@+ names)", asFUNCTION(Global_GetEnumList_module), asCALL_CDECL) < 0)
		WriteLog("%s GetEnumList(moduleName)\n", fail);
	if(engine->RegisterGlobalFunction("bool EnumContains(string& enumName, int value)", asFUNCTION(Global_EnumContains), asCALL_CDECL) < 0)
		WriteLog("%s EnumContains()\n", fail);
	if(engine->RegisterGlobalFunction("bool EnumContains(string& moduleName, string& enumName, int value)", asFUNCTION(Global_EnumContains_module), asCALL_CDECL) < 0)
		WriteLog("%s EnumContains(moduleName)\n", fail);
	if(engine->RegisterGlobalFunction("uint GetEnumValueCount(string& enumName)", asFUNCTION(Global_GetEnumValueCount), asCALL_CDECL) < 0 )
		WriteLog("%s GetEnumValueCount()\n", fail);
	if(engine->RegisterGlobalFunction("uint GetEnumValueCount(string& moduleName, string& enumName)",asFUNCTION(Global_GetEnumValueCount_module), asCALL_CDECL) < 0 )
		WriteLog("%s GetEnumValueCount(moduleName)\n", fail);

#ifdef FOCLASSIC_SERVER
	if(engine->RegisterInterface("IObject") < 0)
		WriteLog("%s IObject\n", fail);
	if(engine->RegisterGlobalFunction("int CallGenericFunction(const string& funcname, IObject@+ object)",asFUNCTION(Global_CallGenericFunction), asCALL_CDECL) < 0 )
		WriteLog("%s CallGenericFunction()\n", fail);
	if(engine->RegisterGlobalFunction("void RunScript(string& funcname, int p0, int p1, int p2, const string& p3)",asFUNCTION(Global_RunScript), asCALL_CDECL) < 0 )
		WriteLog("%s RunScript()\n", fail);

	if(engine->RegisterObjectType("FuncBind", sizeof(ScriptFuncBind), asOBJ_VALUE) < 0)
		WriteLog("%s FuncBind\n", fail);
	if(engine->RegisterObjectBehaviour("FuncBind", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(ScriptFuncBind_Construct), asCALL_CDECL_OBJFIRST) < 0)
		WriteLog("%s FuncBind ctor\n", fail);
	if(engine->RegisterObjectBehaviour("FuncBind", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(ScriptFuncBind_Destruct), asCALL_CDECL_OBJFIRST) < 0)
		WriteLog("%s FuncBind dtor\n", fail);
	if(engine->RegisterObjectMethod("FuncBind", "bool IsValid() const", asMETHOD(ScriptFuncBind, IsValid), asCALL_THISCALL) < 0)
		WriteLog("%s FuncBind::IsValid()\n", fail);
	if(engine->RegisterObjectMethod("FuncBind", "int Call(IObject@+ object) const", asMETHOD(ScriptFuncBind, Call), asCALL_THISCALL) < 0)
		WriteLog("%s FuncBind::Call(...)\n", fail);
	if(engine->RegisterObjectMethod("FuncBind", "bool SetFunction(string& funcname)", asMETHOD(ScriptFuncBind, SetFunction), asCALL_THISCALL) < 0)
		WriteLog("%s FuncBind::SetFunction(...)\n", fail);
#endif // FOCLASSIC_SERVER
}

P::AS::AS() : Extension()
{}

void P::AS::Event(const uint& id)
{
	if (id == ExtensionEvent::SCRIPT_LOAD_MODULES_START)
	{
		RegisterAngelScriptExtensions(Script::GetEngine());
	}
}
