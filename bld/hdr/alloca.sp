#ifndef alloca
 _WCRTLINK extern unsigned stackavail( void );
 _WCRTLINK extern unsigned _stackavail( void );
 #ifdef _M_IX86
  _WCRTLINK extern void __based(__segname("_STACK")) *alloca(__w_size_t __size);
  _WCRTLINK extern void __based(__segname("_STACK")) *_alloca(__w_size_t __size);
  extern void __based(__segname("_STACK")) *__doalloca(__w_size_t __size);
  #pragma aux stackavail __modify __nomemory
  #pragma aux _stackavail __modify __nomemory

  #define __ALLOCA_ALIGN( s )   (((s)+(sizeof(int)-1))&~(sizeof(int)-1))
  #define __alloca( s )         __doalloca(__ALLOCA_ALIGN(s))

:segment DOS | RDOS
  #ifdef _M_I86
:endsegment
   #define alloca( s )  ((__ALLOCA_ALIGN(s)<stackavail())?__alloca(s): (void *)0)
   #define _alloca( s ) ((__ALLOCA_ALIGN(s)<stackavail())?__alloca(s): (void *)0)
:segment DOS | RDOS
  #else
   extern void __GRO(__w_size_t __size);
   #pragma aux __GRO "*" __parm __routine []
   #define alloca( s )  ((__ALLOCA_ALIGN(s)<stackavail())?(__GRO(__ALLOCA_ALIGN(s)),__alloca(s)): (void *)0)
   #define _alloca( s ) ((__ALLOCA_ALIGN(s)<stackavail())?(__GRO(__ALLOCA_ALIGN(s)),__alloca(s)): (void *)0)
  #endif
:endsegment

:segment DOS | QNX | RDOS
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
:segment DOS | QNX | RDOS
  #endif
:endsegment
 #else
:: All non-x86 platforms
  _WCRTLINK extern void *alloca(__w_size_t __size);
  _WCRTLINK extern void *_alloca(__w_size_t __size);
  extern void *__builtin_alloca(__w_size_t __size);
  #pragma intrinsic(__builtin_alloca)

  #define __alloca( s ) (__builtin_alloca(s))

  #define alloca( s )   ((s<stackavail())?__alloca(s):(void *)0)
  #define _alloca( s )  ((s<stackavail())?__alloca(s):(void *)0)
 #endif
#endif
