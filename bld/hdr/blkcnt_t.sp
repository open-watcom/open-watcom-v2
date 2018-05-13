::
:: POSIX blkcnt_t typedef declaration
::
#ifndef _BLKCNT_T_DEFINED_
 #define _BLKCNT_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned long  blkcnt_t;
:elsesegment ARCHMIPS
 typedef unsigned long  blkcnt_t;
:elsesegment
 #ifdef __386__
  typedef unsigned long  blkcnt_t;
 #elif defined(__MIPS__)
  typedef unsigned long  blkcnt_t;
 #endif
:endsegment
:elsesegment QNX
:elsesegment
:endsegment
#endif
