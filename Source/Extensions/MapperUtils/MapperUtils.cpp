#include <CritterCl.h>
#include <HexManager.h>
#include <Mapper.h>
#include <ProtoMap.h>

#include "MapperUtils.h"
#include "PReloaded.h"

#include "_mapper_defines.fos"

using namespace std;

struct MapObjectEx : MapObject
{
	struct _RunTime
	{
		ProtoMap* FromMap;
		uint MapObjId;
		char PicMapName[64];
		char PicInvName[64];
		long RefCounter;
	} RunTime;
};

inline Field* GetField(uint16 hx, uint16 hy)
{
	if( FOMapper::Self->HexMngr.IsMapLoaded() )
		return &FOMapper::Self->HexMngr.GetField(hx, hy);

	return nullptr;
}

CritterCl* GetCritterCl(MapObject& mobj)
{
	MapObjectEx& mobj_= (MapObjectEx&)(mobj);
	if(mobj_.MapObjType!=MAP_OBJECT_CRITTER) 
		return NULL;
	Field* field=GetField(mobj_.MapX,mobj_.MapY);
	if(!field) 
		return NULL;
	if(field->Crit && mobj_.RunTime.MapObjId==field->Crit->Id)
		return field->Crit;
	else
	{
		for (auto it = field->DeadCrits.begin(), end = field->DeadCrits.end(); it != end; ++it)
		{
			if (mobj_.RunTime.MapObjId == (*it)->Id)
				return *it;
		}
	}

	return NULL;
}

static void MapObject_CritterCl_SetParam(MapObject& mobj, uint16 param, int val)
{
	CritterCl* cr=GetCritterCl(mobj);
	if(!cr) return;

	cr->Params[param] = val;
}

static int MapObject_CritterCl_GetParam(MapObject& mobj, uint16 param)
{
	CritterCl* cr=GetCritterCl(mobj);
	if(!cr) return 0;
	return cr->Params[param];
}

static void MapObject_CritterCl_SetBaseType(MapObject& mobj, uint crtype)
{
	CritterCl* cr=GetCritterCl(mobj);
	if(!cr) return;

	cr->BaseType = crtype;

	// todo: alias ctype
}

static uint MapObject_CritterCl_GetBaseType(MapObject& mobj)
{
	CritterCl* cr=GetCritterCl(mobj);
	if(!cr) return 0;
	return cr->BaseType;
	// todo: alias ctype
}

P::MapperUtils::MapperUtils() : Extension()
{}

size_t P::MapperUtils::GetFunctionAddress(const string& name)
{
	GET_ADDRESS(MapObject_CritterCl_GetParam);
	GET_ADDRESS(MapObject_CritterCl_GetBaseType);
	GET_ADDRESS(MapObject_CritterCl_SetBaseType);

	return 0;
}
