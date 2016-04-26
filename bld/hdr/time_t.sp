:segment CNAME
#ifndef _STDTIME_T_DEFINED
#define _STDTIME_T_DEFINED
 namespace std {
:segment QNX | LINUX
   typedef signed long time_t;
:elsesegment
   typedef unsigned long time_t;
:endsegment
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDTIME_T_DEFINED
 #define _STDTIME_T_DEFINED
  namespace std {
:segment QNX | LINUX
    typedef signed long time_t;
:elsesegment
    typedef unsigned long time_t;
:endsegment
  }
 #endif
 #ifndef _TIME_T_DEFINED
 #define _TIME_T_DEFINED
  #define _TIME_T_DEFINED_
  using std::time_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _TIME_T_DEFINED
 #define _TIME_T_DEFINED
  #define _TIME_T_DEFINED_
:segment QNX | LINUX
  typedef signed long time_t;
:elsesegment
  typedef unsigned long time_t;
:endsegment
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
