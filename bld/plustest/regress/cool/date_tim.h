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
// Updated: LGO 07/03/89 -- Inherit from Generic
// Updated: MNF 07/25/89 -- Add the parse member function 
// Updated: MBN 09/06/89 -- Added conditional exception handling
// Updated: MBN 01/17/90 -- Fixed parsing algorithms to be GMT relative
// Updated: MBN 02/06/90 -- Support years prior to the epoch (1/1/1970)
// Updated: MBN 02/12/90 -- Changed all ascii() functions to return const char*
// Updated: MBN 02/13/90 -- Made ascii_duration() a member function for epoch
// Updated: DLS 03/22/91 -- New lite version
// Updated: JAM 08/11/92 -- removed DOS specifics, stdized #includes
// Updated: JAM 08/12/92 -- removed 'inline' from friend declarations
//
// The Date_Time class provides  an   application programmer  with  simplified
// access to the operating system date and time values, along with support for
// comparison to other dates and times, calendar adjustment, and international
// support  for   display formats.   To  implement  this functionality,  three
// additional header files  are maintained: <country.h> providing symbolic and
// character    string  definitions  for      the various supported  contries;
// <timezone.h>  that enumerates symbolic  and character string values for all
// major  time zones; and <calendar.h>  that  lists  the days of the week  and
// months of the year.  The following countries and their associated date/time
// format are supported:
//
//                United States          03-29-1989 17:35:00
//                French Canadian        1989-29-03 17:35:00
//                Latin America          03/29/1989 17:35:00
//                Netherlands            03-29-1989 17:35:00
//                Belgium                03/29/1989 17:35:00
//                France                 03/29/1989 17:35:00
//                Spain                  03/29/1989 17:35:00
//                Italy                  03/29/1989 17:35:00
//                Switzerland            03.29.1989 17.35.00
//                United Kingdom         29-03-1989 17:35:00
//                Denmark                03/29/1989 17:35:00
//                Sweden                 1989-29-03 17.35.00
//                Norway                 03/29/1989 17:35:00
//                Germany                03.29.1989 17.35.00
//                Portugal               03/29/1989 17:35:00
//                Finland                03.29.1989 17.35.00
//                Arabic Countries       03/29/1989 17:35:00
//                Israel                 03 29 1989 17:35:00
//
// The private data section contains a  long integer that  maintains the system
// date and  time as an interval  from the base  date of 01/01/1970 00:00:00am.
// This  is supported  and  structure  in an ANSI  C <time.> tm structure that
// keeps separate fields for seconds,  minutes, hours,  day   of month, day  of
// week, month, year, and day of year.  Two other slots  provided time zone and
// country code values for   each  Date_Time object.   In addition,  two static
// slots containing  the default time zone and  country code for the  Date_Time
// class as a whole are defined.  The following time zones are supported:
//  
//        US/Eastern:                  Eastern time zone, USA                 
//        US/Central:                  Central time zone, USA                 
//        US/Mountain:                 Mountain time zone, USA                
//        US/Pacific:                  Pacific time zone, USA                 
//        US/Pacific-New:              Pacific time zone, USA with DST changes
//        US/Yukon:                    Yukon time zone, USA                   
//        US/East-Indiana:             Estern time zone, USA with no DST      
//        US/Arizona:                  Mountain time zone, USA with no DST    
//        US/Hawaii:                   Hawaii                                 
//        Canada/Newfoundland:         Newfoundland                           
//        Canada/Atlantic:             Atlantic time zone, Canada             
//        Canada/Eastern:              Eastern time zone, Canada              
//        Canada/Central:              Central time zone, Canada              
//        Canada/East-Saskatchewan:    Central time zone, Canada with no DST  
//        Canada/Mountain:             Mountain time zone, Canada             
//        Canada/Pacific:              Pacific time zone, Canada              
//        Canada/Yukon:                Yukon time zone, Canada                
//        GB-Eire:                     Great Britain and Eire (GMT)           
//        WET:                         Western Europe time                    
//        Iceland:                     Iceland                                
//        MET:                         Middle Europe time                     
//        Poland:                      Poland                                 
//        EET:                         Eastern Europe time                    
//        Turkey:                      Turkey                                 
//        W-SU:                        Western Soviet Union                   
//        PRC:                         People's Republic of China             
//        Korea:                       Republic of Korea                      
//        Japan:                       Japan                                  
//        Singapore:                   Singapore                              
//        Hongkong:                    Hongkong                               
//        ROC:                         Republic of China                      
//        Australia/Tasmania:          Tasmainia, Australia                   
//        Australia/Queensland:        Queensland, Australia                  
//        Australia/North:             Northern Territory, Australia          
//        Australia/West:              Western Australia                      
//        Australia/South:             South Australia                        
//        Australia/Victoria:          Victoria, Australia                    
//        Australia/NSW:               New South Wales, Australia             
//        NZ:                          New Zealand                            
//
// There are five constructors for the Date_Time  class.  The first constructor
// takes no arguments and intializes an empty object, setting the time zone and
// the  country  code   to  the  default values   for  the class.   The  second
// constructors takes a time  zone and country code and  initializes an  object
// for the specified local time zone and  country.  The third constructor takes
// a reference to some  other Date_Time object and  copies all the slot values.
// To get the time from a char*, first  create an  object with its country code
// set, and then use parse to fill in the slots.
//
// Public methods are provided for incrementing and  decrementing  the date and
// time, various   logical  tests   for  equality and  inequality,    and ASCII
// representation  of the   date  and/or  time   formatted   according  to  the
// appropriate country standard.  Two methods are  provided  to set a Date_Time
// object to either  the local time  (adjusted for  day  light savings time, if
// necessary) and  Greenwich Mean Time.   In  addition, several  public methods
// have been implemented for  interval arithmetic allowing messages  to be sent
// to the object along the lines of "move forward three weeks", "back up to end
// of last  month", etc.  Two  methods are  provided to  change the   time zone
// and/or country for a particular Date_Time object.  Finally accessors for all
// fields in the  <time.h> structure and  the  ASCII representation of the time
// zone and country are provided.
//
// Two other methods are also  implemented to  be used by  other methods of the
// class.  The first is a private member function that maintains  the integrity
// of   a  Date_Time object after  interval  arithmetic,  updating  to the next
// hour/day/month, etc.   The second is a  private friend function that takes a
// time zone value and  updates the local time  conversion information used for
// converting between GMT and the specified time zone.

