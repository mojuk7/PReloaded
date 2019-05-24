#if !defined(FOCLASSIC_SERVER) && !defined(FOCLASSIC_CLIENT)
#error "PANIC: no defines in server/client parameters"
#endif

#include <string>

#if defined(FOCLASSIC_SERVER)
#include <strstream>

// are they still needed?
#include <windows.h>
#include <Mmsystem.h>

#include <Critter.h>
#elif defined(FOCLASSIC_CLIENT)
#include <CritterCl.h>
#endif
 
#include <App.h>
#include <GameOptions.h>
#include <Item.h>
#include <Log.h>
#include <Script.h>

#include <Extension.h>
#include <Extension.Helper.h>

#include "Parameters.h"
#include "PReloaded.h"

#define BONUS_WEAPON_MAX_AP						(105)
#define BONUS_WEAPON_MAX_RANGE                  (106)
#define BONUS_ARMOR_CRIT_CHANCE					(100)
#define BONUS_ARMOR_CRIT_POWER					(101)
#define BONUS_ARMOR_NORMAL_DT					(102)
#define BONUS_ARMOR_LASER_DT					(103)
#define BONUS_ARMOR_FIRE_DT						(104)
#define BONUS_ARMOR_PLASMA_DT					(105)
#define BONUS_ARMOR_NORMAL_DR					(106)
#define BONUS_ARMOR_LASER_DR					(107)
#define BONUS_ARMOR_FIRE_DR						(108)
#define BONUS_ARMOR_PLASMA_DR					(109)
#define BONUS_ARMOR_EXPLODE_DT					(110)
#define BONUS_ARMOR_EXPLODE_DR					(111)
#define BONUS_ARMOR_MAX_AP						(112)
#define BONUS_ARMOR_RAD_RES						(113)
#define BONUS_ARMOR_POISON_RES					(114)
#define BONUS_ARMOR_STRENGTH					(115)
#define BONUS_ARMOR_PERCEPTION					(116)
#define BONUS_ARMOR_ENDURANCE					(117)
#define BONUS_ARMOR_CHARISMA					(118)
#define BONUS_ARMOR_INTELLIGENCE				(119)
#define BONUS_ARMOR_AGILITY						(120)
#define BONUS_ARMOR_LUCK						(121)
#define BONUS_ARMOR_CARRY_WEIGHT				(122)
#define BONUS_ARMOR_HEALING_RATE				(123)

using namespace std;

// Slot/parameters allowing
bool allowSlot_Hand1(uint8, Item&, Critter&, Critter& toCr);

// Parameters Get behavior
int getParam_Strength(CritterMutual& cr, uint);
int getParam_Perception(CritterMutual& cr, uint);
int getParam_Endurance(CritterMutual& cr, uint);
int getParam_Charisma(CritterMutual& cr, uint);
int getParam_Intellegence(CritterMutual& cr, uint);
int getParam_Agility(CritterMutual& cr, uint);
int getParam_Luck(CritterMutual& cr, uint);
int getParam_Hp(CritterMutual& cr, uint);
int getParam_MaxLife(CritterMutual& cr, uint);
int getParam_MaxAp(CritterMutual& cr, uint);
int getParam_Ap(CritterMutual& cr, uint);
int getParam_MaxMoveAp(CritterMutual& cr, uint);
int getParam_MoveAp(CritterMutual& cr, uint);
int getParam_MaxWeight(CritterMutual& cr, uint);
int getParam_Sequence(CritterMutual& cr, uint);
int getParam_MeleeDmg(CritterMutual& cr, uint);
int getParam_HealingRate(CritterMutual& cr, uint);
int getParam_CriticalChance(CritterMutual& cr, uint);
int getParam_MaxCritical(CritterMutual& cr, uint);
int getParam_Ac(CritterMutual& cr, uint);
int getParam_DamageResistance(CritterMutual& cr, uint index);
int getParam_DamageThreshold(CritterMutual& cr, uint index);
int getParam_RadiationResist(CritterMutual& cr, uint);
int getParam_PoisonResist(CritterMutual& cr, uint);
int getParam_Reputation(CritterMutual& cr, uint index);
int getParam_Timeout(CritterMutual& cr, uint index);
//void changedParam_Reputation(CritterMutual& cr, uint index, int oldValue);

int Critter_GetAC(CritterMutual& cr, bool head);
int Critter_GetDT(CritterMutual& cr, uint dmg_type, bool head);
int Critter_GetDR(CritterMutual& cr, uint dmg_type, bool head);

// Extended methods
bool Critter_IsInjured(CritterMutual& cr);
bool Critter_IsDmgEye(CritterMutual& cr);
bool Critter_IsDmgLeg(CritterMutual& cr);
bool Critter_IsDmgTwoLeg(CritterMutual& cr);
bool Critter_IsDmgArm(CritterMutual& cr);
bool Critter_IsDmgTwoArm(CritterMutual& cr);
bool Critter_IsAddicted(CritterMutual& cr);
bool Critter_IsOverweight(CritterMutual& cr);
bool Item_Weapon_IsHtHAttack(Item& item, uint8 mode);
bool Item_Weapon_IsGunAttack(Item& item, uint8 mode);
bool Item_Weapon_IsRangedAttack(Item& item, uint8 mode);
void Item_Weapon_SetMode(Item& item, uint8 mode);

// Callbacks
uint GetUseApCost(CritterMutual& cr, Item& item, uint8 mode);
uint GetAttackDistantion(CritterMutual& cr, Item& item, uint8 mode);

