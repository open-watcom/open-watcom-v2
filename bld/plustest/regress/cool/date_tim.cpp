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
// Updated: MNF 07/25/89 -- Add the parse member function 
// Updated: MBN 09/05/89 -- Added conditional exception handling
// Updated: MBN 01/03/90 -- Adjusted strfind() calls to match new syntax
// Updated: MBN 01/17/90 -- Fixed parsing algorithms to be GMT relative
// Updated: MBN 02/06/90 -- Support years prior to the epoch (1/1/1970)
// Updated: MBN 02/12/90 -- Changed all ascii() functions to return const char*
// Updated: MBN 02/13/90 -- Made ascii_duration() a member function for epoch
// Updated: MJF 03/12/90 -- Added group names to RAISE
// Updated: MJF 01/17/91 -- Fixed parse to use ANSI function mktime()
// Updated: DAN 01/21/91 -- Added adjust_dst() for OS/2 DST adjustment.
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/12/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/12/92 -- added defs for static data members
// Updated: JAM 08/12/92 -- removed timelocal()/mktime() defs (undid OS/2&SUN hacks)
// Updated: JAM 10/03/92 -- removed "delete t" in funcs because ANSI C
//                          localtime() returns pointer *static* |struct tm|
//
// This file  contains member and friend function  implementation  code for the
// CoolDate_Time class defined  in the Date_Time.h header file.   Where appropriate
// and possible, interfaces to,  and us of, existing  system functions has been
// incorporated.  An overview  of the CoolDate_Time   class structor  along with  a
// synopsis of each member and friend function, can be found in the Date_Time.h
// header file.  Two static  char* tables are defined  within  this  file to be
// used only by member functions of the CoolDate_Time class.  The first is the date
// format table that is indexed by the enum  symbolic values in the <country.h>
// header  file.  This table contains  format  strings for  sscanf()   used  to
// implement the  formatted  ASCII  date output according  to  the rules of the
// appropriate country.  The second is the time table that is  again indexed by
// the enum symbolic values  in the <country.h> header  file and   performs the
// same function for sscanf() output of the time.
//

#ifndef DATETIMEH               // If DateTime class has not been defined
#include <cool/Date_Time.h>     // Include class specification header file
#endif

#include <ctype.h>              // ANSI C character macros
#include <stdlib.h>             // Include standard c library support


static int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static const char* date_format_s[] = {          // Date formatting table
"* UNKNOWN *",                                  // Unknown country code
"%02d-%02d-%4d",                                // 03-29-1989 UNITED STATES   
"%4d-%02d-%02d",                                // 1989-29-03 FRENCH CANADIAN 
"%02d-%02d/%4d",                                // 03/29/1989 LATIN AMERICA   
"%02d-%02d-%4d",                                // 03-29-1989 NETHERLANDS     
"%02d-%02d/%4d",                                // 03/29/1989 BELGIUM         
"%02d-%02d/%4d",                                // 03/29/1989 FRANCE          
"%02d-%02d/%4d",                                // 03/29/1989 SPAIN           
"%02d-%02d/%4d",                                // 03/29/1989 ITALY           
"%02d-%02d.%4d",                                // 03.29.1989 SWITZERLAND     
"%02d-%02d-%4d",                                // 29-03-1989 UNITED KINGDOM  
"%02d-%02d/%4d",                                // 03/29/1989 DENMARK         
"%4d-%02d-%02d",                                // 1989-29-03 SWEDEN          
"%02d/%02d/%4d",                                // 03/29/1989 NORWAY          
"%02d.%02d.%4d",                                // 03.29.1989 GERMANY         
"%02d/%02d/%4d",                                // 03/29/1989 PORTUGAL        
"%02d.%02d.%4d",                                // 03.29.1989 FINLAND         
"%02d/%02d/%4d",                                // 03/29/1989 ARABIC COUNTRIES
"%02d %02d %4d"                                 // 03 29 1989 ISRAEL          
};

static const char* time_format_s[] = {          // Time formatting table
"* UNKNOWN *",                                  // Unknown country code
"%02d:%02d:%02d %s",                            // 17:35:00 UNITED STATES   
"%02d:%02d:%02d %s",                            // 17:35:00 FRENCH CANADIAN 
"%02d:%02d:%02d %s",                            // 17:35:00 LATIN AMERICA   
"%02d:%02d:%02d %s",                            // 17:35:00 NETHERLANDS     
"%02d:%02d:%02d %s",                            // 17:35:00 BELGIUM         
"%02d:%02d:%02d %s",                            // 17:35:00 FRANCE          
"%02d:%02d:%02d %s",                            // 17:35:00 SPAIN           
"%02d:%02d:%02d %s",                            // 17:35:00 ITALY           
"%02d.%02d.%02d %s",                            // 17.35.00 SWITZERLAND     
"%02d:%02d:%02d %s",                            // 17:35:00 UNITED KINGDOM  
"%02d:%02d:%02d %s",                            // 17:35:00 DENMARK         
"%02d.%02d.%02d %s",                            // 17.35.00 SWEDEN          
"%02d:%02d:%02d %s",                            // 17:35:00 NORWAY          
"%02d.%02d.%02d %s",                            // 17.35.00 GERMANY         
"%02d:%02d:%02d %s",                            // 17:35:00 PORTUGAL        
"%02d.%02d.%02d %s",                            // 17.35.00 FINLAND         
"%02d:%02d:%02d %s",                            // 17:35:00 ARABIC COUNTRIES
"%02d:%02d:%02d %s"                             // 17:35:00 ISRAEL          
};

#define TIME_VALUES this->dt.tm_hour, this->dt.tm_min, this->dt.tm_sec, \
            tz_table[this->tz_code]
#define MONTH_DATE_VALUES this->dt.tm_mon+1, this->dt.tm_mday, \
            this->get_year (), tz_table[this->tz_code]
#define DATE_MONTH_VALUES this->dt.tm_mday, this->dt.tm_mon+1, \
            this->get_year (), tz_table[this->tz_code]
#define YEAR_MONTH_VALUES this->get_year(), this->dt.tm_mday, \
            this->dt.tm_mon+1, tz_table[this->tz_code]

// default_tz_code_s -- Default time zone
time_zone CoolDate_Time::default_tz_code_s;

// default_c_code_s -- Default country
country CoolDate_Time::default_c_code_s;

// adjust_tz -- Calculate adjustment for time zone from GMT in seconds
// Input:       None
// Output:      None

void CoolDate_Time::adjust_tz () {
  long local_secs = 0;
  tm* t;                                        // Temporary variable
  time_t sys_seconds = time ((time_t*)0);               // System GMT time in seconds
  set_tz (this->tz_code);                       // Setup local time conversion
  t = localtime (&sys_seconds);                 // Convert to local time
  int i;
  for (i = 70; i < t->tm_year; i++) {
    local_secs += (YEAR + DAY);
    if (IS_LEAP_YEAR (i)) local_secs += DAY;
  }
  for (i = 0; i < t->tm_mon; i++) {
    local_secs += (DAY * days_in_month[i]);
    if (i == FEBRUARY && IS_LEAP_YEAR (t->tm_year))
      local_secs += DAY;
  }
  t->tm_mday--;
  local_secs += (DAY * t->tm_mday);
  local_secs += (HOUR * t->tm_hour);
  local_secs += (MINUTE * t->tm_min);
  local_secs += t->tm_sec;
  this->time_adjust = sys_seconds - local_secs; // TZ offset in seconds
}


