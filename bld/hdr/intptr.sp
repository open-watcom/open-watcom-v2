/* Integer types able to hold *object* pointers. */
#if defined(_M_I86)
 #if defined(__COMPACT__) || defined(__LARGE__) || defined(__HUGE__)
  typedef long intptr_t;
  typedef unsigned long uintptr_t;
 #else
  typedef int intptr_t;
  typedef unsigned int uintptr_t;
 #endif
#else /* 32 bit */
 #if defined(__COMPACT__) || defined(__LARGE__)
  typedef long long intptr_t;
  typedef unsigned long long uintptr_t;
 #else
  typedef long intptr_t;
  typedef unsigned long uintptr_t;
 #endif
#endif
