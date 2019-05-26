#ifndef __P__MAPPER_UTILS__
#define __P__MAPPER_UTILS__

#include <Extension.h>

namespace P
{
	class MapperUtils : public Extension
	{
	public:
		MapperUtils();

		virtual size_t GetFunctionAddress(const std::string& name) override;
	};
};

#endif // __P__MAPPER_UTILS__ //