// adjust_year -- Calculate offset from epoch to support years before 1/1/1970
// Input:         None
// Output:        None
//
// The CoolDate_Time class is  based upon the ANSI  C time(2) function. This
// calculates time/date in seconds since the  epoch (1/1/1970). To allow
// the CoolDate_Time class to support dates before the epoch we need to find
// which year  after 1970 has the same  day of week/dates correlation as
// the one specified. To determine this, we make use of the  fact that a
// yearly periodic cycle of 28 years exists such that 1/1/1970 fell on a
// Thursday and 1/1/1998 is also a Thursday.
//

void CoolDate_Time::adjust_year (long& year) {
  if (year > 99 && year < 1970) {
    this->year_adjust = int(1970 - year);
    year += NEW_YEAR (this->year_adjust);
  }
  else if (year < 70) {
    this->year_adjust = int(70 - year);
    year += NEW_YEAR (this->year_adjust);
  }
  else
    this->year_adjust = 0;
}


// set_tz -- Set the local time zone before conversion
//           of object from GMT seconds to tm structure
// Input:    Time zone
// Output:   None

void set_tz (time_zone tz) {
  static char tz_name[50];                              // Temporary variable
  strcpy (tz_name, "TZ=");                      // TZ envionmental symbol
  strcpy (&tz_name[3], tz_table[tz]);           // Concatenate time zone name
  putenv (tz_name);                             // Set environment TZ variable
}


// resolve -- This private method takes a long representing the
//            number of seconds since 01/01/1970 00:00:00 GMT
//            and converts that into local time, placing the 
//            result into the date/time structure
// Input:     this*
// Output:    None -- date/time structure updated

void CoolDate_Time::resolve () {
  tm* t;                                        // Temporary variable
  set_tz (this->tz_code);                       // Set local time zone
  adjust_tz ();                                 // Calculate time zone offset
  t = localtime (&(this->time_seconds));        // Convert to local time zone
  this->dt.tm_sec = t->tm_sec;                  // Copy seconds value
  this->dt.tm_min = t->tm_min;                  // Copy minutes value
  this->dt.tm_hour = t->tm_hour;                // Copy hours value
  this->dt.tm_mday = t->tm_mday;                // Copy day of month value
  this->dt.tm_mon = t->tm_mon;                  // Copy month value
  this->dt.tm_year = t->tm_year;                // Copy year value
  this->dt.tm_wday = t->tm_wday;                // Copy day of week value
  this->dt.tm_yday = t->tm_yday;                // Copy day of year value
  this->dt.tm_isdst = t->tm_isdst;              // Copy day light savings flag
}


// CoolDate_Time() -- Simple constructor for empty CoolDate_Time object
// Input:         None
// Output:        CoolDate_Time reference

CoolDate_Time::CoolDate_Time () {
  this->time_seconds = time((time_t*)0);                // Zero seconds
#ifdef ERROR_CHECKING
  if (this->default_c_code_s == NULL) {         // If no country code set
    //RAISE (Warning, SYM(CoolDate_Time), SYM(No_C_Code),
    printf ("CoolDate_Time::CoolDate_Time(): Default country code not set but used.\n");
  }
  if (this->default_tz_code_s == NULL) {        // If no time zone code set
    //RAISE (Warning, SYM(CoolDate_Time), SYM(No_Tz_Code),
    printf ("CoolDate_Time::CoolDate_Time(): Default time zone code not set but used.\n");
  }
#endif
  this->c_code = this->default_c_code_s;        // Set country code
  this->tz_code = this->default_tz_code_s;      // Set time zone code
  set_tz (default_tz_code_s);                   // Set local time zone
}


// CoolDate_Time(tz,c) -- Simple constructor for empty CoolDate_Time object
//                    that also sets the time zone and country
// Input:             Time zone, country codes
// Output:            CoolDate_Time reference

CoolDate_Time::CoolDate_Time (time_zone tz, country c) {
  this->time_seconds = time((time_t*)0);                // Zero seconds
  this->c_code = c;                             // Set country code
  this->tz_code = tz;                           // Set time zone code
  set_tz (tz);                                  // Set local time zone
}


// CoolDate_Time(const CoolDate_Time&) -- Constructor to duplicate a CoolDate_Time object
// Input:                        CoolDate_Time reference
// Output:                       CoolDate_Time reference

CoolDate_Time::CoolDate_Time (const CoolDate_Time& d) {
  this->time_seconds = d.time_seconds;
  this->tz_code = d.tz_code;
  this->c_code = d.c_code;
  this->dt.tm_sec = d.dt.tm_sec;
  this->dt.tm_min = d.dt.tm_min;
  this->dt.tm_hour = d.dt.tm_hour;
  this->dt.tm_mday = d.dt.tm_mday;
  this->dt.tm_mon = d.dt.tm_mon;
  this->dt.tm_year = d.dt.tm_year;
  this->dt.tm_wday = d.dt.tm_wday;
  this->dt.tm_yday = d.dt.tm_yday;
  this->dt.tm_isdst = d.dt.tm_isdst;
  this->time_adjust = d.time_adjust;
  this->year_adjust = d.year_adjust;
  this->century = d.century;
}


// ~CoolDate_Time -- CoolDate_Time class destructor
// Input:        this*
// Output:       None

CoolDate_Time::~CoolDate_Time () { 
}
                                       

// set_local_time -- Sets the time zone and DST adjusted local time
// Input:            this*
// Output:           CoolDate_Time reference with slots updated for local time

void CoolDate_Time::set_local_time () {
  tm* t;                                        // Temporary pointer variable
  time_t sys_seconds = time ((time_t*)0);               // System GMT time in seconds
  this->time_seconds = sys_seconds;             // Save time for future use
  set_tz (this->tz_code);                       // Setup local time conversion
  t = localtime (&sys_seconds);                 // Convert secs to local time
  this->dt.tm_sec = t->tm_sec;                  // Copy seconds value
  this->dt.tm_min = t->tm_min;                  // Copy minutes value
  this->dt.tm_hour = t->tm_hour;                // Copy hours value
  this->dt.tm_mday = t->tm_mday;                // Copy day of month value
  this->dt.tm_mon = t->tm_mon;                  // Copy month value
  this->dt.tm_year = t->tm_year;                // Copy year value
  this->dt.tm_wday = t->tm_wday;                // Copy day of week value
  this->dt.tm_yday = t->tm_yday;                // Copy day of year value
  this->dt.tm_isdst = t->tm_isdst;              // Copy day light savings flag
  this->year_adjust = 0;
  this->century = 1900;
}


// set_gm_time -- Set the Greenwich Mean Time
// Input:         this*
// Output:        CoolDate_Time reference with slots updated for GMT

