::
:: POSIX rlim_t typedef declaration
::
:segment LINUX
#ifndef _RLIM_T_DEFINED_
 #define _RLIM_T_DEFINED_
:include ext.sp
 #if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64
  typedef unsigned long long  rlim_t;
 #else
  typedef unsigned long       rlim_t;
 #endif
 typedef unsigned long long   rlim64_t;
:include extelse.sp
 typedef unsigned long        rlim_t;
:include extepi.sp
#endif
:elsesegment QNX
:elsesegment
:endsegment
