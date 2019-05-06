::
:: POSIX, ISO C/C++ uintptr_t typedef declaration
::
:: The type __w_uintptr_t is an alias for std::uintptr_t or uintptr_t as
:: appropriate. It is intended to simplify the standard headers.
::
/* Unsigned integer type able to hold pointers. */
:segment CNAME
#ifndef _STDUINTPTR_T_DEFINED
#define _STDUINTPTR_T_DEFINED
 namespace std {
:segment BITS16
 #ifdef _M_I86
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
:segment BITS16
 #endif
:endsegment
 }
:: The type __w_uintptr_t is an alias for std::uintptr_t or uintptr_t as
:: appropriate. It is intended to simplify the standard headers.
 typedef std::uintptr_t __w_uintptr_t;
#endif
:elsesegment
:segment !CONLY
:segment !CPPONLY
#ifdef __cplusplus
:endsegment
 #ifndef _STDUINTPTR_T_DEFINED
 #define _STDUINTPTR_T_DEFINED
  namespace std {
:segment BITS16
 #ifdef _M_I86
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
:segment BITS16
 #endif
:endsegment
  }
  typedef std::uintptr_t __w_uintptr_t;
 #endif
 #ifndef _UINTPTR_T_DEFINED
 #define _UINTPTR_T_DEFINED
  using std::uintptr_t;
 #endif
:segment !CPPONLY
#else  /* __cplusplus not defined */
:endsegment
:endsegment
:segment !CPPONLY
 #ifndef _UINTPTR_T_DEFINED
 #define _UINTPTR_T_DEFINED
:segment BITS16
 #ifdef _M_I86
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
:segment BITS16
 #endif
:endsegment
  typedef uintptr_t   __w_uintptr_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
:endsegment