void CoolDate_Time::set_gm_time () {
  tm* t;                                        // Temporary pointer variable
  time_t sys_seconds = time ((time_t*)0);               // System GMT time in seconds
  this->tz_code = GB_EIRE;                      // GMT time zone
  set_tz (this->tz_code);                       // Setup GMT conversion
  t = localtime (&sys_seconds);                 // Convert seconds to local time
  this->dt.tm_sec = t->tm_sec;                  // Copy seconds value
  this->dt.tm_min = t->tm_min;                  // Copy minutes value
  this->dt.tm_hour = t->tm_hour;                // Copy hours value
  this->dt.tm_mday = t->tm_mday;                // Copy day of month value
  this->dt.tm_mon = t->tm_mon;                  // Copy month value
  this->dt.tm_year = t->tm_year;                // Copy year value
  this->dt.tm_wday = t->tm_wday;                // Copy day of week value
  this->dt.tm_yday = t->tm_yday;                // Copy day of year value
  this->dt.tm_isdst = t->tm_isdst;              // Copy day light savings flag
  this->year_adjust = 0;
  this->century = 1900;
}


// operator= -- Assignment operator to duplicate a CoolDate_Time object
// Input:       CoolDate_Time reference
// Output:      CoolDate_Time reference

CoolDate_Time& CoolDate_Time::operator= (const CoolDate_Time& d) {
  this->time_seconds = d.time_seconds;
  this->tz_code = d.tz_code;
  this->c_code = d.c_code;
  this->dt.tm_sec = d.dt.tm_sec;
  this->dt.tm_min = d.dt.tm_min;
  this->dt.tm_hour = d.dt.tm_hour;
  this->dt.tm_mday = d.dt.tm_mday;
  this->dt.tm_mon = d.dt.tm_mon;
  this->dt.tm_year = d.dt.tm_year;
  this->dt.tm_wday = d.dt.tm_wday;
  this->dt.tm_yday = d.dt.tm_yday;
  this->dt.tm_isdst = d.dt.tm_isdst;
  this->time_adjust = d.time_adjust;
  this->year_adjust = d.year_adjust;
  this->century = d.century;
  return *this;
}


// operator+= -- Add a time interval to a CoolDate_Time object, assign
//               the result, and update the tm structure
// Input:        "n" seconds representing time interval
// Output:       CoolDate_Time reference

CoolDate_Time& CoolDate_Time::operator+= (long n) {
  this->time_seconds += n;
  this->resolve ();
  return (*this);
}


// operator-= -- Subtract a time interval from a CoolDate_Time object,
//               assign the result, and update the tm structure
// Input:        "n" seconds representing time interval
// Output:       CoolDate_Time reference

CoolDate_Time& CoolDate_Time::operator-= (long n) {
  this->time_seconds -= n;
  this->resolve ();
  return (*this);
}


// operator>> -- Input CoolDate_Time object from input stream. If a time zone is
//               also entered, it will be set too.
// Input:        Input stream reference, CoolDate_Time reference
// Output:       CoolDate_Time reference

istream& operator>> (istream& is, CoolDate_Time& d) {
  char* s = "";                                 // Temporary pointer
  is.get(s,256);                                // Read upto 256 characters
  d.parse(s,1);                                 // Parse input string
  return is;                                    // Return stream object
}


// incr_sec -- Advance Date/Time by "n" second(s)
// Input:      Number of seconds to advance (default 1)
// Output:     None -- CoolDate_Time object updated 

void CoolDate_Time::incr_sec (int n) {
  this->time_seconds += n;
  this->resolve ();
}


// incr_min -- Advance Date/Time by "n" minute(s)
// Input:      Number of minutes to advance (default 1)
// Output:     None -- CoolDate_Time object updated 

void CoolDate_Time::incr_min (int n) {
  this->time_seconds += (MINUTE * n);
  this->resolve ();
}


// incr_hour -- Advance Date/Time by "n" hour(s)
// Input:       Number of hours to advance (default 1)
// Output:      None -- CoolDate_Time object updated 

void CoolDate_Time::incr_hour (int n) {
  this->time_seconds += (HOUR * n);
  this->resolve ();
}


// incr_day -- Advance Date/Time by "n" day(s)
// Input:      Number of days to advance (default 1)
// Output:     None -- CoolDate_Time object updated 

void CoolDate_Time::incr_day (int n) {
  this->time_seconds += (DAY * n);
  this->resolve ();
}


// incr_week -- Advance Date/Time by "n" week(s)
// Input:       Number of weeks to advance (default 1)
// Output:      None -- CoolDate_Time object updated 

void CoolDate_Time::incr_week (int n) {
  this->time_seconds += (WEEK * n);
  this->resolve ();
}


// incr_month -- Advance Date/Time by "n" month(s)
// Input:        Number of months to advance (default 1)
// Output:       None -- CoolDate_Time object updated

void CoolDate_Time::incr_month (int n) {
  int temp_year = this->dt.tm_year;
  int temp_month = this->dt.tm_mon;
  for (int i = 0; i < n; i++, temp_month++) {
    if (temp_month > DECEMBER) {
      temp_month = JANUARY;
      temp_year++;
    }
    time_seconds += (DAY * days_in_month[temp_month]);
    if (temp_month == FEBRUARY && IS_LEAP_YEAR (temp_year))
      time_seconds += DAY;
  }
  this->resolve ();
}


// incr_year -- Advance Date/Time by "n" year(s)
// Input:       Number of years to advance (default 1)
// Output:      None -- CoolDate_Time object updated 

void CoolDate_Time::incr_year (int n) {
  int temp_year = this->dt.tm_year;
  for (int i = 0; i < n; i++, temp_year++) {
    time_seconds += (YEAR+DAY);
    if ((IS_LEAP_YEAR (temp_year) && (this->dt.tm_mon < FEBRUARY || 
        (this->dt.tm_mon == FEBRUARY && this->dt.tm_mday != 29))) ||
        (IS_LEAP_YEAR (temp_year+1) && this->dt.tm_mon > FEBRUARY))
      time_seconds += DAY;
  }
  this->resolve ();
}


// decr_month -- Retreat Date/Time by "n" month(s)
// Input:        Number of months to retreat (default 1)
// Output:       None -- CoolDate_Time object updated

void CoolDate_Time::decr_month (int n) {
  int temp_year = this->dt.tm_year;
  int temp_month = this->dt.tm_mon;
  for (int i = 0; i < n; i++, temp_month--) {
    if (temp_month < JANUARY) {
      temp_month = DECEMBER;
      temp_year--;
    }
    time_seconds -= (DAY * days_in_month[temp_month]);
    if (temp_month == FEBRUARY && IS_LEAP_YEAR (temp_year))
      time_seconds -= DAY;
  }
  this->resolve ();
}


// decr_year -- Retreat Date/Time by "n" year(s)
// Input:       Number of years to retreat (default 1)
// Output:      None -- CoolDate_Time object updated 

void CoolDate_Time::decr_year (int n) {
  int temp_year = this->dt.tm_year;
  for (int i = 0; i < n; i++, temp_year--) {
    time_seconds -= (YEAR+DAY);
    if ((IS_LEAP_YEAR (temp_year) && (this->dt.tm_mon > FEBRUARY ||
        (this->dt.tm_mon == FEBRUARY && this->dt.tm_mday == 29))) ||
        (IS_LEAP_YEAR (temp_year-1) && this->dt.tm_mon <= FEBRUARY))
      time_seconds -= DAY;
  }
  this->resolve ();
}


