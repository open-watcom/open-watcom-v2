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
#include <time.h>

#include <cool/Date_Time.h>                     // Include Date_Time class

void test_stream_local( CoolDate_Time &set )
{
    int start;
    int check;

    set.set_local_time ();
    start = set.get_sec();
    for(;;) {
        set.set_local_time ();
        check = set.get_sec();
        if( check != start ) break;
    }
    set.set_country (UNITED_STATES);
    set.set_time_zone (US_CENTRAL);
    set.parse("01-01-1980 17:12:34");
}

int main (void) {
    cout << __FILE__ << endl;
  set_default_country (UNITED_STATES);          // Set default country code
  set_default_time_zone (US_CENTRAL);           // Set default time zone
  CoolDate_Time d1;                             // Create CoolDate_Time object
  test_stream_local( d1 );
  cout << "Local date/time is: " << d1 << "\n"; // Output date in US format
  d1.set_country (UNITED_KINGDOM);              // Set country to UK
  d1.set_time_zone (GB_EIRE);                   // Set Greenwich Mean Time
  cout << "GMT date/time is: " << d1 << "\n";   // Output date/time at GMT
  d1.parse("1 April 1890, 4:30pm");             // Parse some date in UK format
  cout << "Date/time parsed is: " << d1 << "\n"; // Output date/time parsed
  d1.set_country (FRANCE);                       // Set country to France
  d1.set_time_zone (WET);                        // Western European Timezone
  cout << "Date/time in France: " << d1 << "\n"; // Output date/time in France
  test_stream_local( d1 );
  CoolDate_Time d2;                              // Create another object
  d2.set_country (UNITED_STATES);                // Set country to US
  d2.set_time_zone (US_MOUNTAIN);                // Set Mountain Timezone
  test_stream_local( d2 );
  cout << "Date/time set is: " << d2 << "\n";    // Output date in US format
  d2.decr_month (13);                            // Move back thirteen months
  cout << "Date/time thirteen months earlier: " << d2 << "\n"; // Output date
  cout << "Duration between dates is ";                     // 
  cout << d1.ascii_duration (d2) << "\n";                   // Output time duration
  return 0;                                                 // Return valid sucess code
}
