::
:: POSIX ino_t typedef declaration
::
#ifndef _INO_T_DEFINED_
 #define _INO_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned long  ino_t;  /* Used for file serial numbers     */
:elsesegment ARCHMIPS
 typedef unsigned long  ino_t;  /* Used for file serial numbers     */
:elsesegment
 #ifdef __386__
  typedef unsigned long  ino_t;  /* Used for file serial numbers     */
 #elif defined(__MIPS__)
  typedef unsigned long  ino_t;  /* Used for file serial numbers     */
 #endif
:endsegment
:elsesegment QNX
 typedef long           ino_t;  /* Used for file serial numbers     */
:elsesegment
 typedef unsigned int   ino_t;  /* i-node # type */
:endsegment
#endif