// start_min -- Increment CoolDate_Time object to start of "n" minutes
// Input:       Number of minutes to advance to start (default 1)
//              where time is set to HH:MM:00
// Output:      None -- CoolDate_Time object updated

void CoolDate_Time::start_min (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec);
  incr_min (n-1);
}


// end_min -- Increment CoolDate_Time object to end of "n" minutes
//            time is set to HH:MM:59
// Input:     Number of minutes to advance to end (default 1)
// Output:    None -- CoolDate_Time object updated

void CoolDate_Time::end_min (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec -1);
  incr_min (n-1);
}


// start_hour -- Increment CoolDate_Time object to start of "n" hours
// Input:        Number of hours to advance to start (default 1)
//               where time is set to HH:00:00
// Output:       None -- CoolDate_Time object updated

void CoolDate_Time::start_hour (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  incr_hour (n-1);
}


// end_hour --   Increment CoolDate_Time object to end of "n" hours
//               time is set to HH:59:59
// Input:        Number of hours to advance to end (default 1)
// Output:       None -- CoolDate_Time object updated

void CoolDate_Time::end_hour (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec - 1);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  incr_hour (n-1);
}


// start_day -- Increment CoolDate_Time object to start of "n" days
//              time set to 00:00:00
// Input:       Number of days to advance to start (default 1)
// Output:      None -- CoolDate_Time object updated

void CoolDate_Time::start_day (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  incr_day (n-1);
}


// end_day -- Increment CoolDate_Time object to end of "n" days
//            time set to 23:59:59
// Input:     Number of days to advance to end (default 1)
// Output:    None -- CoolDate_Time object updated

void CoolDate_Time::end_day (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec - 1);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  incr_day (n-1);
}


// start_week -- Increment CoolDate_Time object to start of "n" weeks
//               Date/time set to Monday 00:00:00
// Input:        Number of weeks to advance to start (default 1)
// Output:       None -- CoolDate_Time object updated

void CoolDate_Time::start_week (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  this->time_seconds += (WEEK - (this->dt.tm_wday * DAY));
  this->time_seconds += DAY;
  incr_week (n-1);
}


// end_week -- Increment CoolDate_Time object to end of "n" weeks
//             Date/Time set to Sunday 23:59:59
// Input:      Number of weeks to advance to end (default 1)
// Output:     None -- CoolDate_Time object updated

void CoolDate_Time::end_week (int n) {
  this->time_seconds += (MINUTE - this->dt.tm_sec - 1);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  this->time_seconds += (WEEK - (this->dt.tm_wday * DAY) - DAY);
  this->time_seconds += DAY;
  incr_week (n-1);
}


// start_month -- Increment CoolDate_Time object to start of "n" months
//                Date/time set to 01/MM/YYYY 00:00:00
// Input:         Number of months to advance to start (default 1)
// Output:        None -- CoolDate_Time object updated

void CoolDate_Time::start_month (int n) {
  int temp_day = this->dt.tm_mday;              // Date in month
  int temp_month = this->dt.tm_mon;             // Current month
  this->time_seconds += (MINUTE - this->dt.tm_sec);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  for ( ; temp_day < days_in_month[temp_month]; temp_day++)
    this->time_seconds += DAY;
  if (temp_month == FEBRUARY && IS_LEAP_YEAR (this->dt.tm_year))
    this->time_seconds += DAY;
  incr_month (n-1);
}


// end_month -- Increment CoolDate_Time object to end of "n" months
//              Date/time set to 31/MM/YYYY 23:59:59
// Input:       Number of months to advance to end (default 1)
// Output:      None -- CoolDate_Time obejct updated

void CoolDate_Time::end_month (int n) {
  int temp_day = this->dt.tm_mday;              // Date in month
  int temp_month = this->dt.tm_mon;             // Current month
  this->time_seconds += (MINUTE - this->dt.tm_sec - 1);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  for ( ; temp_day < days_in_month[temp_month]; temp_day++)
    this->time_seconds += DAY;
  if (temp_month == FEBRUARY && IS_LEAP_YEAR (this->dt.tm_year))
    this->time_seconds += DAY;
  incr_month (n-1);
}


// start_year -- Increment CoolDate_Time object to start of "n" years
//               Date/Time set to 01/01/YYYY 00:00:00
// Input:        Number of years to advance to start (default 1)
// Output:       None -- CoolDate_Time object updated

void CoolDate_Time::start_year (int n) {
  int temp_year = this->dt.tm_year;
  int temp_day = this->dt.tm_mday;              // Date in month
  int temp_month = this->dt.tm_mon;             // Current month
  this->time_seconds += (MINUTE - this->dt.tm_sec);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  this->time_seconds += HOUR;
  for ( ; temp_day < days_in_month[temp_month]; temp_day++)
    this->time_seconds += DAY;
  if (temp_month == FEBRUARY && IS_LEAP_YEAR (temp_year))
    this->time_seconds += DAY;
  for (++temp_month; temp_month <= DECEMBER; temp_month++) {
    this->time_seconds += (DAY * days_in_month[temp_month]);
    if (temp_month == FEBRUARY && IS_LEAP_YEAR (temp_year))
      time_seconds += DAY;
  }
  incr_year (n-1);
}
  

// end_year -- Increment CoolDate_Time object to end of "n" years
//             Date/time set to 31/12/YYYY 23:59:59
// Input:      Number of years to advance to end (default 1)
// Output:     None -- CoolDate_Time object updated

void CoolDate_Time::end_year (int n) {
  int temp_year = this->dt.tm_year;
  int temp_day = this->dt.tm_mday;              // Date in month
  int temp_month = this->dt.tm_mon;             // Current month
  this->time_seconds += (MINUTE - this->dt.tm_sec - 1);
  this->time_seconds += (HOUR - (this->dt.tm_min * MINUTE) - MINUTE);
  this->time_seconds += (DAY - (this->dt.tm_hour * HOUR) - HOUR);
  this->time_seconds += HOUR;
  for ( ; temp_day < days_in_month[temp_month]; temp_day++)
    this->time_seconds += DAY;
  if (temp_month == FEBRUARY && IS_LEAP_YEAR (temp_year))
    this->time_seconds += DAY;
  for (++temp_month; temp_month <= DECEMBER; temp_month++) {
    this->time_seconds += (DAY * days_in_month[temp_month]);
    if (temp_month == FEBRUARY && IS_LEAP_YEAR (temp_year))
      time_seconds += DAY;
  }
  incr_year (n-1);
}


// ascii_time -- Returns the time in ASCII format for the country
//               indicated in the CoolDate_Time object. Note that the
//               storage is static and destroyed on the next call.
// Input:        this*
// Output:       Formatted char* string

