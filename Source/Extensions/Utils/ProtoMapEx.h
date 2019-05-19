#ifndef __P__PROTO_MAP_EX__
#define __P__PROTO_MAP_EX__

#include <Map.h>

namespace P
{
	void InitLocationEx();
	void FinishMapLocationEx();

	bool Location_IsEncounter(Location& loc); 
	bool Location_IsTown(Location& loc);
	bool Location_IsCityEncounter(Location& loc);
	bool Location_IsInstancedQuest(Location& loc);
	bool Location_IsBase(Location& loc);
	bool Location_IsReplication(Location& loc);
	bool Location_IsTCTown(Location& loc);
	bool Location_IsTent(Location& loc);
	bool Location_IsMine(Location& loc);
	bool Location_IsCave(Location& loc);
	bool Location_IsGuarded(Location& loc);
	bool Location_IsPublic(Location& loc);

	bool Map_FindNearestGridRough(Map& imap, uint16& hx, uint16& hy, bool toWM);
	bool Map_FindNearestGridApprox(Map& imap, uint16& hx, uint16& hy, bool toWM);
	bool Map_FindNearestGrid(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid);
	bool Map_FindNearestGridEntire(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid, uint toEntire);
	bool Map_FindRandomGridEntire(Map& imap, uint16& hx, uint16& hy, uint16 toMapPid, uint toEntire);
	bool Map_IsGrid(Map& map, uint16 hx, uint16 hy);
	int16 Map_GetGridDirection(Map& map, uint16 hx, uint16 hy);
};

#endif // __P__PROTO_MAP_EX__ //
