:segment CNAME
#ifndef _STDWCTYPE_T_DEFINED
#define _STDWCTYPE_T_DEFINED
 namespace std {
   typedef wchar_t wctype_t;
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDWCTYPE_T_DEFINED
 #define _STDWCTYPE_T_DEFINED
  namespace std {
    typedef wchar_t wctype_t;
  }
 #endif
 #ifndef _WCTYPE_T_DEFINED
 #define _WCTYPE_T_DEFINED
  #define _WCTYPE_T_DEFINED_
  using std::wctype_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _WCTYPE_T_DEFINED
 #define _WCTYPE_T_DEFINED
  #define _WCTYPE_T_DEFINED_
  typedef unsigned short wctype_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
