#ifndef __P__CHECK_LOOK__
#define __P__CHECK_LOOK__

#include <Extension.h>

#define PI_FLOAT       (3.14159265f)
#define PIBY2_FLOAT    (1.5707963f)
#define BIAS_FLOAT     (0.02f)
#define ABS(__val)     ((__val)>0?(__val):(-(__val)))
//#define MIN(__x,__y)   (__x<__y?__x:__y)

#define MAX_ARR (200)
#define SIZE_LIN (201)
#define MAX_TRACE (100)


#define MODE_EXT_NO_WALL_CHECK    (0x00000002) // applies only when sneaking
#define MODE_EXT_LOOK_ADMIN       (0x00008000)
#define MODE_EXT_LOOK_INVISIBLE   (0x00010000)
#define MODE_EXT_GOD              (0x00020000)

//#define COND_LIFE                   (1) // ������� ���

#define _CritHasMode(cr,mode) (cr.GetRawParam(mode)>0)
#define _CritHasExtMode(cr, mode) ((cr.GetRawParam(MODE_EXT)&(mode))!=0)
#define _CritHasPerk(cr,perk) (cr.GetRawParam(perk)>0)

#define MAP_DATA_ACTIVE_COUNTDOWN   (8)

#define BONUS_OCCLUDER				(60)
#define BONUS_WALL					(30)
#define BONUS_STEALTH_BOY			(60)

#define BONUS_ARMOR_METAL			(-72)
#define BONUS_ARMOR_COMBAT			(-36)

#define BONUS_WEAPON_RIFLE			(-100)
#define BONUS_WEAPON_HEAVY			(-150)

#define BONUS_ACTIVE_EXPLOSIVES		(-72)
#define BONUS_RUNNING				(-60)

//#define VOLUME #(proto)		(proto.Volume == 0 ? 50 : proto.Volume)
#define OCCLUDER_DIST (2) // 2 only, no reason to be more overkill

#define MAX_WALLS_DIST	(5)

namespace P
{
	class CheckLook : public Extension
	{
	public:
		CheckLook();

		virtual size_t GetFunctionAddress(const std::string& name) override;
		virtual void Event(const uint& id) override;
	};
};

#endif // __P__CHECK_LOOK__ //
