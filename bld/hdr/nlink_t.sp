::
:: POSIX nlink_t typedef declaration
::
#ifndef _NLINK_T_DEFINED_
 #define _NLINK_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned short nlink_t;/* Used for link counts             */
:elsesegment ARCHMIPS
 typedef unsigned long  nlink_t;/* Used for link counts             */
:elsesegment
 #ifdef __386__
  typedef unsigned short nlink_t;/* Used for link counts             */
 #elif defined(__MIPS__)
  typedef unsigned long  nlink_t;/* Used for link counts             */
 #endif
:endsegment
:elsesegment QNX
 typedef short unsigned nlink_t;/* Used for link counts             */
:elsesegment
 typedef unsigned short nlink_t;/* Used for link counts             */
:endsegment
#endif