// Generic stuff
//int  GetNightPersonBonus();
uint GetAimApCost(int hitLocation);
uint GetAimHit(int hitLocation);
int GetArmoredDR(CritterMutual& cr, int dmgType, Item* armor);
int GetArmoredDT(CritterMutual& cr, int dmgType, Item* armor);
Item* GetHeadArmor(CritterMutual& cr);
bool IsRunning(CritterMutual& cr);
int GetArmorDR(CritterMutual& cr, int dmgType, Item* armor);
int GetArmorDT(CritterMutual& cr, int dmgType, Item* armor);

/************************************************************************/
/* Slot/parameters allowing                                             */
/************************************************************************/

static bool allowSlot_Hand1(uint8, Item&, Critter&, Critter& toCr)
{
	//return toCr.GetRawParam(PE_AWARENESS) != 0;
	return true;
}

/************************************************************************/
/* Parameters Get behaviors                                             */
/************************************************************************/

static int checkBonus(Item& it, int bonusType)
{
	if(it.Data.ScriptValues[0]==bonusType) return it.Data.ScriptValues[5];
	if(it.Data.ScriptValues[1]==bonusType) return it.Data.ScriptValues[6];
	if(it.Data.ScriptValues[2]==bonusType) return it.Data.ScriptValues[7];
	if(it.Data.ScriptValues[3]==bonusType) return it.Data.ScriptValues[8];
	if(it.Data.ScriptValues[4]==bonusType) return it.Data.ScriptValues[9];
	return 0;
}

static int checkBonus(const Item* it, int bonusType)
{
	if( it )
	{
		if(it->Data.ScriptValues[0]==bonusType) return it->Data.ScriptValues[5];
		if(it->Data.ScriptValues[1]==bonusType) return it->Data.ScriptValues[6];
		if(it->Data.ScriptValues[2]==bonusType) return it->Data.ScriptValues[7];
		if(it->Data.ScriptValues[3]==bonusType) return it->Data.ScriptValues[8];
		if(it->Data.ScriptValues[4]==bonusType) return it->Data.ScriptValues[9];
	}
	return 0;
}

static int getParam_Strength(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_STRENGTH) + cr.GetRawParam(ST_STRENGTH_EXT);

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_STRENGTH)!=0) val++;

	return CLAMP(val, 1, 10);
}

static int getParam_Perception(CritterMutual& cr, uint)
{
	int val = (cr.GetRawParam(DAMAGE_EYE) ? 1 : cr.GetRawParam(ST_PERCEPTION) + cr.GetRawParam(ST_PERCEPTION_EXT));
	//if(cr.GetRawParam(TRAIT_NIGHT_PERSON)) val += GetNightPersonBonus();

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_PERCEPTION)!=0) val++;

	return CLAMP(val, 1, 10);
}

static int getParam_Endurance(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_ENDURANCE) + cr.GetRawParam(ST_ENDURANCE_EXT);

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_ENDURANCE)!=0) val++;

	return CLAMP(val, 1, 10);
}

static int getParam_Charisma(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_CHARISMA) + cr.GetRawParam(ST_CHARISMA_EXT);

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_CHARISMA)!=0) val++;

	return CLAMP(val, 1, 10);
}

static int getParam_Intellegence(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_INTELLECT) + cr.GetRawParam(ST_INTELLECT_EXT);
	//if(cr.GetRawParam(TRAIT_NIGHT_PERSON)) val += GetNightPersonBonus();

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_INTELLIGENCE)!=0) val++;

	return CLAMP(val, 1, 10);
}

static int getParam_Agility(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_AGILITY) + cr.GetRawParam(ST_AGILITY_EXT);

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_AGILITY)!=0) val++;

	return CLAMP(val,1,10);
}

static int getParam_Luck(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_LUCK) + cr.GetRawParam(ST_LUCK_EXT);

	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_LUCK)!=0) val++;

	return CLAMP(val, 1, 10);
}

static int getParam_Hp(CritterMutual& cr, uint)
{
	return cr.GetRawParam(ST_CURRENT_HP);
}

static int getParam_MaxLife(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_MAX_LIFE) + cr.GetRawParam(ST_MAX_LIFE_EXT) + cr.GetRawParam(ST_STRENGTH) + cr.GetRawParam(ST_ENDURANCE) * 2;
	return CLAMP(val, 1, 9999);
}

static int getParam_MaxAp(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_ACTION_POINTS) + cr.GetRawParam(ST_ACTION_POINTS_EXT) + getParam_Agility(cr, 0) / 2;
	
	const Item* armor=cr.ItemSlotArmor;
	if(checkBonus(armor, BONUS_ARMOR_MAX_AP)!=0) val++;

	const Item* weapon = cr.ItemSlotMain;
	if (checkBonus(weapon, BONUS_WEAPON_MAX_AP)!=0) val++;

	return CLAMP(val, 1, 9999);
}

static int getParam_Ap(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_CURRENT_AP);
	val /= AP_DIVIDER;
	return CLAMP(val, -9999, 9999);
}

static int getParam_MaxMoveAp(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_MAX_MOVE_AP);
	if(getParam_Agility(cr, 0) % 2 == 1)
		val++;
	return CLAMP(val, 0, 9999);
}

static int getParam_MoveAp(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_MOVE_AP);
	return CLAMP(val, 0, 9999);
}

static int getParam_MaxWeight(CritterMutual& cr, uint)
{
	int val = max(cr.GetRawParam(ST_CARRY_WEIGHT) + cr.GetRawParam(ST_CARRY_WEIGHT_EXT), 0);
	val += CONVERT_GRAMM(25 + getParam_Strength(cr, 0) * 25);
	if(cr.GetRawParam(PE_PACK_RAT))
	{
		val*=4;
		val/=3;
	}
	val+=20000;

	const Item* armor=cr.ItemSlotArmor;
	val+=checkBonus(armor, BONUS_ARMOR_CARRY_WEIGHT)*1000;

	return CLAMP(val, 0, 2000000000);
}

