::
:: POSIX id_t typedef declaration
::
#ifndef _ID_T_DEFINED_
 #define _ID_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef int            id_t;   /* Generic ID, may be uid, gid, pid */
:elsesegment ARCHMIPS
 typedef long           id_t;   /* Generic ID, may be uid, gid, pid */
:elsesegment
 #ifdef __386__
  typedef int            id_t;   /* Generic ID, may be uid, gid, pid */
 #elif defined(__MIPS__)
  typedef long           id_t;   /* Generic ID, may be uid, gid, pid */
 #endif
:endsegment
:elsesegment QNX
 typedef int            id_t;   /* Generic ID, may be uid, gid, pid */
:elsesegment
 typedef long           id_t;   /* Generic ID, may be uid, gid, pid */
:endsegment
#endif
