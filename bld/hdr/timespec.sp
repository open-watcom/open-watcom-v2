#ifndef _TIMESPEC_DEFINED
 #define _TIMESPEC_DEFINED
 struct timespec {
     __w_time_t tv_sec;
     long       tv_nsec;
 };
#endif /* _TIMESPEC_DEFINED */
