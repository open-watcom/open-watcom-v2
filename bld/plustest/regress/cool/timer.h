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
// Updated: JAM 08/12/92 -- made standard functions by default, define
//                          UNIXTIMER to use sys/ #includes and features
//
// The Timer class provides timing code  for performance evaluation.  This code
// was originally written by Joe Rahmeh at UT Austin.
//
// FUTURE
// JAM -- Want to also remove <sys/timeb.h> or at least make POSIX
//


#include <time.h>    // standard (time_t)
#if defined(UNIXTIMER)
#include <sys/timeb.h>  // not standard, but UNIX and MSDOS (timeb,ftime())
#include <sys/types.h>
#include <sys/time.h>   // Shouldn't this be just <time.h>?
#include <sys/resource.h>
#endif   // extra UNIX functions and functionality


class CoolTimer {
public:
  CoolTimer () {mark();}        // constructor
  
  void mark ();         // mark timer
  
  long user ();         // user        time (ms) since last Mark
  long system ();       // system      time (ms) since last Mark
  long all ();          // user+system time (ms) since last Mark
  long real ();         // real        time (ms) since last Mark
  
  long user_usec ();    // user        time (us) since last Mark
  long system_usec ();  // system      time (us) since last Mark
  long all_usec ();     // user+system time (us) since last Mark

private:
#if !defined(UNIXTIMER)    // standard functions
  clock_t usage0;
#else
  rusage  usage0;       // rusage structure at last mark
  timeb   real0;        // elapsed real time at last mark
#endif
};