const char* CoolDate_Time::ascii_time () const {
  static char storage_s[35];
  switch (this->c_code) {

  case UNKNOWN_COUNTRY:
    sprintf (storage_s, time_format_s[UNKNOWN_COUNTRY]);
    break;

  case UNITED_STATES:
  case NORWAY:
  case GERMANY:
  case PORTUGAL:
  case FINLAND:
  case ARABIC_COUNTRIES:
  case ISRAEL:
  case LATIN_AMERICA:
  case NETHERLANDS:
  case BELGIUM:
  case FRANCE:
  case SPAIN:
  case ITALY:
  case SWITZERLAND:
  case DENMARK:
  case UNITED_KINGDOM:
  case SWEDEN:
  case FRENCH_CANADIAN:
    sprintf (storage_s, time_format_s[this->c_code], TIME_VALUES);
    break;
  }
  return storage_s;
}


// ascii_date -- Returns the date in ASCII format for the country
//               indicated in the CoolDate_Time object. Note that the
//               storage is static and destroyed on the next call.
// Input:        this*
// Output:       Formatted char* string

const char* CoolDate_Time::ascii_date () const {
  static char storage_s[11];
  switch (this->c_code) {

  case UNKNOWN_COUNTRY:
    sprintf (storage_s, date_format_s[UNKNOWN_COUNTRY]);
    break;

  case UNITED_STATES:
  case NORWAY:
  case GERMANY:
  case PORTUGAL:
  case FINLAND:
  case ARABIC_COUNTRIES:
  case ISRAEL:
  case LATIN_AMERICA:
  case NETHERLANDS:
  case BELGIUM:
  case FRANCE:
  case SPAIN:
  case ITALY:
  case SWITZERLAND:
  case DENMARK:
    sprintf (storage_s, date_format_s[this->c_code], MONTH_DATE_VALUES);
    break;

  case UNITED_KINGDOM:
    sprintf (storage_s, date_format_s[this->c_code], DATE_MONTH_VALUES);
    break;

  case SWEDEN:
  case FRENCH_CANADIAN:
    sprintf (storage_s, date_format_s[this->c_code], YEAR_MONTH_VALUES);
    break;
  }
  return storage_s;
}


// ascii_date_time -- Returns date and time in ASCII format for
//                    country indicated in CoolDate_Time object. Note
//                    that the storage is static and destroyed 
//                    on the next call
// Input:             this*
// Output:            Formatted char* string

const char* CoolDate_Time::ascii_date_time () const {
  static char storage_s[50];                    // String to hold output
  switch (this->c_code) {

  case UNKNOWN_COUNTRY:
    sprintf (storage_s, date_format_s[UNKNOWN_COUNTRY]);

  case UNITED_STATES:
  case NORWAY:
  case GERMANY:
  case PORTUGAL:
  case FINLAND:
  case ARABIC_COUNTRIES:
  case ISRAEL:
  case LATIN_AMERICA:
  case NETHERLANDS:
  case BELGIUM:
  case FRANCE:
  case SPAIN:
  case ITALY:
  case SWITZERLAND:
  case DENMARK:
    sprintf (storage_s, date_format_s[this->c_code], MONTH_DATE_VALUES);
    storage_s[10] = ' ';
    sprintf (&storage_s[11], time_format_s[this->c_code], TIME_VALUES);
    break;

  case UNITED_KINGDOM:
    sprintf (storage_s, date_format_s[this->c_code], DATE_MONTH_VALUES);
    storage_s[10] = ' ';
    sprintf (&storage_s[11], time_format_s[this->c_code], TIME_VALUES);
    break;

  case SWEDEN:
  case FRENCH_CANADIAN:
    sprintf (storage_s, date_format_s[this->c_code], YEAR_MONTH_VALUES);
    storage_s[10] = ' ';
    sprintf (&storage_s[11], time_format_s[this->c_code], TIME_VALUES);
    break;
  }
  return storage_s;
}


// ascii_duration -- Returns the duration of time in an interval between this
//                   object and some other date/time object
// Input:            Reference to date/time object
// Output:           Formatted char* string

const char* CoolDate_Time::ascii_duration (const CoolDate_Time& d) const {
  static char storage_s[100];                   // String to hold output
  long n;
  long num_years;
  long num_weeks;
  long num_days;
  long num_hours;
  long num_minutes;
  long num_seconds;
  n = ABS(((long)(this->time_seconds-d.time_seconds))); // Difference in seconds
  // num_years = ABS((this->get_year() - d.get_year())); // Number years
  num_years = ( n / YEAR ); // Number years
  num_weeks = (n % YEAR) / WEEK;                // Number of weeks
  num_days = ((n % YEAR) % WEEK) / DAY; // Number days
  num_hours = (((n % YEAR) % WEEK) % DAY) / HOUR; // Number hours
  num_minutes = ((((n % YEAR) % WEEK) % DAY) % HOUR) / MINUTE;
  num_seconds = (((((n % YEAR) % WEEK) % DAY) % HOUR) % MINUTE);
  sprintf (storage_s,
     "%ld years, %ld weeks, %ld days, %ld hours, %ld minutes, %ld seconds",
      num_years, num_weeks, num_days, num_hours, num_minutes, num_seconds);
  return storage_s;
}


// inrange -- Takes a char and returns a 1 if it is a digit between 0 and 9,
//            returns 2 if it is a letter of the alphabet and returns 0
//            otherwise.  This is used by parse to break things into tokens.
//
// Input   -- a char
// Output  -- an int as described above.

int inrange (char c) {
  if (isdigit (c))                              // If character is a digit
    return (1);                                 // Return indication
  if (isalpha (c))                              // If character is a letter
    return(2);                                  // Return indication
  else return(0);                               // Else return failure
}


// getzone -- This looks for a valid time zone in the char*, if one is
//            found the proper slots of the CoolDate_Time object are set and
//            that part of the char* is deleted.
//
// Input   -- Takes a reference to a CoolDate_Time object, and a char*
// Output  -- Does not return anything, just mutates the CoolDate_Time object.
        