static int getParam_Sequence(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_SEQUENCE) + cr.GetRawParam(ST_SEQUENCE_EXT) + getParam_Perception(cr, 0) * 2;
	return CLAMP(val, 0, 9999);
}

static int getParam_MeleeDmg(CritterMutual& cr, uint)
{
	int strength = getParam_Strength(cr, 0);
	int val = cr.GetRawParam(ST_MELEE_DAMAGE) + cr.GetRawParam(ST_MELEE_DAMAGE_EXT) + (strength > 6 ? (cr.GetRawParam(TRAIT_BRUISER)?2*(strength - 5):(strength - 5)) : 1);
	return CLAMP(val, 1, 9999);
}

static int getParam_HealingRate(CritterMutual& cr, uint)
{
	int e = getParam_Endurance(cr, 0);
	int val = cr.GetRawParam(ST_HEALING_RATE) + cr.GetRawParam(ST_HEALING_RATE_EXT) + 7 + e/2;

	const Item* armor=cr.ItemSlotArmor;
	val+=checkBonus(armor, BONUS_ARMOR_HEALING_RATE);

	return CLAMP(val, 0, 9999);
}

static int getParam_CriticalChance(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_CRITICAL_CHANCE) + cr.GetRawParam(ST_CRITICAL_CHANCE_EXT) + getParam_Luck(cr, 0);
	return CLAMP(val, 0, 100);
}

static int getParam_MaxCritical(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_MAX_CRITICAL) + cr.GetRawParam(ST_MAX_CRITICAL_EXT);
	return CLAMP(val, -100, 100);
}

static int GetAC(CritterMutual& cr, bool head)
{
	int val = cr.GetRawParam(ST_ARMOR_CLASS) + cr.GetRawParam(ST_ARMOR_CLASS_EXT) + 3*getParam_Agility(cr, 0);

	while(cr.GetRawParam(PE_LIVEWIRE))
	{
		Item* weapon=cr.ItemSlotMain;
		if(weapon->IsWeapon() && weapon->Proto->Weapon_Skill[0]!=SK_UNARMED && weapon->Proto->Weapon_Skill[0]!=SK_THROWING && FLAG(weapon->Proto->Flags,ITEM_FLAG_TWO_HANDS))
			break; // 2-handed sg/bg/ew

		weapon=cr.ItemSlotExt;
		if(weapon->IsWeapon() && weapon->Proto->Weapon_Skill[0]!=SK_UNARMED && weapon->Proto->Weapon_Skill[0]!=SK_THROWING && FLAG(weapon->Proto->Flags,ITEM_FLAG_TWO_HANDS))
			break; // 2-handed sg/bg/ew

		val += 3*getParam_Agility(cr, 0);
		break;
	}

	Item* armor = (head ? (Item*)GetHeadArmor(cr) : cr.ItemSlotArmor);
	if (armor->GetId() && armor->IsArmor())
	{
		P::ProtoItemUserData userdata = P::GetProtoItemUserData(armor->Proto);
		val += userdata.Armor_AC;
	}

	return CLAMP(val, 0, 90);
}

static int getParam_Ac(CritterMutual& cr, uint)
{
	return GetAC(cr,false);
}

static int getParam_DamageResistance(CritterMutual& cr, uint index)
{
	int dmgType = index - ST_NORMAL_RESIST + 1;
	return GetArmoredDR(cr, dmgType, cr.ItemSlotArmor);
}

static int getParam_DamageThreshold(CritterMutual& cr, uint index)
{
	int dmgType = index - ST_NORMAL_ABSORB + 1;
	return GetArmoredDT(cr, dmgType, cr.ItemSlotArmor);
}

static int getParam_RadiationResist(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_RADIATION_RESISTANCE) + cr.GetRawParam(ST_RADIATION_RESISTANCE_EXT) + getParam_Endurance(cr, 0) * 2;

	const Item* armor=cr.ItemSlotArmor;
	val+=checkBonus(armor, BONUS_ARMOR_RAD_RES);

	return CLAMP(val, 0, 95);
}

static int getParam_PoisonResist(CritterMutual& cr, uint)
{
	int val = cr.GetRawParam(ST_POISON_RESISTANCE) + cr.GetRawParam(ST_POISON_RESISTANCE_EXT) + getParam_Endurance(cr, 0) * 3;

	const Item* armor=cr.ItemSlotArmor;
	val+=checkBonus(armor, BONUS_ARMOR_POISON_RES);

	return CLAMP(val, 0, 95);
}

static int getParam_Reputation(CritterMutual& cr, uint index)
{
/*#ifdef __SERVER
	if(cr.GetRawParam(index) == 0x80000000)
	{
		FOClassic->CritterChangeParameter(cr, index);
		cr.Data.Params[index] = 0;
	}
#else*/
	if(cr.GetRawParam(index) == 0x80000000) return 0;
//#endif
	return cr.GetRawParam(index);
}

static int getParam_Timeout(CritterMutual& cr, uint index)
{
	return (uint)cr.GetRawParam(index) > GameOpt.FullSecond ? (uint)cr.GetRawParam(index) - GameOpt.FullSecond : 0;
}

