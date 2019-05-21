#include <cmath>
#include <sstream>
#include <string>
#include <vector>

#include <Critter.h>
#include <GameOptions.h>
#include <Map.h>

#include <Extension.Helper.h>

#include "CheckLook.h"
#include "ITEMPID.H" // scripts/
#include "Move.h"
#include "Parameters/Parameters.h"
#include "PReloaded.h"
#include "WallDist.h"

#include "_animation.fos"

using namespace std;

// TODO: multihex bonus to look distance (once multihex works properly)

bool isCompiler=false;
bool Init=false;

extern uint8* Lookup;
extern void InitLookup();
extern void FinishLookup();

extern void InitDists();
extern WallDist* GetProtoDists(Map& map);
extern void FinishDists();
//extern bool IsMoving(Critter& cr);

void InitLook() // generation
{
	InitLookup();
	InitDists();
	Init=true;
}

int CheckOccluder(uint16 hx, uint16 hy, uint16 tx, uint16 ty, Map& map)
{
    int dx = (int)tx - (int)hx;
    int dy = (int)ty - (int)hy;
    if ((ABS(dx)>=MAX_TRACE) || (ABS(dy)>=MAX_TRACE)) return 0;
    int idx = (SIZE_LIN*(dy+MAX_TRACE)+dx+MAX_TRACE)*MAX_ARR;
    uint16 maxX = map.Proto->Header.MaxHexX;
    uint16 maxY = map.Proto->Header.MaxHexY;
    for (uint i=0;i<OCCLUDER_DIST;i++)
    {
        Move(hx,hy,Lookup[idx++]);
        if ((hx>=maxX) || (hy>=maxY)) return 0; // this include uint16(-1), it's not nice, but practical

		// you can shoot thru
		if(map.IsHexRaked(hx, hy))
		{
			// but not pass - that means occluder
			if(!map.IsHexPassed(hx, hy))
                return BONUS_OCCLUDER;// optimize it that way for now
			else
				continue; // clean
		} // you can't shoot through it
		else
			return 100; // shouldn't even occur (trace check first)
	}
    return 0;
}

int TraceWall(uint16 hx, uint16 hy, uint16 tx, uint16 ty, Map& map, int dist)
{
    // if (dist==0) dist=GetDistantion(hx,hy,tx,ty); // not needed here; enable if and when needed
    int dx = (int)tx - (int)hx;
    int dy = (int)ty - (int)hy;
    if ((ABS(dx)>=MAX_TRACE) || (ABS(dy)>=MAX_TRACE)) return 0;
    int idx = (SIZE_LIN*(dy+MAX_TRACE)+dx+MAX_TRACE)*MAX_ARR;
    uint16 maxX = map.Proto->Header.MaxHexX;
    uint16 maxY = map.Proto->Header.MaxHexY;
    for (int i=0;i<dist;i++)
    {
        Move(hx,hy,Lookup[idx++]);
        if ((hx>=maxX) || (hy>=maxY)) return i; // this include uint16(-1), it's not nice, but practical
        if(!map.IsHexRaked(hx, hy)) return i;
	}
    return dist;
}