void getzone (CoolDate_Time& d, char* s) {
  c_upcase(s);
  int ind = 0;
  long start,end;
  if (strfind(s,"*CANADA?EASTERN*",&start,&end)) {
    d.tz_code = CANADA_EASTERN;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*CANADA?CENTRAL*",&start,&end)) {
    d.tz_code = CANADA_CENTRAL;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*CANADA?MOUNTAIN*",&start,&end)) {
    d.tz_code = CANADA_MOUNTAIN;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*CANADA?PACIFIC*",&start,&end)) {
    d.tz_code = CANADA_PACIFIC;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*CANADA?YUKON*",&start,&end)) {
    d.tz_code = CANADA_YUKON;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*EASTERN*",&start,&end) || strfind(s,"*EDT*",&start,&end)) {
    d.tz_code = US_EASTERN;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*CENTRAL*",&start,&end) || strfind(s,"*CDT*",&start,&end)) {
    d.tz_code = US_CENTRAL;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*MOUNTAIN*",&start,&end) || strfind(s,"*MDT*",&start,&end)) {
    d.tz_code = US_MOUNTAIN;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*PACIFIC*",&start,&end)) {
    d.tz_code = US_PACIFIC;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*PACIFIC?NEW*",&start,&end)) {
    d.tz_code = US_PACIFIC_NEW;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*YUKON*",&start,&end)) {
    d.tz_code = US_YUKON;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*EAST?INDIANA*",&start,&end)) {
    d.tz_code = US_EAST_INDIANA;
    s = strnremove(s+start,end-start);
  }   
  else if (strfind(s,"*ARIZONA*",&start,&end)) {
    d.tz_code = US_ARIZONA;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*HAWAII*",&start,&end)) {
    d.tz_code = US_HAWAII;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*NEWFOUNDLAND*",&start,&end)) {
    d.tz_code = CANADA_NEWFOUNDLAND;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*ATLANTIC*",&start,&end)) {
    d.tz_code = CANADA_ATLANTIC;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*SASKATCHEWAN*",&start,&end)) {
    d.tz_code = CANADA_EAST_SASKATCHEWAN;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*EIRE*",&start,&end) || strfind(s,"*GMT*",&start,&end)) {
    d.tz_code = GB_EIRE;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*WET*",&start,&end)) {
    d.tz_code = WET;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*ICELAND*",&start,&end)) {
    d.tz_code = ICELAND;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*MET*",&start,&end)) {
    d.tz_code = MET;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*POLAND*",&start,&end)) {
   d.tz_code = POLAND;
   s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*EET*",&start,&end)) {
    d.tz_code = EET;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*TURKEY*",&start,&end)) {
    d.tz_code = TURKEY;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*W_SU*",&start,&end)) {
    d.tz_code = W_SU;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*PRC*",&start,&end)) {
    d.tz_code = PRC;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*KOREA*",&start,&end)) {
    d.tz_code = KOREA;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*JAPAN*",&start,&end)) {
   d.tz_code = JAPAN;
    s = strnremove(s+start,end-start);   
  }
  else if (strfind(s,"*SINGAPORE*",&start,&end)) {
    d.tz_code = SINGAPORE;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*HONGKONG*",&start,&end)) {
    d.tz_code = HONGKONG;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*ROC*",&start,&end)) {
    d.tz_code = ROC;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*TASMANIA*",&start,&end)) {
    d.tz_code = AUSTRALIA_TASMANIA;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*QUEENSLAND*",&start,&end)) {
    d.tz_code = AUSTRALIA_QUEENSLAND;
    s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*NORTH*",&start,&end)) {
   d.tz_code = AUSTRALIA_NORTH;
   s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*WEST*",&start,&end)) {
   d.tz_code = AUSTRALIA_WEST;
   s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*SOUTH*",&start,&end)) {
   d.tz_code = AUSTRALIA_SOUTH;
   s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*VICTORIA*",&start,&end)) {
   d.tz_code = AUSTRALIA_VICTORIA;
   s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*NSW*",&start,&end)) {
   d.tz_code = AUSTRALIA_NSW;
   s = strnremove(s+start,end-start);
  }
  else if (strfind(s,"*NZ*",&start,&end)) {
   d.tz_code = NZ;
   s = strnremove(s+start,end-start);
  }
}


#define TOKEN_LIMIT 16


// parse  -- This is the CoolDate_Time parser, it is a member function.  It parses
//           the time and date out of the given char*, source, and sets all 
//           the appropriate slots of *this to the proper values.  If the 
//           optional argument settz is true, then the getzone function above
//           is called and searches for a timezone.
// Input  -- A char* source, and an optional int settz whose defalut is zero.
// Output -- Nothing is returned, the object is modified.

