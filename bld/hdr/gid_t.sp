:segment LINUX
#ifndef _GID_T_DEFINED_
 #define _GID_T_DEFINED_
:segment ARCHI386
 typedef unsigned short gid_t;  /* Used for group IDs               */
:elsesegment ARCHMIPS
 typedef long           gid_t;  /* Used for group IDs               */
:elsesegment
 #if defined(__386__)
  typedef unsigned short gid_t; /* Used for group IDs               */
 #elif defined(__MIPS__)
  typedef long           gid_t; /* Used for group IDs               */
 #endif
:endsegment
#endif
:elsesegment QNX
#ifndef _GID_T_DEFINED_
#define _GID_T_DEFINED_
 typedef int            gid_t;  /* Used for group IDs               */
 typedef short          mgid_t; /* Used for group in messages       */
#endif
:elsesegment
typedef unsigned long   gid_t;  /* group identifier */
:endsegment
