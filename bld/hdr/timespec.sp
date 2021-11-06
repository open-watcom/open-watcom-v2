#ifndef _TIMESPEC_DEFINED
 #define _TIMESPEC_DEFINED
:segment DOS | IBMTOOLKIT
:segment DOS

#ifdef __OS2__
:endsegment
:include pshpack4.sp
 struct timespec {
   union {
     __w_time_t tv_sec;
     __w_time_t ts_sec;
   };
   union {
     long       tv_nsec;
     long       ts_nsec;
   };
 };
:include poppack.sp
:segment DOS
#else
:endsegment
:endsegment
:segment !IBMTOOLKIT
 struct timespec {
     __w_time_t tv_sec;
     long       tv_nsec;
 };
:endsegment
:segment DOS
#endif

:endsegment
#endif /* _TIMESPEC_DEFINED */
