//#include "common.h"
#include <algorithm>
#include <map>
#include <string>
#include <vector>
#include <stdio.h>

#include <Ini.h>
#include <Log.h>
#include <Map.h>
#include <ProtoMap.h>
#include <Random.h>
#include <Text.h>

#include <Extension.Helper.h>

#include "ProtoMapEx.h"

using namespace std;

#define BUFFER_SIZE				(1024)
#define APPROX_GRID_THRESHOLD	(5)

class LocationEx
{
public:
	bool IsEncounter;
	bool IsTown;
	bool IsCityEncounter;
	bool IsInstancedQuest;
	bool IsBase;
	bool IsReplication;
	bool IsTCTown;
	bool IsTent;
	bool IsMine;
	bool IsCave;
	bool IsGuarded;
	bool IsPublic;

	LocationEx();
	~LocationEx();
};

LocationEx::LocationEx() :
	IsEncounter(false),
	IsTown(false),
	IsCityEncounter(false),
	IsInstancedQuest(false),
	IsBase(false),
	IsReplication(false),
	IsTCTown(false),
	IsTent(false),
	IsMine(false),
	IsCave(false),
	IsGuarded(false),
	IsPublic(false)
{}

LocationEx::~LocationEx() {}

struct GridCarrier
{
	vector<MapObjectPtrVec> ToWM;
	vector<MapObjectPtrVec> ToMap;
};

map<uint, LocationEx> LocationExMap;
typedef map<uint16, GridCarrier> GridMap;
GridMap GridInfo;

void P::InitLocationEx()
{
	Ini locations;

	if (!locations.LoadFile("maps/Locations.cfg"))
	{
		WriteLog("WARNING : Cannot load maps/Locations.cfg");
		return;
	}

	StrVec sections;
	locations.GetSections(sections);
	for (auto it = sections.begin(), end = sections.end(); it != end; ++it)
	{
		// make sure prefix is valid
		if (it->size() < 6)
			continue;
		if (it->substr(0, 5) != "Area ")
			continue;

		// make sure pid is valid
		const char* cpid = Str::Duplicate(it->substr(5).c_str());
		char*       cend;
		long        l;
		errno = 0;
		l = strtol(cpid, &cend, 10);
		if (((errno == ERANGE && l == LONG_MAX) || l > MAX_PROTO_LOCATIONS) ||
			((errno == ERANGE && l == LONG_MIN) || l < 0) ||
			(*cpid == '\0' || *cend != '\0')) 
			continue;

		// add strings here
		LocationExMap[l].IsEncounter = locations.GetBool(*it, "@IsEncounter", false);
		LocationExMap[l].IsTown = locations.GetBool(*it, "@IsTown", false);
		LocationExMap[l].IsCityEncounter = locations.GetBool(*it, "@IsCityEncounter", false);
		LocationExMap[l].IsInstancedQuest = locations.GetBool(*it, "@IsInstancedQuest", false);
		LocationExMap[l].IsBase = locations.GetBool(*it, "@IsBase", false);
		LocationExMap[l].IsReplication = locations.GetBool(*it, "@IsReplication", false);
		LocationExMap[l].IsTCTown = locations.GetBool(*it, "@IsTCTown", false);
		LocationExMap[l].IsTent = locations.GetBool(*it, "@IsTent", false);
		LocationExMap[l].IsCave = locations.GetBool(*it, "@IsCave", false);
		LocationExMap[l].IsMine = locations.GetBool(*it, "@IsMine", false);
		LocationExMap[l].IsGuarded = locations.GetBool(*it, "@IsGuarded", false);
		LocationExMap[l].IsPublic = locations.GetBool(*it, "@IsPublic", false);
	}

	// debug :P

	uint IsEncounter = 0, IsTown = 0, IsCityEncounter = 0, IsInstancedQuest = 0, IsBase = 0, IsReplication = 0, IsTCTown = 0, IsTent = 0, IsCave = 0, IsMine = 0, IsGuarded = 0, IsPublic = 0;
	for_each(LocationExMap.begin(), LocationExMap.end(), [&](pair<uint16, LocationEx> loc)
	{
		IsEncounter += loc.second.IsEncounter ? 1 : 0;
		IsTown += loc.second.IsTown ? 1 : 0;
		IsCityEncounter += loc.second.IsCityEncounter ? 1 : 0;
		IsInstancedQuest += loc.second.IsInstancedQuest ? 1 : 0;
		IsBase += loc.second.IsBase ? 1 : 0;
		IsReplication += loc.second.IsReplication ? 1 : 0;
		IsTCTown += loc.second.IsTCTown ? 1 : 0;
		IsTent += loc.second.IsTent ? 1 : 0;
		IsCave += loc.second.IsCave ? 1 : 0;
		IsMine += loc.second.IsMine ? 1 : 0;
		IsGuarded += loc.second.IsGuarded ? 1 : 0;
		IsPublic += loc.second.IsPublic ? 1 : 0;
	});

	WriteLogF( _FUNC_, "\n");
	#define DLOCATIONEX(var)  WriteLog("  %s=%u\n", #var, var)
	DLOCATIONEX(IsEncounter);
	DLOCATIONEX(IsTown);
	DLOCATIONEX(IsCityEncounter);
	DLOCATIONEX(IsInstancedQuest);
	DLOCATIONEX(IsBase);
	DLOCATIONEX(IsReplication);
	DLOCATIONEX(IsTCTown);
	DLOCATIONEX(IsTent);
	DLOCATIONEX(IsCave);
	DLOCATIONEX(IsMine);
	DLOCATIONEX(IsGuarded);
	DLOCATIONEX(IsPublic);
}

