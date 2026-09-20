::
:: POSIX ino_t typedef declaration
::
#ifndef _INO_T_DEFINED_
 #define _INO_T_DEFINED_
:segment LINUX
 typedef unsigned long       ino_t;   /* Used for file serial numbers     */
:include ext.sp
 typedef unsigned long long  ino64_t; /* Used for file serial numbers     */
:include extepi.sp
:elsesegment QNX
 typedef long           ino_t;  /* Used for file serial numbers     */
:elsesegment
 typedef unsigned int   ino_t;  /* i-node # type */
:endsegment
#endif
