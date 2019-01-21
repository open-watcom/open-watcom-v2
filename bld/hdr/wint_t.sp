::
:: POSIX, ISO C/C++ wint_t typedef declaration
::
:segment CNAME
#ifndef _STDWINT_T_DEFINED
#define _STDWINT_T_DEFINED
 namespace std {
   typedef wchar_t wint_t;
 }
 typedef std::wint_t __w_wint_t;
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDWINT_T_DEFINED
 #define _STDWINT_T_DEFINED
  namespace std {
    typedef wchar_t wint_t;
  }
  typedef std::wint_t __w_wint_t;
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
  typedef wint_t __w_wint_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