static int getParam_SleepinessTimeout(CritterMutual& cr, uint index)
{
	return 0;
	/*
	uint full = FOClassic->FullSecond;
	uint stopped=cr.GetRawParam(TO_SLEEPY_STOPPED);
	uint cumulated=0;
	if(cr.GetRawParam(TO_SLEEPY_CUMULATE)!=0 && full>uint(cr.GetRawParam(TO_IMMUNITY_TIME)))
	{
		if(cr.GetRawParam(TO_IMMUNITY_TIME)>cr.GetRawParam(TO_SLEEPY_CUMULATE)) cumulated=cr.GetRawParam(TO_IMMUNITY_TIME);
		else cumulated=cr.GetRawParam(TO_SLEEPY_CUMULATE);
	}
	uint sleepiness = cr.GetRawParam(index) + ((cumulated!=0)?(full-cumulated):0) + ((stopped!=0 && stopped!=(uint)-1)?(full-stopped):0);
	return sleepiness > FOClassic->FullSecond ? sleepiness - FOClassic->FullSecond : 0;
	*/
}

/*static void changedParam_Reputation(CritterMutual& cr, uint index, int oldValue)
{
	if(oldValue == 0x80000000) cr.GetRawParam(index) += 0x80000000;
}*/

/************************************************************************/
/* Extended methods                                                     */
/************************************************************************/

static bool Critter_IsInjured(CritterMutual& cr)
{
	return Critter_IsDmgArm(cr) || Critter_IsDmgLeg(cr) || Critter_IsDmgEye(cr);
}

static bool Critter_IsDmgEye(CritterMutual& cr)
{
	return cr.GetRawParam(DAMAGE_EYE) != 0;
}

static bool Critter_IsDmgLeg(CritterMutual& cr)
{
	return cr.GetRawParam(DAMAGE_RIGHT_LEG) || cr.GetRawParam(DAMAGE_LEFT_LEG);
}

static bool Critter_IsDmgTwoLeg(CritterMutual& cr)
{
	return cr.GetRawParam(DAMAGE_RIGHT_LEG) && cr.GetRawParam(DAMAGE_LEFT_LEG);
}

static bool Critter_IsDmgArm(CritterMutual& cr)
{
	return cr.GetRawParam(DAMAGE_RIGHT_ARM) || cr.GetRawParam(DAMAGE_LEFT_ARM);
}

static bool Critter_IsDmgTwoArm(CritterMutual& cr)
{
	return cr.GetRawParam(DAMAGE_RIGHT_ARM) && cr.GetRawParam(DAMAGE_LEFT_ARM);
}

static bool Critter_IsAddicted(CritterMutual& cr)
{
	for(uint i = ADDICTION_BEGIN; i <= ADDICTION_END; i++)
		if(cr.GetRawParam(i)) return true;
	return false;
}

static bool Critter_IsOverweight(CritterMutual& cr)
{
	// Calculate inventory items weight
	// TODO check if difference between Critter::GetItemsWeight() and CritterCl::GetItemsWeight() matters
	return cr.GetItemsWeight() > (uint)getParam_MaxWeight(cr, 0);
}

static int Critter_GetAC(CritterMutual& cr, bool head)
{
	return GetAC(cr,head);
}

static int Critter_GetDR(CritterMutual& cr, uint dmgType, bool head)
{
	return GetArmoredDR(cr, dmgType, head ? GetHeadArmor(cr) : cr.ItemSlotArmor);
}

static int Critter_GetDT(CritterMutual& cr, uint dmgType, bool head)
{
	return GetArmoredDT(cr, dmgType, head ? GetHeadArmor(cr) : cr.ItemSlotArmor);
}

static int Critter_GetArmorDR(CritterMutual& cr, uint dmgType, bool head)
{
	return GetArmorDR(cr, dmgType, head ? GetHeadArmor(cr) : cr.ItemSlotArmor);
}

static int Critter_GetArmorDT(CritterMutual& cr, uint dmgType, bool head)
{
	return GetArmorDT(cr, dmgType, head ? GetHeadArmor(cr) : cr.ItemSlotArmor);
}

static bool Item_Weapon_IsHtHAttack(Item& item, uint8 mode)
{
	if(!item.IsWeapon() || !item.WeapIsUseAviable(mode & 7)) return false;
	int skill = SKILL_OFFSET(item.Proto->Weapon_Skill[mode & 7]);
	return skill == SK_UNARMED;
}

static bool Item_Weapon_IsGunAttack(Item& item, uint8 mode)
{
	if(!item.IsWeapon() || !item.WeapIsUseAviable(mode & 7)) return false;
	int skill = SKILL_OFFSET(item.Proto->Weapon_Skill[mode & 7]);
	return skill == SK_SMALL_GUNS || skill == SK_BIG_GUNS || skill == SK_ENERGY_WEAPONS;
}

static bool Item_Weapon_IsRangedAttack(Item& item, uint8 mode)
{
	if(!item.IsWeapon() || !item.WeapIsUseAviable(mode & 7)) return false;
	int skill = SKILL_OFFSET(item.Proto->Weapon_Skill[mode & 7]);
	return skill == SK_SMALL_GUNS || skill == SK_BIG_GUNS || skill == SK_ENERGY_WEAPONS || skill == SK_THROWING;
}


static void Item_Weapon_SetMode(Item& item, uint8 mode)
{
	item.Data.Mode=mode;
}

/************************************************************************/
/* Callbacks                                                            */
/************************************************************************/

