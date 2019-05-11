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
:segment !CONLY
:segment !CNAME
#ifdef __cplusplus
:endsegment
:segment BITS16
 #if !defined(_M_I86) || defined(__SMALL__) || defined(__MEDIUM__)
:endsegment
  #define NULL 0
:segment BITS16
 #else
  #define NULL 0L
 #endif
:endsegment
:segment !CNAME
#else
:endsegment
:endsegment
:segment !CNAME
 #define NULL ((void *)0)
:segment !CONLY
#endif
:endsegment
:endsegment
#endif