void P::FinishMapLocationEx()
{
	LocationExMap.clear();
	GridInfo.clear();
}

bool P::Location_IsEncounter(Location& loc) { return LocationExMap[loc.Data.LocPid].IsEncounter; }
bool P::Location_IsTown(Location& loc) { return LocationExMap[loc.Data.LocPid].IsTown; }
bool P::Location_IsCityEncounter(Location& loc) { return LocationExMap[loc.Data.LocPid].IsCityEncounter; }
bool P::Location_IsInstancedQuest(Location& loc) { return LocationExMap[loc.Data.LocPid].IsInstancedQuest; }
bool P::Location_IsBase(Location& loc) { return LocationExMap[loc.Data.LocPid].IsBase; }
bool P::Location_IsReplication(Location& loc) { return LocationExMap[loc.Data.LocPid].IsReplication; }
bool P::Location_IsTCTown(Location& loc) { return LocationExMap[loc.Data.LocPid].IsTCTown; }
bool P::Location_IsTent(Location& loc) { return LocationExMap[loc.Data.LocPid].IsTent; }
bool P::Location_IsMine(Location& loc) { return LocationExMap[loc.Data.LocPid].IsMine; }
bool P::Location_IsCave(Location& loc) { return LocationExMap[loc.Data.LocPid].IsCave; }
bool P::Location_IsGuarded(Location& loc) { return LocationExMap[loc.Data.LocPid].IsGuarded; }
bool P::Location_IsPublic(Location& loc) { return LocationExMap[loc.Data.LocPid].IsPublic; }

MapObjectPtrVec* FindNearestGroup(vector<MapObjectPtrVec>& groups, uint16 hx, uint16 hy)
{
	if(groups.empty()) return NULL;

	auto cur_group = groups.begin();
	auto closest = cur_group;
	uint16 closest_dist = GetDistantion(hx, hy, cur_group->front()->MapX, cur_group->front()->MapY);
	if(!closest_dist) return closest._Ptr; //! _Ptr is implementation-specific
	for(++cur_group; cur_group != groups.end(); ++cur_group)
	{
		uint16 dist = GetDistantion(hx, hy, cur_group->front()->MapX, cur_group->front()->MapY);
		if(dist < closest_dist)
		{
			closest_dist = dist;
			closest = cur_group;
			if(!closest_dist) break; // nothing can be closer
		}
	}
	return closest._Ptr; //! _Ptr is implementation-specific
}

MapObject* FindNearestObject(const MapObjectPtrVec& vec, uint16 hx, uint16 hy, bool withPid = false, uint16 toMapPid = 0, bool withEntire = false, uint toEntire = 0)
{
	if(vec.empty()) return NULL;
	auto closest = vec.begin();
	auto curr = closest;
	uint16 min_dist = uint16(-1);
	// old version; leads to bugs if withPid/withEntire is true
	// left for historical reasons
	// uint16 min_dist = GetDistantion(hx, hy, (*closest)->MapX, (*closest)->MapY);
	// if(!min_dist) return *closest;
	for(++curr; curr != vec.end(); ++curr)
	{
		MapObject* obj = *curr;
		if(withPid && obj->MScenery.ToMapPid != toMapPid) continue;
		if(withEntire && obj->MScenery.ToEntire != toEntire) continue;

		uint16 dist = GetDistantion(hx, hy, obj->MapX, obj->MapY);
		if(dist < min_dist)
		{
			if(!dist) return *curr;
			min_dist = dist;
			closest = curr;
		}
	}
	return *closest;
}

MapObject* FindRandomObject(const MapObjectPtrVec& vec, uint16 hx, uint16 hy, bool withPid = false, uint16 toMapPid = 0, bool withEntire = false, uint toEntire = 0)
{
	if(vec.empty()) return NULL;
	MapObjectPtrVec objects;
	for(auto curr = vec.begin(); curr != vec.end(); ++curr)
	{
		MapObject* obj = *curr;
		if(withPid && obj->MScenery.ToMapPid != toMapPid) continue;
		if(withEntire && obj->MScenery.ToEntire != toEntire) continue;

		objects.push_back(obj);
	}
	if(objects.empty()) return NULL;
	return(objects.at(Random(0,objects.size()-1)));
}

