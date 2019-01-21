::
:: POSIX mode_t typedef declaration
::
#ifndef _MODE_T_DEFINED_
 #define _MODE_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned short mode_t; /* Used for some file attributes    */
:elsesegment ARCHMIPS
 typedef unsigned long  mode_t; /* Used for some file attributes    */
:elsesegment
 #ifdef __386__
  typedef unsigned short mode_t; /* Used for some file attributes    */
 #elif defined(__MIPS__)
  typedef unsigned long  mode_t; /* Used for some file attributes    */
 #endif
:endsegment
:elsesegment QNX
 typedef unsigned short mode_t; /* Used for some file attributes    */
:elsesegment
 typedef unsigned short mode_t; /* Used for some file attributes    */
:endsegment
#endif
