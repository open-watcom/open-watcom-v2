#ifndef _WCTHROWS
#ifdef _CPPUNWIND
#define _WCTHROWS( __t )        throw __t
#else
#define _WCTHROWS( __t )
#endif
#endif
