/* Unsigned integer type able to hold pointers. */
#ifndef _UINTPTR_T_DEFINED_
 #define _UINTPTR_T_DEFINED_
:segment DOS | QNX
 #if defined(_M_I86)
  #if defined(__SMALL__) || defined(__MEDIUM__)
   typedef unsigned int uintptr_t;
  #else
   typedef unsigned long uintptr_t;
  #endif
 #else /* 32 bit */
:endsegment
  #if defined(__COMPACT__) || defined(__LARGE__)
   typedef unsigned long long uintptr_t;
  #else
   typedef unsigned long uintptr_t;
  #endif
:segment DOS | QNX
 #endif
:endsegment
#endif
