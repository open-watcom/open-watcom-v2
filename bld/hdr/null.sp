#ifndef NULL
:segment QNX
 #if defined(__SMALL__) || defined(__MEDIUM__) || defined(__386__)
:elsesegment
 #if defined(__SMALL__) || defined(__MEDIUM__) || defined(__386__) || defined(__AXP__) || defined(__PPC__)
:endsegment
  #define NULL   0
 #else
  #define NULL   0L
 #endif
#endif