#ifndef DATETIMEH                               // If we have not defined class
#define DATETIMEH                               // Indicate class Date_Time

#include <time.h>
#include <iostream.h>

#ifndef MISCELANEOUSH                           // If no misc.h file
#include <cool/misc.h>                  // Include useful defintions
#endif  

#ifndef CHARH
#include <cool/char.h>
#endif

#ifndef STRINGH
#include <cool/String.h>
#endif

#ifndef COUNTRYH
#include <cool/country.h> 
#endif

#ifndef TIMEZONEH
#include <cool/timezone.h>
#endif

#ifndef CALENDARH
#include <cool/calendar.h>
#endif

class CoolDate_Time {
public:
  CoolDate_Time ();                             // Simple constructor
  CoolDate_Time (time_zone, country);           // Constructor with TZ/Country
  CoolDate_Time (const CoolDate_Time&);         // Constructor with reference
  ~CoolDate_Time ();                            // Destructor
  
  void set_gm_time ();                          // Sets Greenwich Mean Time
  void set_local_time ();                       // Sets local time 
  inline void set_time_zone (time_zone);        // Set time zone for object
  inline void set_country (country);            // Set country for object
  friend void set_default_time_zone (time_zone);// Set default time zone
  friend void set_default_country (country);    // Set default country

  CoolDate_Time& operator= (const CoolDate_Time&); // CoolDate_Time x = y;
  inline long operator- (const CoolDate_Time&) const; // Interval subtraction
  CoolDate_Time& operator+= (long);                   // Interval addition/assignment
  CoolDate_Time& operator-= (long);                   // Interval subtraction/assign

  friend istream& operator>> (istream&, CoolDate_Time&);
  friend ostream& operator<< (ostream&, const CoolDate_Time&);
  friend ostream& operator<< (ostream&, const CoolDate_Time*);
  