void CoolDate_Time::parse (char* source, int settz) {
  const char* m;
  int token_num[TOKEN_LIMIT];
  int token_alph[TOKEN_LIMIT];
  int token_used[TOKEN_LIMIT];
  CoolString token_list[TOKEN_LIMIT];
  int tcount = 0;
  token_alph[0] = 0;
  token_num[0] = 0;
  int flag = 0;
  int ind;
  long year = -1;
  int month = -1;
  int day = -1;
  int hour = -1;
  int minute = -1;
  int second = -1;

  if (settz) getzone(*this,source);
  int len = (int)strlen(source);

  //////////////////////////////////////////////////////////////////////
  //
  // This part of the parser breaks source down into tokens.
  // 
  //////////////////////////////////////////////////////////////////////

  for (ind = 0; ind < TOKEN_LIMIT; ind++)
    token_num[ind] = 0;
  for (ind = 0; ind < TOKEN_LIMIT; ind++)
    token_alph[ind] = 0;
  for (ind = 0; ind < TOKEN_LIMIT; ind++)
    token_used[ind] = 0;
  
  for (ind=0; ind < len; ind++)
    switch (inrange(source[ind])) {
    case 1:
      flag = 1;
      token_list[tcount] += source[ind];
      token_num[tcount] = 1;
      break;
    case 2:
      flag = 1;
      token_list[tcount] += source[ind];
      token_alph[tcount] = 1;
      break;
    case 0:
      if (flag) {
        if (++tcount > TOKEN_LIMIT) {
          //Raise Error, SYM(CoolDate_Time), SYM(Too_Many_Tokens),
          printf ("CoolDate_Time::parse(): More than %d tokens in input.\n",
                  TOKEN_LIMIT);
          abort ();
        }
        token_alph[tcount] = 0;
        token_num[tcount] = 0;
      }
      flag = 0;
      break;
    }
  if (flag == 0) tcount--; 
  if (tcount <= 0) {
    //Raise Error, SYM(CoolDate_Time), SYM(Too_Few_Tokens),
    printf ("CoolDate_Time::parse(): Not enough tokens in input.\n");
    abort ();
  }

  //////////////////////////////////////////////////////////////////////
  //
  // This part of the parser checks for spelled out months, and converts
  // them to integer representations.
  // 
  //////////////////////////////////////////////////////////////////////
  
  for (ind=0; ind<=tcount; ind++)
    if (token_num[ind] == 0 && token_alph[ind] == 1) {
      m = upcase(token_list[ind]);
      if (0 == strcmp(m,"JAN") || 0 == strcmp(m,"JANUARY")) {
        token_list[ind] = "01";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"FEB") || 0 == strcmp(m,"FEBRUARY")) {
        token_list[ind] = "02";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"MAR") || 0 == strcmp(m,"MARCH")) {
        token_list[ind] = "03";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"APR") || 0 == strcmp(m,"APRIL")) {
        token_list[ind] = "04";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"MAY")) {
        token_list[ind] = "05";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"JUN") || 0 == strcmp(m,"JUNE")) {
        token_list[ind] = "06";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"JUL") || 0 == strcmp(m,"JULY")) {
        token_list[ind] = "07";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"AUG") || 0 == strcmp(m,"AUGUST")) {
        token_list[ind] = "08";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"SEP") || 0 == strcmp(m,"SEPTEMBER")) {
        token_list[ind] = "09";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"OCT") || 0 == strcmp(m,"OCTOBER")) {
        token_list[ind] = "10";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"NOV") || 0 == strcmp(m,"NOVEMBER")) {
        token_list[ind] = "11";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
      else if (0 == strcmp(m,"DEC") || 0 == strcmp(m,"DECEMBER")) {
        token_list[ind] = "12";
        token_num[ind] = 1;
        token_alph[ind] = 0;
      }
    }

  ////////////////////////////////////////////////////////////////////////
  //
  // LOOKS FOR A YEAR, AND TAKES THE APPROPRIATE THINGS AROUND IT TO 
  // FILL IN THE DATE SLOTS (year, month, day).
  //
  ////////////////////////////////////////////////////////////////////////
  
  int lngth;
  flag = -1;
  for(ind=0; ind <= tcount; ind++)
    if (token_num[ind] == 1 &&
        token_alph[ind] == 0 &&
        strlen(token_list[ind]) > 2 ||
        atoi(token_list[ind]) > 59) {
      flag = ind;
      break;
    }
  if (flag != -1) {
    year = atoi(token_list[flag]);
    token_used[flag]=1; 
    if (this->c_code==SWEDEN || this->c_code==FRENCH_CANADIAN) {
      for (++ind; token_num[ind] == 1 &&
           token_alph[ind] != 1 &&
           strlen(token_list[ind]) <= 2; ind++);
      ind = ind - flag -1;
      if (ind > 2) ind = 2;
      switch(ind){
      case 0:
        month = 1;
        day = 1;
      case 1:
        len = atoi(token_list[flag+1]);
        if (len > 31) {
          //Raise Error, SYM(CoolDate_Time), SYM(Unknown_Token),
          printf ("CoolDate_Time::parse(): Unrecognized token in input.\n");
          abort ();
        }
        token_used[flag+1] = 1;
        if (len > 12) {
          day = len;
          month = 1;
        }
        else {
          month = len;
          day = 1;
        }
      case 2:
        len = atoi(token_list[flag+1]);
        lngth = atoi(token_list[flag+2]);
        if (len > 31 || lngth > 31) {
          //Raise Error, SYM(CoolDate_Time), SYM(Unknown_Token),
          printf ("CoolDate_Time::parse(): Unrecognized token in input.\n");
          abort ();
        }
        token_used[flag+1] = 1;
        if (len<13 && lngth>12) {
          day = lngth;
          month = len;
          token_used[flag+2] = 1;
        }
        else {
          day = len;
          if (lngth>12) month = 1;
          else {
            month=lngth;
            token_used[flag+2] = 1;
          }
        }
      }
    }
    else {
      for (--ind; 0 <= ind && token_num[ind] == 1 &&
           token_alph[ind] != 1 &&
           strlen(token_list[ind]) <= 2; ind--);
      ind = flag - ind - 1;
      if (ind>2) ind=2;
      switch(ind){
      case 0:
        month = 1;
        day = 1;
        break;
      case 1:
        len = atoi(token_list[flag-1]);
        if (len > 31) {
          //Raise Error, SYM(CoolDate_Time), SYM(Unknown_Token),
          printf ("CoolDate_Time::parse(): Unrecognized token in input.\n");
          abort ();
        }
        token_used[flag-1] = 1;
        if (len>12) {
          day = len;
          month = 1;
        }
        else {
          month = len;
          day = 1;
        }
        break;
      case 2:
        len = atoi(token_list[flag-1]);
        lngth = atoi(token_list[flag-2]);
        if (len > 31 || lngth > 31) {
          //Raise Error, SYM(CoolDate_Time), SYM(Unknown_Token),
          printf ("CoolDate_Time::parse(): Unrecognized token in input.\n");
          abort ();
        }
        if (this->c_code != UNITED_KINGDOM) {
          token_used[flag-1] = 1;
          if (len < 13 && lngth > 12) {
            day = lngth;
            month = len;
            token_used[flag-2] = 1;
          }
          else {
            day = len;
            if (lngth>12) month = 1;
            else {
              month=lngth;
              token_used[flag-2] = 1;
            }
          }
        }
        else {
          token_used[flag-2] = 1;
          if (lngth < 13 && len > 12) {
            day = len;
            month = lngth;
            token_used[flag-1] = 1;
          }
          else {
            day = lngth;
            if (len>12) month = 1;
            else {
              month=len;
            token_used[flag-1] = 1;
            }
          }
        }
        break;
      }
    }
  }
 
  ////////////////////////////////////////////////////////////////////////
  //
  // LOOKS FOR AN "AM" OR "PM" AND TAKES EVERY APROPRIATE TOKEN BEFORE 
  // THE "AM" OR "PM" TO FILL THE TIME SLOTS (hour, minute, second).
  //
  ////////////////////////////////////////////////////////////////////////

  int btrack;
  long start, end;
  for (ind = 0; ind <= tcount; ind++) 
    if (token_alph[ind] == 1) {
      flag = 0;
      m = upcase(token_list[ind]);
      if (strrfind(m, "*PM*", &start, &end) ||
          strrfind(m, "*AM*", &start, &end) ) { 
        
        if (token_list[ind][start] == 'P') flag=1;    
        btrack = token_num[ind];
        len = (int)strlen(token_list[ind]);
        
        if (btrack && (token_list[ind][len-1] != 'M' || len > 4 || len < 3)) {
          //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Time),
          printf ("CoolDate_Time::parse(): Invalid time specified in input.\n");
          abort ();
        }
        if (btrack==0 && len != 2) {
          //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Time),
          printf ("CoolDate_Time::parse(): Invalid time specified in input.\n");
          abort ();
        }
        int tmp;
        for (tmp = ind-1; 0 <= tmp && token_num[tmp] != 0 
             && strlen(token_list[tmp]) <= 2
             && token_used[tmp] != 1; tmp--);
        tmp = ind - tmp - 1;
        if (btrack) tmp++;
        if (tmp>3) tmp = 3;
        // This case if part of the time is in the same token as the
        // "am" or "pm".

        if (btrack)    // PM/AM and number in one token
          switch(tmp) {
          case 1:
            hour = atoi(token_list[ind]);
            if (hour > 23) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Hour),
              printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
              abort ();
            }
            if (flag && hour<12) hour += 12;
            token_used[ind] = 1;
            minute = 0;
            second = 0;
            break;
          case 2:
            hour = atoi(token_list[ind-1]);
            if (hour > 23) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Hour),
              printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
              abort ();
            }
            if (flag && hour<12) hour += 12;
            minute = atoi(token_list[ind]);
            if (minute > 59) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Minutes),
              printf ("CoolDate_Time::parse(): Specified minutes out of range.\n");
              abort ();
            }
            token_used[ind] = 1;
            token_used[ind-1] = 1;
            second = 0;
            break;
          case 3:
            hour = atoi(token_list[ind-2]);
            if (hour > 23) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Hour),
              printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
              abort ();
            }
            if (flag && hour<12) hour += 12;
            minute = atoi(token_list[ind-1]);
            if (minute > 59) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Minutes),
              printf ("CoolDate_Time::parse(): Specified minutes out of range.\n");
              abort ();
            }
            second = atoi(token_list[ind]);
            if (second > 59) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Seconds),
              printf ("CoolDate_Time::parse(): Specified seconds out of range.\n");
              abort ();
            }
            token_used[ind] = 1;
            token_used[ind-1] = 1;
            token_used[ind-2] = 1;
            break;
          }

        else
        
          switch(tmp) {
          case 0: 
            minute = 0;
            second = 0;
            if (flag) hour=12; 
            else hour=0; 
            break;
          case 1:
            hour = atoi(token_list[ind-1]);
            token_used[ind-1] = 1;
            if (hour > 23) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Hour),
              printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
              abort ();
            }
            if (flag && hour < 12) hour += 12;
            minute = 0;
            second = 0;
            break;
          case 2:
            hour = atoi(token_list[ind-2]);
            if (hour > 23) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Hour),
              printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
              abort ();
            }
            if (flag && hour < 12) hour += 12;
            minute = atoi(token_list[ind-1]);
            if (minute > 59) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Minutes),
              printf ("CoolDate_Time::parse(): Specified minutes out of range.\n");
              abort ();
            }
            token_used[ind-1] = 1;
            token_used[ind-2] = 1;
            second=0;
            break;
          case 3:
            hour = atoi(token_list[ind-3]);
            if (hour > 23) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Hour),
              printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
              abort ();
            }
            if (flag && hour < 12) hour += 12;
            minute = atoi(token_list[ind-2]);
            if (minute > 59) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Minutes),
              printf ("CoolDate_Time::parse(): Specified minutes out of range.\n");
              abort ();
            }
            second = atoi(token_list[ind-1]);
            if (second > 59) {
              //Raise Error, SYM(CoolDate_Time), SYM(Invalid_Seconds),
              printf ("CoolDate_Time::parse(): Specified seconds out of range.\n");
              abort ();
            }
            token_used[ind-1] = 1;
            token_used[ind-2] = 1;
            token_used[ind-3] = 1;
            break;
          }
        break;
      }
    }

  ////////////////////////////////////////////////////////////////////////
  //
  // This part of the parser fills in any remaining slots not filled 
  // above, and sets the rest to defaults.
  //
  ////////////////////////////////////////////////////////////////////////

  CoolDate_Time current;
  current.set_local_time();
  int first = -1;
  int secnd = -1;

  if (year == -1) {
    int sind, find;
    for (ind = 0; ind <= tcount; ind++) 
           if (token_num[ind] != 0 
               && token_alph[ind] == 0
               && strlen(token_list[ind]) <= 2
               && token_used[ind] != 1) {
                 if (first != -1) {                    
                   secnd = atoi(token_list[ind]);
                   sind = ind;
                   token_used[ind] = 1;
                   break;
                 }
                 first = atoi(token_list[ind]);
                 find = ind;
                 token_used[ind] = 1;
               }
  
    if (first != -1 && first <= 31){
      if (this->c_code != UNITED_KINGDOM) {
        if (secnd == -1 || secnd > 31) day = first;
        else {
          if (first > 12 && secnd > 12) {
            day = first;
            token_used[sind] = 0;
          }
          else {
            if (first <= 12){
              month = first;
              day = secnd;
            }
            else 
              if (secnd <= 12) { 
                month = secnd;
                day = first;
              }
          }
        }
      }
      else {
        if (secnd==-1 || secnd>31) day = first;
        else {
          if (first>12 && secnd>12) {
            day = secnd;
            token_used[find] = 0;
          }
          else {
            if (secnd <= 12){
              month = secnd;
              day = first;
            }
            else 
              if (first <= 12) { 
                month = first;
                day = secnd;
              }
          }
        }
      }
    }
    if (year == -1) year = current.get_year();
    if (month == -1) month = current.get_mon() + 1;
    if (day == -1) day = current.get_mday();
  }

  // THIS PART DEALS WITH PARTS OF THE TIME NOT ALREADY FILLED IN

  if (hour == -1) {
    first = -1;
    secnd = -1;
    int third = -1;
    for (ind=0; ind<=tcount; ind++) 
      if (token_num[ind] != 0 
          && token_alph[ind] == 0
          && strlen(token_list[ind]) <= 2
          && token_used[ind] != 1) {
          
            if (secnd != -1) {
              third = atoi(token_list[ind]);
              break;
            }
            if (first != -1)
              secnd = atoi(token_list[ind]);
            else 
              first = atoi(token_list[ind]);
          }
    if (third != -1) {
      second = third;
      if (second > 59) {
        printf ("CoolDate_Time::parse(): Specified seconds out of range.\n");
        abort ();
      }
    }
    if (secnd != -1) {
      minute = secnd;
      if (minute > 59) {
        printf ("CoolDate_Time::parse(): Specified minutes out of range.\n");
        abort ();
      }
    }
    if (first != -1) {
      hour = first;
      if (hour > 23) {
        printf ("CoolDate_Time::parse(): Specified hour out of range.\n");
        abort ();
      }
    }
  }
  if (hour == -1) hour = 0;
  if (minute == -1) minute = 0;
  if (second == -1) second = 0;

  //////////////////////////////////////////////////////////////////////
  //
  // DO SOME FINAL BOUNDS CHECKING AND SET THE SLOTS OF OBJ 
  // TO THE CORRECT VALUES
  //
  //////////////////////////////////////////////////////////////////////

  this->adjust_year(year);
  month--;
  if (year > 99) {
    this->century = int((year / 100) * 100);
    year -= this->century;
  }
  else
    this->century = 1900;
  if ((month == 0 || month == 2 || month == 4 || month == 6 || month == 7 ||
       month == 9 || month == 11) && day > 31) {
    printf ("CoolDate_Time::parse(): Too many days for month of %s.\n",
            month_names[month]);
    abort ();
  }
  else if ((month == 3 || month == 5 || month == 8 || month == 10)
           && day > 30) {
    printf ("CoolDate_Time::parse(): Too many days for month of %s.\n",
            month_names[month]);
    abort ();
  }
  else if (month == 1 && day > 28) 
    if (IS_LEAP_YEAR(year) && day < 30) ;
    else {
      printf ("CoolDate_Time::parse(): Too many days for month of %s.\n", 
              month_names[month]);
      abort ();
    }

   tm t;
   memset( &t, 0, sizeof(t) );
   t.tm_sec = second;                   // Copy seconds value
   t.tm_min = minute;                   // Copy minutes value
   t.tm_hour = hour;                    // Copy hours value
   t.tm_mday = day;                     // Copy day of month value
   t.tm_mon = month;                    // Copy month value
   t.tm_year = (int)(this->century + year - 1900);  // Copy year value
   t.tm_isdst = 0;

   this->time_seconds = mktime (&t);

   if (this->time_seconds == -1)
     // Note: DOS returns -1 for any year prior to 1980
     this->time_seconds = time((time_t *)0);
   
#if defined(DOS)
  // OS2 mktime increments the hour by one if daylight savings time is in effect
  // we have to roll everything back one to correct for this.
   adjust_dst(&t);
#endif

   this->dt.tm_sec = t.tm_sec;                  // Copy seconds value
   this->dt.tm_min = t.tm_min;                  // Copy minutes value
   this->dt.tm_hour = t.tm_hour;                // Copy hours value
   this->dt.tm_mday = t.tm_mday;                // Copy day of month value
   this->dt.tm_mon = t.tm_mon;                  // Copy month value
   this->dt.tm_year = t.tm_year;                // Copy year value
   this->dt.tm_wday = t.tm_wday;                // Copy day of week value
   this->dt.tm_yday = t.tm_yday;                // Copy day of year value
   this->dt.tm_isdst = t.tm_isdst;              // Copy day light savings flag
}


// operator<< -- Output CoolDate_Time object to output stream
// Input:        Output stream reference, CoolDate_Time reference
// Output:       Output stream reference

ostream& operator<< (ostream& os, const CoolDate_Time& d) {
  return os << country_names[d.c_code] << " " << d.ascii_date_time ();
}

