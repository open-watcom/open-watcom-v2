#ifndef _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED
#define _PTRDIFF_T_DEFINED_
#if defined(__HUGE__)
typedef long ptrdiff_t;
#else
typedef int ptrdiff_t;
#endif
#endif
