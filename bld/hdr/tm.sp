:segment DECLARE_STRUCT
:include nsstd.sp
struct tm {
    int  tm_sec;    /* seconds after the minute -- [0,61] */
    int  tm_min;    /* minutes after the hour   -- [0,59] */
    int  tm_hour;   /* hours after midnight     -- [0,23] */
    int  tm_mday;   /* day of the month         -- [1,31] */
    int  tm_mon;    /* months since January     -- [0,11] */
    int  tm_year;   /* years since 1900                   */
    int  tm_wday;   /* days since Sunday        -- [0,6]  */
    int  tm_yday;   /* days since January 1     -- [0,365]*/
    int  tm_isdst;  /* Daylight Savings Time flag */
};
:include nsstdepi.sp
:endsegment

:segment CNAME
#ifndef _STDTM_DEFINED
#define _STDTM_DEFINED
 #define _STDTM_DEFINED_
:elsesegment
#ifndef _TM_DEFINED
#define _TM_DEFINED
 #define _TM_DEFINED_
:endsegment
:segment !DECLARE_STRUCT
:segment CNAME
 namespace std {
  struct tm;
 }
:elsesegment
 struct tm;
:endsegment
:endsegment
:segment CNAME
 typedef struct std::tm __w_tm;
#endif
:elsesegment
 typedef struct tm __w_tm;
#endif
:endsegment