  void incr_sec (int n = 1);                    // Advance seconds (default 1)
  void incr_min (int n = 1);                    // Advance minutes (default 1)
  void incr_hour (int n = 1);                   // Advance hours (default 1)
  void incr_day (int n = 1);                    // Advance day (default 1)
  void incr_week (int n = 1);                   // Advance week (default 1)
  void incr_month (int n = 1);                  // Advance month (default 1)
  void incr_year (int n = 1);                   // Advance year (default 1)
  
  inline void decr_sec (int n = 1);             // Decrement second (default 1)
  inline void decr_min (int n = 1);             // Decrement minute (default 1)
  inline void decr_hour (int n = 1);            // Decrement hours (default 1)
  inline void decr_day (int n = 1);             // Decrement day (default 1)
  inline void decr_week (int n = 1);            // Decrement week (default 1)
  void decr_month (int n = 1);                  // Decrement month (default 1)
  void decr_year (int n = 1);                   // Decrement year (default 1)
  
  void start_min (int n = 1);   // Change to start of +/- "n" minute (default 1
  void end_min (int n = 1);     // Change to end of +/- "n" minutes (default 1)
  void start_hour (int n = 1);  // Change to start of +/- "n" hours (default 1)
  void end_hour (int n = 1);    // Change to end of +/- "n" hours (default 1)
  void start_day (int n = 1);   // Change to start of +/- "n" days (default 1)
  void end_day (int n = 1);     // Change to end of +/- "n" days (default 1)
  void start_week (int n = 1);  // Change to start of +/- "n" weeks (default 1)
  void end_week (int n = 1);    // Change to end of +/- "n" weeks (default 1)
  void start_month (int n = 1); // Change to start of +/- "n" month (default 1)
  void end_month (int n = 1);   // Change to end of +/- "n" months (default 1)
  void start_year (int n = 1);  // Change to start of +/- "n" years (default 1)
  void end_year (int n = 1);    // Change to end of +/- "n" years (default 1)

  inline Boolean operator== (const CoolDate_Time&) const; // is same date/time
  inline Boolean operator!= (const CoolDate_Time&) const; // is not same date/time
  inline Boolean operator< (const CoolDate_Time&) const;  // is earlier date/time
  inline Boolean operator> (const CoolDate_Time&) const;  // is later date/time
  inline Boolean operator<= (const CoolDate_Time&) const; // is earlier or same
  inline Boolean operator>= (const CoolDate_Time&) const; // is later or same

  const char* ascii_time () const;      // Return time in ASCII country format
  const char* ascii_date () const;      // Return date in ASCII country format
  const char* ascii_date_time () const; // Return date/time ASCII country form
  const char* ascii_duration (const CoolDate_Time&) const; // Return time duration 
  void parse (char*, int settz = 0);    // Parses char* into the CoolDate_Time

  inline int get_sec () const;          // Return seconds from tm struct
  inline int get_min () const;          // Return minutes from tm struct
  inline int get_hour () const;         // Return hours from tm struct
  inline int get_mday () const;         // Return day of month from tm struct
  inline int get_mon () const;          // Return month from tm struct
  inline int get_year () const;         // Return year from tm struct
  inline int get_wday () const;         // Return day of week from tm struct
  inline int get_yday () const;         // Return day of year from tm struct
  inline Boolean is_day_light_savings () const; // Return daylight savings flag
  inline const char* get_time_zone () const ; // Return ASCII form of time zone
  inline const char* get_country () const; // Return ASCII form of country 
  inline const char* get_day_name () const; // Return ASCII form of week day
  inline const char* get_month_name () const; // Return ASCII form of month
  
private:
  time_t time_seconds;                          // Seconds 01/01/1970 00:00:00
  /*struct*/ tm dt;                             // OS date/time structure
  country c_code;                               // Retains date/time country
  time_zone tz_code;                            // Retains time zone code
  long time_adjust;                             // Time zone offset in seconds
  int year_adjust;                              // Year offset prior to epoch
  int century;                                  // Maintains century of year

  static time_zone default_tz_code_s;           // Default time zone
  static country default_c_code_s;              // Default country
  void resolve ();                              // Resolve/update object
  void adjust_tz ();                            // Adjust for local time zone
  void adjust_year (long&);                     // Adjust for years < 1/1/1970
  friend void set_tz (time_zone);               // Setup for local time zone
  