static uint GetUseApCost(CritterMutual& cr, Item& item, uint8 mode)
{
	uint8 use = mode & 0xF;
	uint8 aim = mode >> 4;
	int apCost = 1;

	if(use == USE_USE)
	{
		if(TB_BATTLE_TIMEOUT_CHECK(getParam_Timeout(cr, TO_BATTLE)))
			apCost = GameOpt.TbApCostUseItem;
		else
			apCost = GameOpt.RtApCostUseItem;
	}
	else if(use == USE_RELOAD)
	{
		if(cr.GetRawParam(PE_QUICK_POCKETS)) return 1;
		if(TB_BATTLE_TIMEOUT_CHECK(getParam_Timeout(cr, TO_BATTLE)))
			apCost = GameOpt.TbApCostReloadWeapon;
		else
			apCost = GameOpt.RtApCostReloadWeapon;

		if(item.IsWeapon() && item.Proto->Weapon_Perk == WEAPON_PERK_FAST_RELOAD) apCost--;
	}
	else if(use >= USE_PRIMARY && use <= USE_THIRD && item.IsWeapon())
	{
		int skill = item.Proto->Weapon_Skill[use];
		bool hthAttack = Item_Weapon_IsHtHAttack(item, mode);
		bool rangedAttack = Item_Weapon_IsRangedAttack(item, mode);

		apCost = item.Proto->Weapon_ApCost[use];
		if(aim) apCost += GetAimApCost(aim);
		if(cr.GetRawParam(PE_BONUS_RATE_OF_ATTACK)) apCost--;
		if(cr.GetRawParam(TRAIT_FAST_SHOT) && !hthAttack && item.WeapIsCanAim(use))
		{
			apCost--;
			if(FLAG(item.Proto->Flags,ITEM_FLAG_TWO_HANDS)) apCost--;
		}
		// Handling -AP cost bonus for weapons
		//if(checkBonus(item, BONUS_WEAPON_AP_COST)!=0) apCost--;

		if(FLAG(item.Proto->Flags,ITEM_FLAG_TWO_HANDS) && skill != SK_UNARMED && skill != SK_THROWING && apCost < 3) apCost = 3;
		else if(apCost < 2) apCost = 2; 
	}

	if(apCost < 1) apCost = 1;
	if(item.GetProtoId()==144) apCost=5;
	return apCost;
}

static uint GetAttackDistantion(CritterMutual& cr, Item& item, uint8 mode)
{
	uint8 use = mode & 0xF;
	int dist = item.Proto->Weapon_MaxDist[use];
	int strength = getParam_Strength(cr, 0);
	if(item.Proto->Weapon_Skill[use] == SKILL_OFFSET(SK_THROWING))
	{
		dist = min(dist, 3 * min(10, strength));
		dist+=6*cr.GetRawParam(PE_HEAVE_HO);
	}
	if(Item_Weapon_IsHtHAttack(item, mode) && cr.GetRawParam(MODE_RANGE_HTH)) dist++;
	dist += cr.GetMultihex();
	dist += checkBonus(item, BONUS_WEAPON_MAX_RANGE);
	if(dist < 0) dist = 0;
	return dist;
}



/************************************************************************/
/* Generic stuff                                                        */
/************************************************************************/

/*
static int GetNightPersonBonus()
{
	if(FOClassic->Hour < 6 || FOClassic->Hour > 18) return 1;
	if(FOClassic->Hour == 6 && FOClassic->Minute == 0) return 1;
	if(FOClassic->Hour == 18 && FOClassic->Minute > 0) return 1;
	return -1;
}
*/

static uint GetAimApCost(int hitLocation)
{
	switch(hitLocation)
	{
	case HIT_LOCATION_TORSO:     return GameOpt.ApCostAimTorso;
	case HIT_LOCATION_EYES:      return GameOpt.ApCostAimEyes;
	case HIT_LOCATION_HEAD:      return GameOpt.ApCostAimHead;
	case HIT_LOCATION_LEFT_ARM:
	case HIT_LOCATION_RIGHT_ARM: return GameOpt.ApCostAimArms;
	case HIT_LOCATION_GROIN:     return GameOpt.ApCostAimGroin;
	case HIT_LOCATION_RIGHT_LEG:
	case HIT_LOCATION_LEFT_LEG:  return GameOpt.ApCostAimLegs;
	case HIT_LOCATION_NONE:
	case HIT_LOCATION_UNCALLED:
	default: break;
	}
	return 0;
}

static uint GetAimHit(int hitLocation)
{
	switch(hitLocation)
	{
	case HIT_LOCATION_TORSO:     return *P::GlobalVars.HitAimTorso;
	case HIT_LOCATION_EYES:      return *P::GlobalVars.HitAimEyes;
	case HIT_LOCATION_HEAD:      return *P::GlobalVars.HitAimHead;
	case HIT_LOCATION_LEFT_ARM:
	case HIT_LOCATION_RIGHT_ARM: return *P::GlobalVars.HitAimArms;
	case HIT_LOCATION_GROIN:     return *P::GlobalVars.HitAimGroin;
	case HIT_LOCATION_RIGHT_LEG:
	case HIT_LOCATION_LEFT_LEG:  return *P::GlobalVars.HitAimLegs;
	case HIT_LOCATION_NONE:
	case HIT_LOCATION_UNCALLED:
	default: break;
	}
	return 0;
}

/************************************************************************/
/* Support functions                                                    */
/************************************************************************/