static bool check_look(Map& map, Critter& cr, Critter& opponent)
{
	if(!Init) InitLook();
	if(_CritHasExtMode(opponent,MODE_EXT_GOD)) return false;

	if(!cr.CritterIsNpc)
	{
		if(_CritHasExtMode(cr,MODE_EXT_LOOK_ADMIN)) return true;
		if(cr.GetMap() != map.Data.MapId && map.Data.UserData[MAP_DATA_ACTIVE_COUNTDOWN]!=0) return false;
	}
	if(_CritHasExtMode(opponent,MODE_EXT_LOOK_INVISIBLE) && _CritHasMode(opponent, MODE_HIDE))
        return false; // 100% invis for admins

	if(_CritHasExtMode(opponent,MODE_EXT_LOOK_ALWAYS_VISIBLE) && !_CritHasMode(opponent, MODE_HIDE))
        return true;

	uint16 cx = cr.GetHexX();
	uint16 cy = cr.GetHexY();
	uint16 ox = opponent.GetHexX();
	uint16 oy = opponent.GetHexY();

	bool stealhBoyActive = (opponent.ItemSlotExt->Proto->ProtoId == PID_ACTIVE_STEALTH_BOY || opponent.ItemSlotMain->Proto->ProtoId == PID_ACTIVE_STEALTH_BOY);

	int dist = GetDistantion(cx, cy, ox, oy);
	if(dist>60) return false;

	P::ProtoItemUserData userdata_main = P::GetProtoItemUserData(cr.ItemSlotMain->Proto);
	P::ProtoItemUserData userdata_ext  = P::GetProtoItemUserData(cr.ItemSlotExt->Proto);

	if((dist<=userdata_ext.MagicPower || dist<=userdata_main.MagicPower) &&            // is in Motion Sensor range
		(!stealhBoyActive || (stealhBoyActive && !_CritHasMode(opponent, MODE_HIDE)))) // don't have active Stealth Boy OR have active SB and is not sneaked
		return true;

	// min range - always visible
	if(dist <= (int)(GameOpt.LookMinimum)) return true;

	// dead/unconcious/neg hp - only minimum range
	if(cr.Data.Cond != CRITTER_CONDITION_LIFE) return (dist <= (int)(GameOpt.LookMinimum));

    int front_range=(cr.GetRawParam(DAMAGE_EYE)!=0)?1:(CLAMP((cr.GetRawParam(ST_PERCEPTION)+cr.GetRawParam(ST_PERCEPTION_EXT)),1,10));
	if(cr.GetRawParam(PE_SHARPSHOOTER)) front_range+=2*cr.GetRawParam(PE_SHARPSHOOTER);
    front_range*=3;
    front_range+= cr.GetRawParam(ST_BONUS_LOOK);
	front_range+=(int)(GameOpt.LookNormal);

	if(dist > front_range) return false;

	// max perception range
	int max_range = front_range;

	// transform direction from critter A to critter B into "character coord-space"
	uint8 dir = (uint8)GetDirection(cx, cy, ox, oy);

	dir = cr.GetDir()>dir?cr.GetDir()-dir:dir-cr.GetDir();

    // adjust distance based on fov (NOT only for sneakers)
    switch(dir)
    {
        case 0:
            max_range -= (max_range* (int)(GameOpt.LookDir[0]))/100; // front
            break;
        case 1:
        case 5:
            max_range -= (max_range* (int)(GameOpt.LookDir[1]))/100; // frontsides
            break;
        case 2:
        case 4:
            max_range -= (max_range* (int)(GameOpt.LookDir[2]))/100; // backsides
            break;
        default:
            max_range -= (max_range* (int)(GameOpt.LookDir[3]))/100; // back
    }

	if(dist > max_range) return false;

	if(_CritHasMode(opponent, MODE_HIDE))
	{
		// wall gives success
		if (!_CritHasExtMode(cr,MODE_EXT_NO_WALL_CHECK))
		{
			max_range = TraceWall(cx, cy, ox, oy, map, max_range); // in case wall is blocking
			if(dist > max_range) return false;
		}

		int sk = opponent.GetRawParam(SK_SNEAK);

		// bonuses before clamp

		// 1. next to a wall
		WallDist* wd=GetProtoDists(map);
		if(wd->distances[oy*wd->proto->Header.MaxHexX+ox] <=
			(opponent.GetRawParam(PE_GHOST) ? 5 : 1)) sk+=BONUS_WALL;

		// 2. occluder bonus
		if (!_CritHasExtMode(cr,MODE_EXT_NO_WALL_CHECK))
			sk += CheckOccluder(ox, oy, cx, cy, map);

		// 3. stealth boy
		//if(opponent.Params[ST_SNEAK_FLAGS]&4) sk+=BONUS_STEALTH_BOY;
		if(stealhBoyActive)
			sk += BONUS_STEALTH_BOY;
		
		// 4. night TODO?

		// clamp
		sk=CLAMP(sk,-300,300);

		switch(dir)
        {
            case 0:
                sk -= (int)(GameOpt.LookSneakDir[0]); // front
                break;
            case 1:
            case 5:
                sk -= (int)(GameOpt.LookSneakDir[1]); // frontsides
                break;
            case 2:
            case 4:
                sk -= (int)(GameOpt.LookSneakDir[2]); // backsides
                break;
            default: ;
                sk -= (int)(GameOpt.LookSneakDir[3]); // back
        }

		// armor penalty, TODO: move values to protos?
		switch(opponent.ItemSlotArmor->GetProtoId())
		{
			case 1100: // for bluesuit, default armor prototype
				sk-=50;
				break;
			case PID_METAL_ARMOR:
			case PID_METAL_ARMOR_MK_II:
			case PID_TESLA_ARMOR:
				sk=-10000;
				break;
			case PID_COMBAT_ARMOR:
			case PID_COMBAT_ARMOR_MK_II:
			case PID_BROTHERHOOD_COMBAT_ARMOR:
			case PID_NCR_ARMOR:
			case PID_ENCLAVE_COMBAT_ARMOR:
			case PID_BRIDGEKEEPER_ROBE:
			case PID_CLOTHES_POLICE:
			case PID_DESERT_COMBAT_ARMOR:
				sk=-10000;
				break;
			case PID_POWERED_ARMOR :
			case PID_HARDENED_POWER_ARMOR:
			case PID_ADVANCED_POWER_ARMOR :
			case PID_ADVANCED_POWER_ARMOR_MK2:
				sk=-10000;
				break;
			default: ;
		}

		// weapons penalty
		const ProtoItem* proto=opponent.ItemSlotMain->Proto;
		if(proto->Type==ITEM_TYPE_WEAPON && (FLAG(proto->Flags,ITEM_FLAG_TWO_HANDS) || proto->Weapon_Anim1==ANIM1_SMG))
		{
			switch(proto->Weapon_Anim1)
			{
			case ANIM1_HEAVY_RIFLE:
			case ANIM1_MINIGUN:
			case ANIM1_ROCKET_LAUNCHER:
			case ANIM1_FLAMER:
				sk+=BONUS_WEAPON_HEAVY;
				break;
			default:
				if(proto->Weapon_Skill[0]<=SK_ENERGY_WEAPONS) sk+=BONUS_WEAPON_RIFLE;
				break;
			}
		}

		const ProtoItem* proto2=opponent.ItemSlotExt->Proto;
		if(proto2->Type==ITEM_TYPE_WEAPON && (FLAG(proto2->Flags,ITEM_FLAG_TWO_HANDS) || proto2->Weapon_Anim1==ANIM1_SMG))
		{
			switch(proto2->Weapon_Anim1)
			{
			case ANIM1_HEAVY_RIFLE:
			case ANIM1_MINIGUN:
			case ANIM1_ROCKET_LAUNCHER:
			case ANIM1_FLAMER:
				sk+=BONUS_WEAPON_HEAVY;
				break;
			default:
				if(proto2->Weapon_Skill[0]<=SK_ENERGY_WEAPONS) sk+=BONUS_WEAPON_RIFLE;
				break;
			}
		}

		// running
		// SESSIONS 1&2 -- if(IsMoving(opponent) && opponent.IsRuning && !opponent.Params[PE_SILENT_RUNNING]) sk+=BONUS_RUNNING;
		if( opponent.IsRuning && !opponent.GetRawParam(PE_SILENT_RUNNING)) sk+=BONUS_RUNNING;

		// active explosive held
		if(opponent.GetRawParam(ST_SNEAK_FLAGS)&1) sk+=BONUS_ACTIVE_EXPLOSIVES;

		if(sk <= 0)	return true;

        sk/=(int)(GameOpt.SneakDivider);
        return front_range >= dist+sk;
	}
	else // opponent doesn't sneak
	{
	  if (!cr.CritterIsNpc)
	  {
			max_range = TraceWall(cx, cy, ox, oy, map, max_range); // in case wall is blocking
			return dist <= max_range; // behind a wall
	  }
	  return true; // max pe range handled before
	}
}

