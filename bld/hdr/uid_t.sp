:segment LINUX
#ifndef _UID_T_DEFINED_
 #define _UID_T_DEFINED_
:segment ARCHI386
 typedef unsigned short uid_t;  /* Used for user IDs                */
:elsesegment ARCHMIPS
 typedef long           uid_t;  /* Used for user IDs                */
:elsesegment
 #if defined(__386__)
  typedef unsigned short uid_t; /* Used for user IDs                */
 #elif defined(__MIPS__)
  typedef long           uid_t; /* Used for user IDs                */
 #endif
:endsegment
#endif
:elsesegment QNX
#ifndef _UID_T_DEFINED_
#define _UID_T_DEFINED_
 typedef int            uid_t;  /* Used for user IDs                */
 typedef short          muid_t; /* used in messages                 */
#endif
:elsesegment
typedef unsigned long   uid_t;  /* user identifier */
:endsegment
