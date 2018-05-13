::
:: POSIX dev_t typedef declaration
::
#ifndef _DEV_T_DEFINED_
 #define _DEV_T_DEFINED_
:segment LINUX
:segment ARCHI386
 typedef unsigned long  dev_t;  /* Combination of node and device   */
:elsesegment ARCHMIPS
 typedef unsigned long  dev_t;  /* Combination of node and device   */
:elsesegment
 #ifdef __386__
  typedef unsigned long  dev_t;  /* Combination of node and device   */
 #elif defined(__MIPS__)
  typedef unsigned long  dev_t;  /* Combination of node and device   */
 #endif
:endsegment
:elsesegment QNX
 typedef long           dev_t;  /* Combination of node and device   */
:elsesegment
 typedef int            dev_t;  /* device code (drive #) */
:endsegment
#endif
