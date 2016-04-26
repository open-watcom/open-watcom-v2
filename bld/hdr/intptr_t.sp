/* Integer types able to hold *object* pointers. */
:segment DOS | QNX
#if defined(_M_I86)
 #if defined(__SMALL__) || defined(__MEDIUM__)
  typedef int intptr_t;
  typedef unsigned int uintptr_t;
 #else
  typedef long intptr_t;
  typedef unsigned long uintptr_t;
 #endif
#else /* 32 bit */
:endsegment
 #if defined(__COMPACT__) || defined(__LARGE__)
  typedef long long intptr_t;
  typedef unsigned long long uintptr_t;
 #else
  typedef long intptr_t;
  typedef unsigned long uintptr_t;
 #endif
:segment DOS | QNX
#endif
:endsegment