int GetEngineLook(Critter& cr)
{
	// name is bit misleading ;)
	// see Critter::GetLook()

	int look=(cr.GetRawParam(DAMAGE_EYE)!=0)?1:(CLAMP((cr.GetRawParam(ST_PERCEPTION)+cr.GetRawParam(ST_PERCEPTION_EXT)),1,10));
    look*=3;
    look+= cr.GetRawParam(ST_BONUS_LOOK);
	look+=(int)(GameOpt.LookNormal);
    if( look < (int)GameOpt.LookMinimum )
        look = GameOpt.LookMinimum;
    return look;
}

static bool check_trap_look(Map& map, Critter& cr, Item& trap)
{
	int dist = GetDistantion(cr.GetHexX(),cr.GetHexY(),trap.AccHex.HexX,trap.AccHex.HexY);
	int perception = CLAMP(cr.GetRawParam(ST_PERCEPTION)+cr.GetRawParam(ST_PERCEPTION_EXT),1,10);
	int skilldiff = cr.GetRawParam(SK_TRAPS)-trap.TrapGetValue();
	return dist <= max((perception/2 + skilldiff/50), 2);
}

static uint Map_WallDistance(Map& map, uint16 hx, uint16 hy) // test
{
	WallDist* wd=GetProtoDists(map);
	return wd->distances[hy*wd->proto->Header.MaxHexX+hx];
}

P::CheckLook::CheckLook() : Extension()
{}

size_t P::CheckLook::GetFunctionAddress(const std::string & name)
{
	GET_ADDRESS(check_look);
	GET_ADDRESS(check_trap_look);

	return 0;
}

void P::CheckLook::Event(const uint& id)
{
	if (id == ExtensionEvent::FINISH)
	{
		FinishLookup();
		FinishDists();
	}
}
