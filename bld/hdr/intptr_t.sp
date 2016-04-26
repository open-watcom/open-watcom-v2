/* Integer type able to hold pointers. */
#ifndef _INTPTR_T_DEFINED_
 #define _INTPTR_T_DEFINED_
:segment DOS | QNX
 #if defined(_M_I86)
  #if defined(__SMALL__) || defined(__MEDIUM__)
   typedef int intptr_t;
  #else
   typedef long intptr_t;
  #endif
 #else /* 32 bit */
:endsegment
  #if defined(__COMPACT__) || defined(__LARGE__)
   typedef long long intptr_t;
  #else
   typedef long intptr_t;
  #endif
:segment DOS | QNX
 #endif
:endsegment
#endif