GridCarrier& ProcessCarrier(ProtoMap* proto)
{
	GridCarrier carrier;

	// first pass: create initial grids
	for(auto it = proto->GridsVec.begin(), end = proto->GridsVec.end(); it != end; ++it)
	{
		MapObject* obj = *it;
		uint16 hx = obj->MapX;
		uint16 hy = obj->MapY;

		vector<MapObjectPtrVec>& groups = obj->MScenery.ToMapPid ? carrier.ToMap : carrier.ToWM;

		// is it closer than threshold value to some previously chosen initial?
		auto comparator = [&](const MapObjectPtrVec& group) -> bool
		{
			MapObject* o = group.front();
			return GetDistantion(hx, hy, o->MapX, o->MapY) < APPROX_GRID_THRESHOLD;
		};

		bool is_close = find_if(groups.begin(), groups.end(), comparator) != groups.end();
		if(is_close) continue;

		// make a new initial and its group
		groups.push_back(MapObjectPtrVec());
		groups.back().push_back(obj);
	}

	// second pass: assign grids to initials
	for(auto it = proto->GridsVec.begin(), end = proto->GridsVec.end(); it != end; ++it)
	{
		MapObject* obj = *it;
		uint16 hx = obj->MapX;
		uint16 hy = obj->MapY;

		vector<MapObjectPtrVec>& groups = obj->MScenery.ToMapPid ? carrier.ToMap : carrier.ToWM;
		MapObjectPtrVec* closest = FindNearestGroup(groups, hx, hy);
		uint16 closest_dist = GetDistantion(hx, hy, closest->front()->MapX, closest->front()->MapY);
		if(!closest_dist) continue; // is initial
		closest->push_back(obj);
	}

	GridInfo[proto->GetPid()] = carrier;
	return GridInfo[proto->GetPid()];
}

GridCarrier& GetCarrier(ProtoMap* proto)
{
	GridMap::iterator it = GridInfo.find(proto->GetPid());
	if(it == GridInfo.end()) return ProcessCarrier(proto);
	return it->second;
}

bool P::Map_FindNearestGridRough(Map& imap, uint16& hx, uint16& hy, bool toWM)
{
	GridCarrier& carrier = GetCarrier(imap.Proto);

	vector<MapObjectPtrVec>& groups = toWM ? carrier.ToWM : carrier.ToMap;
	MapObjectPtrVec* vec = FindNearestGroup(groups, hx, hy);
	if(!vec) return false;
	hx = vec->front()->MapX;
	hy = vec->front()->MapY;
	return true;
}

bool P::Map_FindNearestGridApprox(Map& imap, uint16& hx, uint16& hy, bool toWM)
{
	GridCarrier& carrier = GetCarrier(imap.Proto);

	vector<MapObjectPtrVec>& groups = toWM ? carrier.ToWM : carrier.ToMap;
	MapObjectPtrVec* vec = FindNearestGroup(groups, hx, hy);
	if(!vec) return false;

	MapObject* obj = FindNearestObject(*vec, hx, hy);
	hx = obj->MapX;
	hy = obj->MapY;
	return true;
}

bool P::Map_FindNearestGrid(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid)
{
	MapObject* obj = FindNearestObject(imap.Proto->GridsVec, hx, hy, true, toMapPid);
	if(!obj) return false;
	hx=obj->MapX;
	hy=obj->MapY;
	return true;
}

bool P::Map_FindNearestGridEntire(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid, uint toEntire)
{
	MapObject* obj = FindNearestObject(imap.Proto->GridsVec, hx, hy, true, toMapPid, true, toEntire);
	if(!obj) return false;
	hx=obj->MapX;
	hy=obj->MapY;
	return true;
}

bool P::Map_FindRandomGridEntire(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid, uint toEntire)
{
	MapObject* obj = FindRandomObject(imap.Proto->GridsVec, hx, hy, true, toMapPid, true, toEntire);
	if(!obj) return(false);
	hx=obj->MapX;
	hy=obj->MapY;
	return(true);
}
/*
static void Map_GetGrids( Map& map, ScriptArray& result )
{
	for( int g=0, gMax=map.Proto->GridsVec.size(); g<gMax; g++ )
	{
		result.InsertLast( (uint16*)map.Proto->GridsVec[g]->MapX );
		result.InsertLast( (uint16*)map.Proto->GridsVec[g]->MapY );
	}
}
*/

bool P::Map_IsGrid( Map& map, uint16 hx, uint16 hy )
{
	if( map.Proto->HexFlags[hy * map.GetMaxHexX() + hx] & HEX_FLAG_SCEN_GRID )
		return( true );

	return( false );
}

int16 P::Map_GetGridDirection(Map& map, uint16 hx, uint16 hy)
{
	for(auto it = map.Proto->GridsVec.begin(), end = map.Proto->GridsVec.end(); it != end; ++it)
	{
		MapObject* obj = *it;
		if( obj->MapX == hx && obj->MapY == hy )
			return( obj->Dir );
	}

	return( -255 );
}