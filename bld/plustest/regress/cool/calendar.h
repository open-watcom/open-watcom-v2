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
// Created: MBN 04/11/89 -- Initial design and implementation
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/24/92 -- made static array global and moved to .c
// Updated: JAM 08/24/92 -- made #define constants 'const' objects
//
// This file  contains an enum  definitions  for types day_of_week and months.
// It specifies symbolic constants to be used  as day  and month types.   Note
// that C++  does not distinguish between   an enum  type  and an integer,  so
// function arguments of type `day_of_week' and `month'  are really just `int'
// types.  In addition, ASCII string representations for the  days of the week
// and months  are  provided  in English only.   These can  be  indexed by the
// symbolic enums of the appropriate type.
//
// **NOTE**
// No international  string or  symbolic  representations  are   yet provided.
// These should be added later and can be indexed by the country type.
//

#ifndef CALENDARH               // If we have not yet defined the CALENDAR type,
#define CALENDARH               // Indicate that type CALENDAR has been included

enum day_of_week {                              // Define days type
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY 
  };

enum months {                                   // Define months type
  JANUARY,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
  };

extern const char* const day_names[];           // Define ASCII day names

extern const char* const month_names[]; // Define ASCII month names

const long SECOND = 1;
const long MINUTE = 60L*SECOND;
const long HOUR = MINUTE*60L;
const long DAY = HOUR*24L;
const long WEEK = DAY*7L;
const long YEAR = WEEK*52L;

inline int IS_LEAP_YEAR(int y)
   { return (y % 4) == 0 && (y % 100) != 0 || (y % 400) == 0; }

#endif                                          // End #ifdef of CALENDARH
