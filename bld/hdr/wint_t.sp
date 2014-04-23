:segment CNAME
#ifndef _STDWINT_T_DEFINED
#define _STDWINT_T_DEFINED
 namespace std {
   typedef wchar_t wint_t;
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDWINT_T_DEFINED
 #define _STDWINT_T_DEFINED
  namespace std {
    typedef wchar_t wint_t;
  }
 #endif
 #ifndef _WINT_T_DEFINED
 #define _WINT_T_DEFINED
  #define _WINT_T_DEFINED_
  using std::wint_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _WINT_T_DEFINED
 #define _WINT_T_DEFINED
  #define _WINT_T_DEFINED_
  typedef unsigned short wint_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
