:segment CNAME
#ifndef _STDPTRDIFF_T_DEFINED
#define _STDPTRDIFF_T_DEFINED
 namespace std {
  #ifdef __HUGE__
   typedef long ptrdiff_t;
  #else
   typedef int ptrdiff_t;
  #endif
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDPTRDIFF_T_DEFINED
 #define _STDPTRDIFF_T_DEFINED
  namespace std {
   #ifdef __HUGE__
    typedef long ptrdiff_t;
   #else
    typedef int ptrdiff_t;
   #endif
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
  #ifdef __HUGE__
   typedef long ptrdiff_t;
  #else
   typedef int ptrdiff_t;
  #endif
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
