#ifndef __P__EXT_EVENTS__
#define __P__EXT_EVENTS__

class Critter;
class Map;
class ScriptString;

namespace P
{
	void Critter_ClearExtEvents(Critter* cr);
	bool Critter_SetEventExt(Critter* cr, int event_type, ScriptString* func_name);
	bool Critter_EventExtMapIn(Critter* cr, Map* map);
	bool Critter_EventExtMapOut(Critter* cr, Map* map);
};

#endif // __P__EXT_EVENTS__ //