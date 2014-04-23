:segment CNAME
#ifndef _STDWCTRANS_T_DEFINED
 #define _STDWCTRANS_T_DEFINED
 namespace std {
   typedef wchar_t wctrans_t;
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDWCTRANS_T_DEFINED
  #define _STDWCTRANS_T_DEFINED
  namespace std {
    typedef wchar_t wctrans_t;
  }
 #endif
 #ifndef _WCTRANS_T_DEFINED
 #define _WCTRANS_T_DEFINED
  #define _WCTRANS_T_DEFINED_
  using std::wctrans_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _WCTRANS_T_DEFINED
 #define _WCTRANS_T_DEFINED
  #define _WCTRANS_T_DEFINED_
  typedef unsigned short wctrans_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