static int GetRawDR(CritterMutual& cr, int dmgType)
{
	int val = 0;
	switch(dmgType)
	{
	case DAMAGE_TYPE_NORMAL:   val = cr.GetRawParam(ST_NORMAL_RESIST)  + cr.GetRawParam(ST_NORMAL_RESIST_EXT);  break;
	case DAMAGE_TYPE_LASER:    val = cr.GetRawParam(ST_LASER_RESIST)   + cr.GetRawParam(ST_LASER_RESIST_EXT);   break;
	case DAMAGE_TYPE_FIRE:     val = cr.GetRawParam(ST_FIRE_RESIST)    + cr.GetRawParam(ST_FIRE_RESIST_EXT);    break;
	case DAMAGE_TYPE_PLASMA:   val = cr.GetRawParam(ST_PLASMA_RESIST)  + cr.GetRawParam(ST_PLASMA_RESIST_EXT);  break;
	case DAMAGE_TYPE_ELECTR:   val = cr.GetRawParam(ST_ELECTRO_RESIST) + cr.GetRawParam(ST_ELECTRO_RESIST_EXT); break;
	case DAMAGE_TYPE_EMP:      val = cr.GetRawParam(ST_EMP_RESIST)     + cr.GetRawParam(ST_EMP_RESIST_EXT);     break;
	case DAMAGE_TYPE_EXPLODE:  val = cr.GetRawParam(ST_EXPLODE_RESIST) + cr.GetRawParam(ST_EXPLODE_RESIST_EXT); break;
	case DAMAGE_TYPE_UNCALLED:
	default: break;
	}

	if(cr.GetRawParam(PE_ADRENALINE_RUSH)) // Adrenaline rush perk
	{
		int max_life=getParam_MaxLife(cr,0);
		if(cr.GetRawParam(ST_CURRENT_HP) <= max_life/4) val+=15;
		else if(cr.GetRawParam(ST_CURRENT_HP) <= max_life/2) val+=10;
		else if(cr.GetRawParam(ST_CURRENT_HP) <= 3*max_life/4) val+=5;
	}

	return val;
}

static int GetRawDT(CritterMutual& cr, int dmgType)
{

	int val = 0;
	switch(dmgType)
	{
	case DAMAGE_TYPE_NORMAL:   val = cr.GetRawParam(ST_NORMAL_ABSORB)  + cr.GetRawParam(ST_NORMAL_ABSORB_EXT);  break;
	case DAMAGE_TYPE_LASER:    val = cr.GetRawParam(ST_LASER_ABSORB)   + cr.GetRawParam(ST_LASER_ABSORB_EXT);   break;
	case DAMAGE_TYPE_FIRE:     val = cr.GetRawParam(ST_FIRE_ABSORB)    + cr.GetRawParam(ST_FIRE_ABSORB_EXT);    break;
	case DAMAGE_TYPE_PLASMA:   val = cr.GetRawParam(ST_PLASMA_ABSORB)  + cr.GetRawParam(ST_PLASMA_ABSORB_EXT);  break;
	case DAMAGE_TYPE_ELECTR:   val = cr.GetRawParam(ST_ELECTRO_ABSORB) + cr.GetRawParam(ST_ELECTRO_ABSORB_EXT); break;
	case DAMAGE_TYPE_EMP:      val = cr.GetRawParam(ST_EMP_ABSORB)     + cr.GetRawParam(ST_EMP_ABSORB_EXT);     break;
	case DAMAGE_TYPE_EXPLODE:  val = cr.GetRawParam(ST_EXPLODE_ABSORB) + cr.GetRawParam(ST_EXPLODE_ABSORB_EXT); break;
	case DAMAGE_TYPE_UNCALLED:
	default: break;
	}

	if(cr.GetRawParam(PE_ADRENALINE_RUSH)) // Adrenaline rush perk
	{
		int max_life=getParam_MaxLife(cr,0);
		if(cr.GetRawParam(ST_CURRENT_HP) <= max_life/4) val+=3;
		else if(cr.GetRawParam(ST_CURRENT_HP) <= max_life/2) val+=2;
		else if(cr.GetRawParam(ST_CURRENT_HP) <= 3*max_life/4) val+=1;
	}

	return val;
}

static int GetArmoredDR(CritterMutual& cr, int dmgType, Item* armor)
{
	P::ProtoItemUserData userdata = P::GetProtoItemUserData( armor->Proto );

	int val = 0;
	int drVal = 0;

	switch(dmgType)
	{
	case DAMAGE_TYPE_NORMAL:   val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DRNormal + checkBonus(armor, BONUS_ARMOR_NORMAL_DR);  break;
	case DAMAGE_TYPE_LASER:    val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DRLaser + checkBonus(armor, BONUS_ARMOR_LASER_DR);   break;
	case DAMAGE_TYPE_FIRE:     val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DRFire + checkBonus(armor, BONUS_ARMOR_FIRE_DR);    break;
	case DAMAGE_TYPE_PLASMA:   val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DRPlasma + checkBonus(armor, BONUS_ARMOR_PLASMA_DR);  break;
	case DAMAGE_TYPE_ELECTR:   val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DRElectr;  break;
	case DAMAGE_TYPE_EMP:      val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DREmp;     break;
	case DAMAGE_TYPE_EXPLODE:  val = GetRawDR(cr,dmgType); drVal = userdata.Armor_DRExplode + checkBonus(armor, BONUS_ARMOR_EXPLODE_DR); break;
	case DAMAGE_TYPE_UNCALLED:
	default: break;
	}

	if(armor->GetId() && armor->IsArmor())
	{
		if(FLAG(cr.GetRawParam(MODE_EXT),MODE_EXT_NO_DETERIORATION))
			val += drVal;
		else
			val += (armor->GetDeteriorationProc()!=100)?drVal:0;
	}

	if(dmgType == DAMAGE_TYPE_EMP) return CLAMP(val, 0, 999);
	return CLAMP(val, 0, 90);
}

