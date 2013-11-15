#ifndef alloca
 _WCRTLINK extern void  *alloca(_w_size_t __size);
 _WCRTLINK extern void  *_alloca(_w_size_t __size);
 _WCRTLINK extern unsigned stackavail( void );
 _WCRTLINK extern unsigned _stackavail( void );
 #ifdef _M_IX86
  extern void  *__doalloca(_w_size_t __size);
  #pragma aux stackavail __modify __nomemory;
  #pragma aux _stackavail __modify __nomemory;

  #define __ALLOCA_ALIGN( s )   (((s)+(sizeof(int)-1))&~(sizeof(int)-1))
  #define __alloca( s )         __doalloca(__ALLOCA_ALIGN(s))

:segment DOS
  #if defined( _M_I86 )
:endsegment
   #define alloca( s )  ((__ALLOCA_ALIGN(s)<stackavail())?__alloca(s):NULL)
   #define _alloca( s ) ((__ALLOCA_ALIGN(s)<stackavail())?__alloca(s):NULL)
:segment DOS
  #else
   extern void __GRO(_w_size_t __size);
   #pragma aux __GRO "*" __parm __routine [];
   #define alloca( s )  ((__ALLOCA_ALIGN(s)<stackavail())?(__GRO(__ALLOCA_ALIGN(s)),__alloca(s)):NULL)
   #define _alloca( s ) ((__ALLOCA_ALIGN(s)<stackavail())?(__GRO(__ALLOCA_ALIGN(s)),__alloca(s)):NULL)
  #endif
:endsegment

:segment DOS | QNX
  #if defined( _M_I86 )
   #if defined(__SMALL__) || defined(__MEDIUM__)
:: small data models
    #pragma aux __doalloca = \
            "sub sp,ax"     \
            __parm __nomemory [__ax] __value [__sp] __modify __exact __nomemory [__sp];
   #else
:: large data models
    #pragma aux __doalloca = \
            "sub sp,ax"     \
            "mov ax,sp"     \
            "mov dx,ss"     \
            __parm __nomemory [__ax] __value [__dx __ax] __modify __exact __nomemory [__dx __ax __sp];
   #endif
  #else            
:endsegment
   #if defined(__COMPACT__) || defined(__LARGE__)
:: large data models
     #pragma aux __doalloca = \
            "sub esp,eax"   \
            "mov eax,esp"   \
            "mov dx,ss"     \
            __parm __nomemory [__eax] __value [__dx __eax] __modify __exact __nomemory [__dx __eax __esp];
   #else            
:: small data models
     #pragma aux __doalloca = \
            "sub esp,eax"   \
            __parm __nomemory [__eax] __value [__esp] __modify __exact __nomemory [__esp];
   #endif            
:segment DOS | QNX
  #endif
:endsegment
 #else
:: All non-x86 platforms
  extern void *__builtin_alloca(_w_size_t __size);
  #pragma intrinsic(__builtin_alloca);

  #define __alloca( s ) (__builtin_alloca(s))

  #define alloca( s )   ((s<stackavail())?__alloca(s):NULL)
  #define _alloca( s )  ((s<stackavail())?__alloca(s):NULL)
 #endif
#endif
