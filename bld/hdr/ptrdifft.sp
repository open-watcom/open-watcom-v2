:segment CNAME
#ifndef _STDPTRDIFF_T_DEFINED
#define _STDPTRDIFF_T_DEFINED
 namespace std {
:segment BITS16
  #ifdef __HUGE__
   typedef long ptrdiff_t;
  #else
:endsegment
   typedef int ptrdiff_t;
:segment BITS16
  #endif
:endsegment
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDPTRDIFF_T_DEFINED
 #define _STDPTRDIFF_T_DEFINED
  namespace std {
:segment BITS16
   #ifdef __HUGE__
    typedef long ptrdiff_t;
   #else
:endsegment
    typedef int ptrdiff_t;
:segment BITS16
   #endif
:endsegment
  }
 #endif
 #ifndef _PTRDIFF_T_DEFINED
 #define _PTRDIFF_T_DEFINED
  #define _PTRDIFF_T_DEFINED_
  using std::ptrdiff_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _PTRDIFF_T_DEFINED
 #define _PTRDIFF_T_DEFINED
  #define _PTRDIFF_T_DEFINED_
:segment BITS16
  #ifdef __HUGE__
   typedef long ptrdiff_t;
  #else
:endsegment
   typedef int ptrdiff_t;
:segment BITS16
  #endif
:endsegment
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
