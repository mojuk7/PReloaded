#ifndef __P__PARAMETERS__
#define __P__PARAMETERS__

#include <string>

#include <Extension.h>

class ProtoItem;

namespace P
{
	struct _GlobalVars
	{
		int*  CurX;
		int*  CurY;
		uint* HitAimEyes;
		uint* HitAimHead;
		uint* HitAimGroin;
		uint* HitAimTorso;
		uint* HitAimArms;
		uint* HitAimLegs;
	};

	static _GlobalVars GlobalVars;

	struct ProtoItemUserData
	{
		// TODO SYNCH WITH #pragma bindfield IN SCRIPTS

		int    MagicPower;
		uint8  Unused[96];

		// Armor, offset 100
		uint   Armor_CrTypeMale;
		uint   Armor_CrTypeFemale;
		int    Armor_AC;
		uint   Armor_Perk;
		int    Armor_DRNormal;
		int    Armor_DRLaser;
		int    Armor_DRFire;
		int    Armor_DRPlasma;
		int    Armor_DRElectr;
		int    Armor_DREmp;
		int    Armor_DRExplode;
		int    Armor_DTNormal;
		int    Armor_DTLaser;
		int    Armor_DTFire;
		int    Armor_DTPlasma;
		int    Armor_DTElectr;
		int    Armor_DTEmp;
		int    Armor_DTExplode;
		uint8  Armor_Unused[28];
		// Weapon, offset 200
		int    Weapon_DmgType[3];
		uint   Weapon_Anim2[3];
		int    Weapon_DmgMin[3];
		int    Weapon_DmgMax[3];
		uint16 Weapon_Effect[3];
		bool   Weapon_Remove[3];
		uint   Weapon_ReloadAp;
		int    Weapon_UnarmedCriticalBonus;
		uint   Weapon_CriticalFailture;
		bool   Weapon_UnarmedArmorPiercing;
		uint8  Weapon_Unused[27];
		// Ammo, offset 300
		int    Ammo_AcMod;
		int    Ammo_DrMod;
		uint   Ammo_DmgMult;
		uint   Ammo_DmgDiv;
		// Other
		uint8  UnusedEnd[183];
		uint8  UnusedLast;
	};

	ProtoItemUserData GetProtoItemUserData(ProtoItem* proto);

	class Parameters : public Extension
	{
public:
		Parameters();

		virtual size_t GetFunctionAddress(const std::string& name) override;
		virtual void Event(const uint& id) override;
	};
};

#endif // __P__PARAMETERS__ //
