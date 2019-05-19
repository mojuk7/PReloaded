#ifndef __P__AS__
#define __P__AS__

#include <string>

#include <Extension.h>

namespace ScriptUtils
{
	bool ParseScript(const std::string& script, std::string& module, std::string& func);
};

namespace P
{
	class AS : public Extension
	{
	public:
		AS();

		virtual void Event(const uint& id) override;
	};
};

#endif // __P__AS__ //
