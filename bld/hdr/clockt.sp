:: Open Watcom only defines clock_t in time.h/ctime. However, the legacy
:: OW declaration was protected by an inclusion guard that defined the
:: symbol _CLOCK_T_DEFINED. This was done for compatibility with MS
:: Visual C. When I upgraded time.mh to support the generation of ctime,
:: I created this file. This complexity is technically unnecessary for
:: Open Watcom, but this is the correct way to handle types defined with
:: inclusion guards. For consistency with size_t, time_t, and other similar
:: types, I felt it appropriate to configure clock_t in this way as well.
::
:: PeterC
::
:segment CNAME
#ifndef _STDCLOCK_T_DEFINED
#define _STDCLOCK_T_DEFINED
 namespace std {
   typedef unsigned long clock_t;
 }
#endif
:elsesegment
:segment !CONLY
#ifdef __cplusplus
 #ifndef _STDCLOCK_T_DEFINED
 #define _STDCLOCK_T_DEFINED
  namespace std {
    typedef unsigned long clock_t;
  }
 #endif
 #ifndef _CLOCK_T_DEFINED
 #define _CLOCK_T_DEFINED
  using std::clock_t;
 #endif
#else  /* __cplusplus not defined */
:endsegment
 #ifndef _CLOCK_T_DEFINED
 #define _CLOCK_T_DEFINED
  typedef unsigned long clock_t;
 #endif
:segment !CONLY
#endif /* __cplusplus not defined */
:endsegment
:endsegment
