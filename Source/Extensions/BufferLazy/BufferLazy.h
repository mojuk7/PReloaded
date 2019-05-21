#ifndef __P__BUFFER_LAZY__
#define __P__BUFFER_LAZY__

#include <Extension.h>

namespace P
{

	class BufferLazy : public Extension
	{
	public:
		BufferLazy();

		virtual size_t GetFunctionAddress( const std::string& name ) override;
	};
};

#endif // __P__BUFFER_LAZY__ //
