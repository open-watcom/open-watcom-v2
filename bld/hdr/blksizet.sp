::
:: POSIX blksize_t typedef declaration
::
#ifndef _BLKSIZE_T_DEFINED_
 #define _BLKSIZE_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned long  blksize_t;
:elsesegment ARCHMIPS
 typedef unsigned long  blksize_t;
:elsesegment
 #ifdef __386__
  typedef unsigned long  blksize_t;
 #elif defined(__MIPS__)
  typedef unsigned long  blksize_t;
 #endif
:endsegment
:elsesegment QNX
:elsesegment
:endsegment
#endif
