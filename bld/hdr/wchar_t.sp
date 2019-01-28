::
:: POSIX, ISO C wchar_t typedef declaration
::
:segment XLOCALE
#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
 #ifndef _WCHAR_T_DEFINED_
 #define _WCHAR_T_DEFINED_
  #ifdef X_LOCALE
   typedef unsigned long wchar_t;
  #else
   #ifdef __cplusplus
    typedef long char wchar_t;
   #else
    typedef unsigned short wchar_t;
   #endif
  #endif
 #endif
#else
 #ifndef _WCHAR_T_DEFINED_
 #define _WCHAR_T_DEFINED_
 #endif
#endif
:elsesegment
:segment !CNAME
:segment EXTWCHART
:include ext.sp
:endsegment
:segment !CONLY
#ifndef __cplusplus
:endsegment
 #ifndef _WCHAR_T_DEFINED
 #define _WCHAR_T_DEFINED
  #define _WCHAR_T_DEFINED_
  typedef unsigned short wchar_t;
 #endif
:segment !CONLY
#endif
:endsegment
:segment EXTWCHART
:include extepi.sp
:endsegment

:endsegment
:endsegment
