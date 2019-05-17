#ifndef __P__
#define __P__

#include "_defines.fos"

#define GET_ADDRESS_N(func,freal)  if( name == #func ) return (size_t)&freal
#define GET_ADDRESS(func)          GET_ADDRESS_N(func,func)

#endif // __P__ //
