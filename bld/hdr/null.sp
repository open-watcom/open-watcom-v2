:: Old code used for versions up to 1.8
::#ifndef NULL
:: #if !defined(_M_I86) || defined(__SMALL__) || defined(__MEDIUM__)
::  #define NULL 0
:: #else
::  #define NULL 0L
:: #endif
::#endif
::
:: New code used after version 1.8
::
#ifndef NULL
 #ifdef __cplusplus
:segment DOS | QNX
  #if !defined(_M_I86) || defined(__SMALL__) || defined(__MEDIUM__)
:endsegment
   #define NULL 0
:segment DOS | QNX
  #else
   #define NULL 0L
  #endif 
:endsegment
 #else
  #define NULL ((void *)0)
 #endif
#endif
