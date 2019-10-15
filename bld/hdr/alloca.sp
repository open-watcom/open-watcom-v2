#ifndef alloca
 _WCRTLINK extern unsigned stackavail( void );
 _WCRTLINK extern unsigned _stackavail( void );
:segment !INTEL_ONLY
 #ifdef _M_IX86
:endsegment
  #pragma aux stackavail __modify __nomemory
  #pragma aux _stackavail __modify __nomemory

  extern void __based(__segname("_STACK")) *__doalloca(__w_size_t __size);
:segment BITS16
  #ifdef _M_I86
    #pragma aux __doalloca = \
            "sub sp,ax"     \
        __parm __nomemory [__ax] \
        __value [__sp] \
        __modify __exact __nomemory [__sp]
  #else
:endsegment
     #pragma aux __doalloca = \
            "sub esp,eax"   \
        __parm __nomemory [__eax] \
        __value [__esp] \
        __modify __exact __nomemory [__esp]
:segment BITS16
  #endif
:endsegment

  #define __ALLOCA_ALIGN( s )   (((s)+(sizeof(int)-1))&~(sizeof(int)-1))
  #define __alloca( s )         __doalloca(__ALLOCA_ALIGN(s))

::
:: in case no space on the stack for segmented architecture (Intel)
:: based pointer can not be simply compared with NULL
:: segment part of based pointer is always non-zero (stack segment) then
:: (void *)0 must be used in this case to be able simply compare with NULL
::
:segment DOS
  #ifdef _M_I86
:endsegment
   #define alloca( s )  ((__ALLOCA_ALIGN(s)<stackavail())?__alloca(s): (void *)0)
   #define _alloca( s ) ((__ALLOCA_ALIGN(s)<stackavail())?__alloca(s): (void *)0)
:segment DOS
  #else
   extern void __GRO(__w_size_t __size);
   #pragma aux __GRO "*" __parm __routine []
   #define alloca( s )  ((__ALLOCA_ALIGN(s)<stackavail())?(__GRO(__ALLOCA_ALIGN(s)),__alloca(s)): (void *)0)
   #define _alloca( s ) ((__ALLOCA_ALIGN(s)<stackavail())?(__GRO(__ALLOCA_ALIGN(s)),__alloca(s)): (void *)0)
  #endif
:endsegment

:segment !INTEL_ONLY
 #else
:: All non-x86 platforms
  extern void *__builtin_alloca(__w_size_t __size);
  #pragma intrinsic(__builtin_alloca)

  #define __alloca( s ) (__builtin_alloca(s))

  #define alloca( s )   ((s<stackavail())?__alloca(s):(void *)0)
  #define _alloca( s )  ((s<stackavail())?__alloca(s):(void *)0)
 #endif
:endsegment
#endif
