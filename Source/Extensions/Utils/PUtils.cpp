//#include "common.h"
#include <string>
#include <stdio.h>

#include <angelscript.h>

#include <Critter.h>
#include <Map.h>
#include <ProtoMap.h>
#include <Script.h>

#include "ExtEvents.h"
#include "ProtoMapEx.h"
#include "PReloaded.h"
#include "PUtils.h"

using namespace std;

// just float<->int converter, for serializator
static float IntToFloat(int value)
{
	return *(float*)(&value);
}

static int FloatToInt(float value)
{
	return *(int*)(&value);
}

static unsigned long long getticks()
{
    LARGE_INTEGER t;
    unsigned long long ret=0;
    QueryPerformanceCounter(&t);
    ret=t.HighPart;
    ret<<=32;
    ret+=t.LowPart;
	return ret;
}

static int Critter_GetScore(Critter& cr, uint8 score)
{
    return cr.Data.Scores[score];
}

static uint Scenery_GetParam(MapObject& scenery, uint8 num)
{
    return scenery.MScenery.Param[num];
}

static bool __cdecl _AllowSlotHand1(uint8 slot, Item& item, Critter& fromCr, Critter& toCr)
{
	//return toCr.Params[PE_AWARENESS]!=0;
	return true;
}

static int _AllowParameterIfAwareness(uint parameter, Critter& fromCr, Critter& toCr)
{
	//return (toCr.Params[PE_AWARENESS]>0)?fromCr.Params[parameter]:0;
	return fromCr.GetRawParam(parameter);
}

static int _AllowParameterIfFieldMedic(uint parameter, Critter& fromCr, Critter& toCr)
{
	return (toCr.GetRawParam(PE_FIELD_MEDIC) > 0) ? fromCr.GetRawParam(parameter) : 0;
}

static int _AllowParameterIfEqualNZ(uint parameter, Critter& fromCr, Critter& toCr)
{
	if(fromCr.GetRawParam(parameter)>0 && (fromCr.GetRawParam(parameter)==toCr.GetRawParam(parameter))) return fromCr.GetRawParam(parameter);
	return 0;
}

static int String_ReplaceText(ScriptString& str, ScriptString& text, ScriptString& replacement)
{
	int size=text.length();
	if(!size) return 0;
	int size_rep=replacement.length();
	string st=str.c_std_str();
	int pos=st.find(text.c_std_str(),0);
	int num=0;
	while(pos>=0)
	{
		st.replace(pos,size,replacement.c_std_str());
		pos=st.find(text.c_std_str(),pos+size_rep);
		num++;
	}
	str=st;
	return num;
}

static void String_ParseFloat(ScriptString& str, float val, uint8 precision)
{
	static char prec[32];
	sprintf(prec,"%%.%df",precision);
	static char buf[1024];
	sprintf(buf,prec,val);
	str.assign(buf);
}

static void Map_ProtoName(Map& map, ScriptString& result)
{
	result=map.Proto->GetName();
}

static void Location_ProtoName(Location& location, ScriptString& result)
{
	result=location.Proto->Name;
}

// todo: remove and replace with fonline-provided callers
static void RunScript(const ScriptString& scriptfunc, int p0, int p1, int p2, const ScriptString& p3)
{
	int delim = scriptfunc.c_std_str().find("@");
	if(delim == -1) return;
	string modulename = scriptfunc.c_std_str().substr(0, delim);
	string funcname = scriptfunc.c_std_str().substr(delim + 1);

	asIScriptEngine* engine = Script::GetEngine();

	asIScriptModule* module = engine->GetModule(modulename.c_str());
	if(!module) return;
	asIScriptFunction* script_func = module->GetFunctionByName(funcname.c_str());
	if(!script_func) return;

	asIScriptContext* ctx = engine->CreateContext();
	ctx->Prepare(script_func);
	ctx->SetArgDWord(0, p0);
	ctx->SetArgDWord(1, p1);
	ctx->SetArgDWord(2, p2);
	ctx->SetArgObject(3, (void*)&p3);
	ctx->Execute();
	
	ctx->Release();
}

static void Critter_GetLexems( Critter& cr, ScriptString& output )
{
	output.assign(cr.Data.Lexems);
	return;
}

static void Item_GetLexems( Item& it, ScriptString& output )
{
	if(!it.PLexems)
	{
		output.assign("");
		return;
	}
	output.assign(it.PLexems);
	return;
}

P::Utils::Utils() : Extension()
{}

size_t P::Utils::GetFunctionAddress(const string& name)
{
	GET_ADDRESS(String_ParseFloat);
	GET_ADDRESS(FloatToInt);
	GET_ADDRESS(IntToFloat);
	GET_ADDRESS(String_ReplaceText);

	GET_ADDRESS_NS(Critter_ClearExtEvents);
	GET_ADDRESS_NS(Critter_SetEventExt);
	GET_ADDRESS_NS(Critter_EventExtMapIn);
	GET_ADDRESS_NS(Critter_EventExtMapOut);

	GET_ADDRESS_NS(Location_IsEncounter);
	GET_ADDRESS_NS(Location_IsTown);
	GET_ADDRESS_NS(Location_IsCityEncounter);
	GET_ADDRESS_NS(Location_IsInstancedQuest);
	GET_ADDRESS_NS(Location_IsBase);
	GET_ADDRESS_NS(Location_IsReplication);
	GET_ADDRESS_NS(Location_IsTCTown);
	GET_ADDRESS_NS(Location_IsTent);
	GET_ADDRESS_NS(Location_IsMine);
	GET_ADDRESS_NS(Location_IsCave);
	GET_ADDRESS_NS(Location_IsGuarded);
	GET_ADDRESS_NS(Location_IsPublic);

	GET_ADDRESS_NS(Map_FindNearestGrid);
	GET_ADDRESS_NS(Map_FindNearestGridEntire);
	GET_ADDRESS_NS(Map_FindRandomGridEntire);
	GET_ADDRESS_NS(Map_FindNearestGridApprox);
	GET_ADDRESS_NS(Map_IsGrid);
	GET_ADDRESS_NS(Map_GetGridDirection);

	GET_ADDRESS(Map_ProtoName);
	GET_ADDRESS(Location_ProtoName);

	GET_ADDRESS(Critter_GetLexems);
	GET_ADDRESS(Item_GetLexems);

	return 0;
}

void P::Utils::Event(const uint& id)
{
	if (id == ExtensionEvent::SCRIPT_INIT)
	{
		InitLocationEx();
	}
	else if (id == ExtensionEvent::FINISH)
	{
		FinishMapLocationEx();
	}
}