  friend int inrange (char);                    // Helper function for parser
  friend void getzone (CoolDate_Time&, char*);  // Gets time zone for parser
};


// set_time_zone -- Set the local time zone of a CoolDate_Time object
// Input:           Time zone
// Output:          None

inline void CoolDate_Time::set_time_zone (time_zone tz) {
  this->tz_code = tz;
  this->resolve ();
}


// set_country -- Set the country of a CoolDate_Time object
// Input:         Country code
// Output:        None

inline void CoolDate_Time::set_country (country c) {
  this->c_code = c;
}


// set_default_time_zone -- Set the default time zone of the CoolDate_Time class
// Input:           Time zone
// Output:          None

inline void set_default_time_zone (time_zone tz) {
  CoolDate_Time::default_tz_code_s = tz;
}


// set_default_country -- Set the default country of the CoolDate_Time class
// Input:         Country code
// Output:        None

inline void set_default_country (country c) {
  CoolDate_Time::default_c_code_s = c;
}


// operator- -- Subtract one CoolDate_Time from a another to calculate
//              the time interval between
// Input:       this*, CoolDate_Time reference
// Output:      Number of seconds presenting time interval 

inline long CoolDate_Time::operator- (const CoolDate_Time& d) const {
  return (this->time_seconds - d.time_seconds);
}


// decr_sec -- Retreat Date/Time by "n" second(s)
// Input:      Number of seconds to retreat (default 1)
// Output:     None -- CoolDate_Time object updated 

inline void CoolDate_Time::decr_sec (int n) {
  incr_sec (-n);
}


// decr_min -- Retreat Date/Time by "n" minute(s)
// Input:      Number of minutes to retreat (default 1)
// Output:     None -- CoolDate_Time object updated 

inline void CoolDate_Time::decr_min (int n) {
  incr_min (-n);
}


// decr_hour -- Retreat Date/Time by "n" hour(s)
// Input:       Number of hours to retreat (default 1)
// Output:      None -- CoolDate_Time object updated 

inline void CoolDate_Time::decr_hour (int n) {
  incr_hour (-n);
}


// decr_day -- Retreat Date/Time by "n" day(s)
// Input:      Number of days to retreat (default 1)
// Output:     None -- CoolDate_Time object updated 

inline void CoolDate_Time::decr_day (int n) {
  incr_day (-n);
}


// decr_week -- Retreat Date/Time by "n" week(s)
// Input:       Number of weeks to retreat (default 1)
// Output:      None -- CoolDate_Time object updated 

inline void CoolDate_Time::decr_week (int n) {
  incr_week (-n);
}


// operator== -- Determine if two CoolDate_Time objects represent the
//               same point in time
// Input:        this*, CoolDate_Time reference
// Output:       TRUE/FALSE boolean value

inline Boolean CoolDate_Time::operator== (const CoolDate_Time& d) const {
  return ((this->time_seconds == d.time_seconds) ? TRUE : FALSE);
}


// operator!= -- Determine if two CoolDate_Time objects represent 
//               different points in time
// Input:        this*, CoolDate_Time reference
// Output:       TRUE/FALSE Boolean value

inline Boolean CoolDate_Time::operator!= (const CoolDate_Time& d) const {
  return ((this->time_seconds != d.time_seconds) ? TRUE : FALSE);
}


// operator< -- Determine if one CoolDate_Time object comes before
//              some other point in time
// Input:       this*, CoolDate_Time reference
// Output:      TRUE/FALSE Boolean value

inline Boolean CoolDate_Time::operator< (const CoolDate_Time& d) const {
  return ((this->time_seconds < d.time_seconds) ? TRUE : FALSE);
}


// operator> -- Determine if one CoolDate_Time object comes aftern
//              some other point in time
// Input:       this*, CoolDate_Time reference
// Output:      TRUE/FALSE Boolean value

inline Boolean CoolDate_Time::operator> (const CoolDate_Time& d) const {
  return ((this->time_seconds > d.time_seconds) ? TRUE : FALSE);
}


