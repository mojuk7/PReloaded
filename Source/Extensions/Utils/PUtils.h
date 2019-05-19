#ifndef __P__UTILS__
#define __P__UTILS__

#include <string>

#include <Extension.h>

namespace P
{
	class Utils : public Extension
	{
	public:
		Utils();

		virtual size_t GetFunctionAddress( const std::string& name ) override;
		virtual void Event( const uint& id ) override;
	};
};

#endif // __P__UTILS__ //