::
:: POSIX gid_t typedef declaration
::
#ifndef _GID_T_DEFINED_
 #define _GID_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned short gid_t;  /* Used for group IDs               */
:elsesegment ARCHMIPS
 typedef long           gid_t;  /* Used for group IDs               */
:elsesegment
 #ifdef __386__
  typedef unsigned short gid_t; /* Used for group IDs               */
 #elif defined(__MIPS__)
  typedef long           gid_t; /* Used for group IDs               */
 #endif
:endsegment
:elsesegment QNX
 typedef int            gid_t;  /* Used for group IDs               */
 typedef short          mgid_t; /* Used for group in messages       */
:elsesegment
 typedef short          gid_t;  /* group identifier */
:endsegment
#endif
