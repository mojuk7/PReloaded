#ifndef __P__WALL_DIST__
#define __P__WALL_DIST__

#include <ProtoMap.h>

struct WallDist
{
	const ProtoMap* proto;
	uint8* distances;

	WallDist(const ProtoMap* proto);
	~WallDist();

private:
	void prepare();
};

#endif // __P__WALL_DIST__ //
