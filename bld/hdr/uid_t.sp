::
:: POSIX uid_t typedef declaration
::
#ifndef _UID_T_DEFINED_
 #define _UID_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned short uid_t;  /* Used for user IDs                */
:elsesegment ARCHMIPS
 typedef long           uid_t;  /* Used for user IDs                */
:elsesegment
 #ifdef __386__
  typedef unsigned short uid_t; /* Used for user IDs                */
 #elif defined(__MIPS__)
  typedef long           uid_t; /* Used for user IDs                */
 #endif
:endsegment
:elsesegment QNX
 typedef int            uid_t;  /* Used for user IDs                */
 typedef short          muid_t; /* used in messages                 */
:elsesegment
 typedef long           uid_t;  /* user identifier */
:endsegment
#endif