static int GetArmoredDT(CritterMutual& cr, int dmgType, Item* armor)
{
	P::ProtoItemUserData userdata = P::GetProtoItemUserData(armor->Proto);

	int val = 0;
	int dtVal = 0;
	switch(dmgType)
	{
	case DAMAGE_TYPE_NORMAL:   val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTNormal + checkBonus(armor, BONUS_ARMOR_NORMAL_DT);  break;
	case DAMAGE_TYPE_LASER:    val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTLaser + checkBonus(armor, BONUS_ARMOR_LASER_DT);   break;
	case DAMAGE_TYPE_FIRE:     val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTFire + checkBonus(armor, BONUS_ARMOR_FIRE_DT);    break;
	case DAMAGE_TYPE_PLASMA:   val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTPlasma + checkBonus(armor, BONUS_ARMOR_PLASMA_DT);  break;
	case DAMAGE_TYPE_ELECTR:   val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTElectr;  break;
	case DAMAGE_TYPE_EMP:      val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTEmp;     break;
	case DAMAGE_TYPE_EXPLODE:  val = GetRawDT(cr,dmgType); dtVal = userdata.Armor_DTExplode + checkBonus(armor, BONUS_ARMOR_EXPLODE_DT); break;
	case DAMAGE_TYPE_UNCALLED:
	default: break;
	}

	if(armor->GetId() && armor->IsArmor())
	{
		if(FLAG(cr.GetRawParam(MODE_EXT),MODE_EXT_NO_DETERIORATION))
			val += dtVal;
		else
			val += (armor->GetDeteriorationProc()!=100)?dtVal:0;
	}

	return CLAMP(val, 0, 999);
}

static Item* GetHeadArmor(CritterMutual& cr)
{
	ItemPtrVec inv;
	#if defined(FOCLASSIC_SERVER)
	inv = cr.GetItemsNoLock();
	#else
	inv = cr.InvItems;
	#endif

	for(auto it=inv.begin(),end=inv.end();it!=end;++it)
	{
		if ( (*it)->AccCritter.Slot==SLOT_HEAD) return *it;
	}
#if defined(FOCLASSIC_SERVER)
	return cr.GetDefaultItemSlotArmor();
#else
	return cr.DefItemSlotArmor;
#endif
}

static int GetArmorDR(CritterMutual& cr, int dmgType, Item* armor)
{
	P::ProtoItemUserData userdata = P::GetProtoItemUserData(armor->Proto);

	int val = 0;
	int drVal = 0;
	switch(dmgType)
	{
	case DAMAGE_TYPE_NORMAL:   drVal = userdata.Armor_DRNormal + checkBonus(armor, BONUS_ARMOR_NORMAL_DR);  break;
	case DAMAGE_TYPE_LASER:    drVal = userdata.Armor_DRLaser + checkBonus(armor, BONUS_ARMOR_LASER_DR);   break;
	case DAMAGE_TYPE_FIRE:     drVal = userdata.Armor_DRFire + checkBonus(armor, BONUS_ARMOR_FIRE_DR);    break;
	case DAMAGE_TYPE_PLASMA:   drVal = userdata.Armor_DRPlasma + checkBonus(armor, BONUS_ARMOR_PLASMA_DR);  break;
	case DAMAGE_TYPE_ELECTR:   drVal = userdata.Armor_DRElectr;  break;
	case DAMAGE_TYPE_EMP:      drVal = userdata.Armor_DREmp;     break;
	case DAMAGE_TYPE_EXPLODE:  drVal = userdata.Armor_DRExplode + checkBonus(armor, BONUS_ARMOR_EXPLODE_DR); break;
	case DAMAGE_TYPE_UNCALLED:
	default: break;
	}

	if(armor->GetId() && armor->IsArmor())
	{
		if(FLAG(cr.GetRawParam(MODE_EXT),MODE_EXT_NO_DETERIORATION))
			val += drVal;
		else
			val += (armor->GetDeteriorationProc()!=100)?drVal:0;
	}

	if(dmgType == DAMAGE_TYPE_EMP) return CLAMP(val, 0, 999);
	return CLAMP(val, 0, 90);
}

static int GetArmorDT(CritterMutual& cr, int dmgType, Item* armor)
{
	P::ProtoItemUserData userdata = P::GetProtoItemUserData(armor->Proto);

	int val = 0;
	int dtVal = 0;
	switch(dmgType)
	{
	case DAMAGE_TYPE_NORMAL:   dtVal = userdata.Armor_DTNormal + checkBonus(armor, BONUS_ARMOR_NORMAL_DT);  break;
	case DAMAGE_TYPE_LASER:    dtVal = userdata.Armor_DTLaser + checkBonus(armor, BONUS_ARMOR_LASER_DT);   break;
	case DAMAGE_TYPE_FIRE:     dtVal = userdata.Armor_DTFire + checkBonus(armor, BONUS_ARMOR_FIRE_DT);    break;
	case DAMAGE_TYPE_PLASMA:   dtVal = userdata.Armor_DTPlasma + checkBonus(armor, BONUS_ARMOR_PLASMA_DT);  break;
	case DAMAGE_TYPE_ELECTR:   dtVal = userdata.Armor_DTElectr;  break;
	case DAMAGE_TYPE_EMP:      dtVal = userdata.Armor_DTEmp;     break;
	case DAMAGE_TYPE_EXPLODE:  dtVal = userdata.Armor_DTExplode + checkBonus(armor, BONUS_ARMOR_EXPLODE_DT); break;
	case DAMAGE_TYPE_UNCALLED:
	default: break;
	}

	if(armor->GetId() && armor->IsArmor())
	{
		if(FLAG(cr.GetRawParam(MODE_EXT),MODE_EXT_NO_DETERIORATION))
			val += dtVal;
		else
			val += (armor->GetDeteriorationProc()!=100)?dtVal:0;
	}

	return CLAMP(val, 0, 999);
}

