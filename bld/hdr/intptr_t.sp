::
:: POSIX, ISO C/C++ intptr_t typedef declaration
::
:: The type __w_intptr_t is an alias for std::intptr_t or intptr_t as
:: appropriate. It is intended to simplify the standard headers.
::
/* Signed integer type able to hold pointers. */
:segment CNAME
#ifndef _STDINTPTR_T_DEFINED
#define _STDINTPTR_T_DEFINED
 namespace std {
:segment BITS16
 #ifdef _M_I86
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
:segment BITS16
 #endif
:endsegment
 }
 typedef std::intptr_t __w_intptr_t;
#endif
:elsesegment
:segment !CONLY
:segment !CPPONLY
#ifdef __cplusplus
:endsegment
 #ifndef _STDINTPTR_T_DEFINED
 #define _STDINTPTR_T_DEFINED
  namespace std {
:segment BITS16
 #ifdef _M_I86
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
:segment BITS16
 #endif
:endsegment
  }
  typedef std::intptr_t __w_intptr_t;
 #endif
 #ifndef _INTPTR_T_DEFINED
 #define _INTPTR_T_DEFINED
  using std::intptr_t;
 #endif
:segment !CPPONLY
#else  /* __cplusplus not defined */
:endsegment
:endsegment
:segment !CPPONLY
 #ifndef _INTPTR_T_DEFINED
 #define _INTPTR_T_DEFINED
:segment BITS16
 #ifdef _M_I86
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
:segment BITS16
 #endif
:endsegment
  typedef intptr_t  __w_intptr_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
:endsegment
