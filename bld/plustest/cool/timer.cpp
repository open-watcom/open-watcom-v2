//
// Copyright (C) 1991 Texas Instruments Incorporated.
//
// Permission is granted to any individual or institution to use, copy, modify,
// and distribute this software, provided that this complete copyright and
// permission notice is maintained, intact, in all copies and supporting
// documentation.
//
// Texas Instruments Incorporated provides this software "as is" without
// express or implied warranty.
//
//
// Created: BMK 07/14/89 -- Initial design and implementation
// Updated: LGO 09/23/89 -- Conform to COOL coding style
// Updated: AFM 12/31/89 -- OS/2 port
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/12/92 -- added *sec_elapsed() funcs
//
// The CoolTimer class provides timing code  for performance evaluation.  This code
// was originally written by Joe Rahmeh at UT Austin.
//
//  User time:
//    time cpu spends in user mode on behalf of the program.
//  System time:
//    time cpu spends in system mode on behalf of the program.
//  Real time:
//    what you get from a stop watch timer.
//

#include <cool/Timer.h>

inline static long micro_sec_elapsed(clock_t start) {
 clock_t end = clock();
 if (end==clock_t(-1))  /* Processor time not available on this system */
   return 0;
 return long(double(end-start)/CLK_TCK*1000000.0);
}

inline static long milli_sec_elapsed(clock_t start) {
 clock_t end = clock();
 if (end==clock_t(-1))  /* Processor time not available on this system */
   return 0;
 return long(double(end-start)/CLK_TCK*1000.0);
}

void CoolTimer::mark () {
#if !defined (UNIXTIMER)
 usage0 = clock();
#else
 getrusage(0, &usage0);
 ftime(&real0);
#endif
}


long CoolTimer::real () {
#if !defined (UNIXTIMER)
 return milli_sec_elapsed(usage0);
#else
 long s, ms;
 timeb  real;        // current elapsed real time

 ftime(&real);
 s  = real.time    - real0.time;
 ms = real.millitm - real0.millitm;
 if(ms < 0)
   {ms += 1000;
    s--;
   }
 return 1000*s + ms;
#endif
}


long CoolTimer::user () {
#if !defined (UNIXTIMER)
 return milli_sec_elapsed(usage0);
#else
 register long dsec, dusec;
 rusage usage;       // current rusage structure

 getrusage(0, &usage);
 dsec  = usage.ru_utime.tv_sec  - usage0.ru_utime.tv_sec;
 dusec = usage.ru_utime.tv_usec - usage0.ru_utime.tv_usec;
 if(dusec < 0)
   {dusec += 1000000;
    dsec--;
   }
 return(dsec*1000 + dusec/1000);
#endif
}


long CoolTimer::system () {
#if !defined (UNIXTIMER)
 return(0L);
#else
 register long dsec, dusec;
 rusage usage;       // current rusage structure

 getrusage(0, &usage);
 dsec  = usage.ru_stime.tv_sec  - usage0.ru_stime.tv_sec;
 dusec = usage.ru_stime.tv_usec - usage0.ru_stime.tv_usec;
 if(dusec < 0)
   {dusec += 1000000;
    dsec--;
   }
 return(dsec*1000 + dusec/1000);
#endif
}


long CoolTimer::all () {
#if !defined (UNIXTIMER)
 return milli_sec_elapsed(usage0);
#else
 register long dsec, dusec;
 rusage usage;       // current rusage structure

 getrusage(0, &usage);
 dsec  = usage.ru_utime.tv_sec   + usage.ru_stime.tv_sec
       - usage0.ru_utime.tv_sec  - usage0.ru_stime.tv_sec;
 dusec = usage.ru_utime.tv_usec  + usage.ru_stime.tv_usec
       - usage0.ru_utime.tv_usec - usage0.ru_stime.tv_usec;
 if(dusec < 0)
   {dusec += 1000000;
    dsec--;
   }
 return(dsec*1000 + dusec/1000);
#endif
}


long CoolTimer::user_usec () {
#if !defined (UNIXTIMER)
 return micro_sec_elapsed(usage0);
#else
 register long dsec, dusec;
 rusage usage;       // current rusage structure

 getrusage(0, &usage);
 dsec  = usage.ru_utime.tv_sec  - usage0.ru_utime.tv_sec;
 dusec = usage.ru_utime.tv_usec - usage0.ru_utime.tv_usec;
 if(dusec < 0)
   {dusec += 1000000;
    dsec--;
   }
 return(dsec*1000000 + dusec);
#endif
}


long CoolTimer::system_usec () {
#if !defined (UNIXTIMER)
 return(0L);
#else
 register long dsec, dusec;
 rusage usage;       // current rusage structure

 getrusage(0, &usage);
 dsec  = usage.ru_stime.tv_sec  - usage0.ru_stime.tv_sec;
 dusec = usage.ru_stime.tv_usec - usage0.ru_stime.tv_usec;
 if(dusec < 0)
   {dusec += 1000000;
    dsec--;
   }
 return(dsec*1000000 + dusec);
#endif
}


long CoolTimer::all_usec() {
#if !defined (UNIXTIMER)
 return micro_sec_elapsed(usage0);
#else
 register long dsec, dusec;
 rusage usage;       // current rusage structure
 
 getrusage(0, &usage);
 dsec  = usage.ru_utime.tv_sec   + usage.ru_stime.tv_sec
       - usage0.ru_utime.tv_sec  - usage0.ru_stime.tv_sec;
 dusec = usage.ru_utime.tv_usec  + usage.ru_stime.tv_usec
       - usage0.ru_utime.tv_usec - usage0.ru_stime.tv_usec;
 if(dusec < 0)
   {dusec += 1000000;
    dsec--;
   }
 return(dsec*1000000 + dusec);
#endif
}