//
// Helpers
//

P::ProtoItemUserData P::GetProtoItemUserData(ProtoItem* proto)
{
	STATIC_ASSERT(sizeof(P::ProtoItemUserData) == PROTO_ITEM_USER_DATA_SIZE);
	STATIC_ASSERT(offsetof(P::ProtoItemUserData, Armor_CrTypeMale) == 100);
	STATIC_ASSERT(offsetof(P::ProtoItemUserData, Ammo_AcMod) == 300);
	STATIC_ASSERT(offsetof(P::ProtoItemUserData, UnusedLast) == PROTO_ITEM_USER_DATA_SIZE - 1);

	if (proto)
		return  *(ProtoItemUserData*)proto->UserData;

	ProtoItemUserData userdata;
	memzero(&userdata, sizeof(ProtoItemUserData));

	return userdata;
}

/************************************************************************/
/* Initialization                                                       */
/************************************************************************/

void P::Parameters::Event( const uint& id )
{
	/*
	// bool compiler - true if script compiled using ASCompiler, false if script compiled in server
	// In this function all global variables is initialized, if compiled not by compiler

	if (compiler) return;
	*/

	if ((App.Type == APP_TYPE_CLIENT && id == ExtensionEvent::SCRIPT_LOAD_CLIENT_MODULES_START) ||
		(App.Type == APP_TYPE_SERVER && id == ExtensionEvent::SCRIPT_LOAD_SERVER_MODULES_START) )
	{
		// Register callbacks
		//GameOpt.GetUseApCost = &GetUseApCost;
		//GameOpt.GetAttackDistantion = &GetAttackDistantion;
		// Register script global vars
		memzero(&GlobalVars, sizeof(GlobalVars));
		asIScriptEngine* engine = Script::GetEngine();
		for (uint i = 0; i < engine->GetGlobalPropertyCount(); i++)
		{
			const char* name;
			void* ptr;
			if (engine->GetGlobalPropertyByIndex(i, &name, NULL, NULL, NULL, NULL, &ptr) < 0) continue;

#define REGISTER_GLOBAL_VAR(type, gvar) else if(!strcmp(#gvar, name)) GlobalVars.gvar = (type*)ptr
			REGISTER_GLOBAL_VAR(int, CurX);
			REGISTER_GLOBAL_VAR(int, CurY);
			REGISTER_GLOBAL_VAR(uint, HitAimEyes);
			REGISTER_GLOBAL_VAR(uint, HitAimHead);
			REGISTER_GLOBAL_VAR(uint, HitAimGroin);
			REGISTER_GLOBAL_VAR(uint, HitAimTorso);
			REGISTER_GLOBAL_VAR(uint, HitAimArms);
			REGISTER_GLOBAL_VAR(uint, HitAimLegs);
		}
		WriteLog("GlobalVars extracted\n");
	}
}

P::Parameters::Parameters() : Extension()
{}

size_t P::Parameters::GetFunctionAddress(const string& name)
{
	GET_ADDRESS_X(critter_attack_distance, GetAttackDistantion);
	GET_ADDRESS_X(critter_use_item_ap_cost, GetUseApCost);

	GET_ADDRESS(Critter_IsInjured);
	GET_ADDRESS(Critter_IsDmgEye);
	GET_ADDRESS(Critter_IsDmgLeg);
	GET_ADDRESS(Critter_IsDmgTwoLeg);
	GET_ADDRESS(Critter_IsDmgArm);
	GET_ADDRESS(Critter_IsDmgTwoArm);
	GET_ADDRESS(Critter_IsAddicted);
	GET_ADDRESS(Critter_IsOverweight);
	GET_ADDRESS(Critter_GetAC);
	GET_ADDRESS(Critter_GetDR);
	GET_ADDRESS(Critter_GetDT);
	GET_ADDRESS(Critter_GetArmorDR);
	GET_ADDRESS(Critter_GetArmorDT);

	GET_ADDRESS(Item_Weapon_IsHtHAttack);
	GET_ADDRESS(Item_Weapon_IsGunAttack);
	GET_ADDRESS(Item_Weapon_IsRangedAttack);
	GET_ADDRESS(Item_Weapon_SetMode);

	GET_ADDRESS(getParam_Strength);
	GET_ADDRESS(getParam_Perception);
	GET_ADDRESS(getParam_Endurance);
	GET_ADDRESS(getParam_Charisma);
	GET_ADDRESS(getParam_Intellegence);
	GET_ADDRESS(getParam_Agility);
	GET_ADDRESS(getParam_Luck);
	GET_ADDRESS(getParam_Hp);
	GET_ADDRESS(getParam_MaxLife);
	GET_ADDRESS(getParam_Ap);
	GET_ADDRESS(getParam_MaxAp);
	GET_ADDRESS(getParam_MoveAp);
	GET_ADDRESS(getParam_MaxMoveAp);
	GET_ADDRESS(getParam_MaxWeight);
	GET_ADDRESS(getParam_MeleeDmg);
	GET_ADDRESS(getParam_Sequence);
	GET_ADDRESS(getParam_HealingRate);
	GET_ADDRESS(getParam_CriticalChance);
	GET_ADDRESS(getParam_MaxCritical);
	GET_ADDRESS(getParam_Ac);
	GET_ADDRESS(getParam_RadiationResist);
	GET_ADDRESS(getParam_PoisonResist);
	GET_ADDRESS(getParam_DamageResistance);
	GET_ADDRESS(getParam_DamageThreshold);
	GET_ADDRESS(getParam_Timeout);

	GET_ADDRESS(getParam_Reputation);

	return 0;
}

//
