:segment CNAME
#ifndef _STDMBSTATE_T_DEFINED
#define _STDMBSTATE_T_DEFINED
 namespace std {
   typedef int mbstate_t;
 }
 typedef std::mbstate_t __w_mbstate_t;
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDMBSTATE_T_DEFINED
 #define _STDMBSTATE_T_DEFINED
  namespace std {
    typedef int mbstate_t;
  }
  typedef std::mbstate_t __w_mbstate_t;
 #endif
 #ifndef _MBSTATE_T_DEFINED
 #define _MBSTATE_T_DEFINED
  #define _MBSTATE_T_DEFINED_
  using std::mbstate_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _MBSTATE_T_DEFINED
 #define _MBSTATE_T_DEFINED
  #define _MBSTATE_T_DEFINED_
  typedef int mbstate_t;
  typedef mbstate_t __w_mbstate_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