// operator<= -- Compare one CoolDate_Time object to see if it lies before
//               or is the same as another CoolDate_Time object
// Input:        this*, CoolDate_Time reference
// Output:       TRUE/FALSE Boolean value

inline Boolean CoolDate_Time::operator<= (const CoolDate_Time& d) const {
  return ((this->time_seconds <= d.time_seconds) ? TRUE : FALSE);
}


// operator>= -- Compare one CoolDate_Time object to see if it lies after
//               or is the same as another CoolDate_Time object
// Input:        this*, CoolDate_Time reference
// Output:       TRUE/FALSE boolean value

inline Boolean CoolDate_Time::operator>= (const CoolDate_Time& d) const {
  return ((this->time_seconds >= d.time_seconds) ? TRUE : FALSE);
}


// get_sec -- Accessor to seconds slot value
// Input:     this* 
// Output:    Integer representing number of seconds

inline int CoolDate_Time::get_sec () const {
  return (this->dt.tm_sec);
}


// get_min -- Accessor to minutes slot value
// Input:     this* 
// Output:    Integer representing number of minutes

inline int CoolDate_Time::get_min () const {
  return (this->dt.tm_min);
}


// get_hour -- Accessor to hour slot value
// Input:      this* 
// Output:     Integer representing number of hour

inline int CoolDate_Time::get_hour () const {
  return (this->dt.tm_hour);
}


// get_mday -- Accessor to day of month slot value
// Input:      this* 
// Output:     Integer representing day of month

inline int CoolDate_Time::get_mday () const {
  return (this->dt.tm_mday);
}


// get_mon -- Accessor to month slot value
// Input:     this* 
// Output:    Integer representing month

inline int CoolDate_Time::get_mon () const {
  return (this->dt.tm_mon);
}


#define YEAR_PERIOD 28
#define NEW_YEAR(x) (((x / YEAR_PERIOD) * YEAR_PERIOD) + YEAR_PERIOD)
#define OLD_YEAR(x,y) (x - NEW_YEAR (y))


// get_year -- Accessor to year slot value
// Input:      this* 
// Output:     Integer representing year

inline int CoolDate_Time::get_year () const {
  return ((this->year_adjust > 0) ? (this->century+OLD_YEAR (this->dt.tm_year,
                                               this->year_adjust)) :
          1900 + this->dt.tm_year);
}


// get_yday -- Accessor to day of year slot value
// Input:      this* 
// Output:     Integer representing day of year

inline int CoolDate_Time::get_yday () const {
  return (this->dt.tm_yday);
}


// get_wday -- Accessor to day of week slot value
// Input:      this* 
// Output:     Integer representing day of week

inline int CoolDate_Time::get_wday () const {
  return (this->dt.tm_wday);
}


// is_day_light_savings -- Accessor to Day Light Savings time flag
// Input:                  this* 
// Output:                 TRUE/FALSE if in effect

inline Boolean CoolDate_Time::is_day_light_savings () const {
  return ((this->dt.tm_isdst) ? TRUE : FALSE);
}


// get_time_zone -- Accessor to ASCII representation of time zone
// Input:           this* 
// Output:          character string representing time zone

inline const char* CoolDate_Time::get_time_zone () const {
  return (tz_table[this->tz_code]);
}


// get_country -- Accessor to country code slot value
// Input:         this* 
// Output:        character string representing country

inline const char* CoolDate_Time::get_country () const {
  return (country_names[this->c_code]);
}


// get_day_name -- Accessor to ASCII representation of week day
// Input:         this* 
// Output:        character string representing day of week

inline const char* CoolDate_Time::get_day_name () const {
  return (day_names[this->dt.tm_wday]);
}


// get_month_name -- Accessor to ASCII representation of month
// Input:         this* 
// Output:        character string representing month

inline const char* CoolDate_Time::get_month_name () const {
  return (month_names[this->dt.tm_mon]);
}

// operator<< -- Output CoolDate_Time object to output stream
// Input:        Output stream reference, CoolDate_Time pointer
// Output:       Output stream reference

inline ostream& operator<< (ostream& os, const CoolDate_Time* d) {
  return  os << *d;
}


#endif                                          // End of DATETIMEH
