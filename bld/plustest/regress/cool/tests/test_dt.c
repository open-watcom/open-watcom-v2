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

#if defined(DOS)
extern "C" {
#include <string.h>                             // for strcmp()
}
#else
#include <string.h>                             // for strcmp()
#endif

#include <cool/Date_Time.h>
#include <cool/test.h>


// the tst function helps make the TESTS for the parser easier
// and cleaner.

int tst(CoolDate_Time& obj, int yr, int mo,int da,int hr,int mn,int sc) {
  if (obj.get_year() == yr &&
      obj.get_mon() == mo-1 &&
      obj.get_mday() == da &&
      obj.get_hour() == hr &&
      obj.get_min() == mn &&
      obj.get_sec() == sc ) return(1);
  else return(0);
}

void test_date_time() {    
  set_default_country (UNITED_STATES);
  set_default_time_zone(US_CENTRAL);
  CoolDate_Time d3;
  TEST("set_default_country(US)",strcmp (d3.get_country(),"United States"),0);
  TEST("set_default_time_zone(CST)",strcmp(d3.get_time_zone(),"US/Central"),0);
  CoolDate_Time current;
  current.set_local_time();
  int year = current.get_year();
  int month = current.get_mon();
  month++;
  int day = current.get_mday();
  CoolDate_Time d1; 
  d1.set_country(UNITED_STATES);
  TEST ("d1.set_country(UNITED_STATES)", strcmp (d1.get_country(),
        "United States"), 0);
  d1.set_time_zone(US_CENTRAL);
  TEST ("d1.set_time_zone(US_CENTRAL)", strcmp (d1.get_time_zone(),
        "US/Central"), 0);

  d1.parse("03/10/90, 4:30am");
  TEST ("03/10/90, 4:30am", tst(d1,1990,3,10,4,30,0), 1);
 
  d1.parse("03/10/90, 4:30 pm");
  TEST ("03/10/90, 4:30 pm", tst(d1,1990,3,10,16,30,0),1);

  d1.parse("03/10/20, 4:30pm");
  TEST ("03/10/20, 4:30pm", tst(d1,year,3,10,20,4,30),1);
  
  d1.parse( "4/21/90");
  TEST ("4/21/90",tst(d1,1990,4,21,0,0,0),1);
  
  d1.parse( " 90 5:15Am ");
  TEST (" 90 5:15Am ", tst(d1,1990,1,1,5,15,0),1);
  
  d1.parse("3 pM 90");
  TEST ("3 pM 90",tst(d1,1990,1,1,15,0,0),1);
    
  d1.parse( "4pm 1 5 94");
  TEST ("4pm 1 5 94", tst(d1,1994,1,5,16,0,0),1);
  
  d1.set_country(UNITED_KINGDOM);
  d1.set_time_zone(GB_EIRE);
  d1.parse( "4pm 1 5 94");
  TEST ("4pm 1 5 94", tst(d1,1994,5,1,16,0,0),1);

  d1.set_country(SWEDEN);
  d1.set_time_zone(WET);
  d1.parse( "4pm 94 1 5 ");
  TEST ("4pm 94 1 5", tst(d1,1994,5,1,16,0,0),1);

  d1.set_country(FRENCH_CANADIAN);
  d1.set_time_zone(US_CENTRAL);
  d1.parse( "4pm 94 1 5 ");
  TEST ("4pm 94 1 5", tst(d1,1994,5,1,16,0,0),1);
  
  d1.parse( "1984 4 23");
  TEST ("1984 4 23", tst(d1,1984,4,23,0,0,0),1);

  d1.set_country(UNITED_KINGDOM);
  d1.set_time_zone(GB_EIRE);
  d1.parse( "3 22 1975");
  TEST ("3 22 1975", tst(d1,1975,3,22,0,0,0),1);

  d1.set_country(UNITED_STATES);
  d1.set_time_zone(US_CENTRAL);
  d1.parse( "24 3 1987"); 
  TEST ("24 3 1987", tst(d1,1987,3,24,0,0,0),1);
  
  d1.parse( "4 2 3 4 5pm");
  TEST ("4 2 3 4 5pm", tst(d1,year,4,2,15,4,5),1);

  d1.parse( "4 pm 4");
  TEST("4 pm 4", tst(d1,year,month,4,16,0,0),1);

  d1.parse( " 05 05 05 05 05");
  TEST (" 05 05 05 05 05",tst(d1,year,5,5,5,5,5),1);
  
  d1.parse( "2 22 3");
  TEST("2 22 3",tst(d1,year,2,22,3,0,0),1);

  d1.parse( " 05 05 05 FEB 05 05");
  TEST(" 05 05 05 FEB 05 05", tst(d1,year,5,5,5,2,5),1);
  
  d1.parse( "Jun 4 1991, 4:40 pm");
  TEST("Jun 4 1991, 4:40 pm", tst(d1,1991,6,4,16,40,0),1);
  
  d1.parse( "30 NovEMber 91");
  TEST("30 NovEMber 91",tst(d1,1991,11,30,0,0,0),1);
  
  d1.parse( "march 10");
  TEST("march 10",tst(d1,year,3,10,0,0,0),1);

  d1.set_time_zone(NZ);
  d1.parse( "5:44pm 6 15 90");
  TEST ("d1.get_time_zone()", strcmp (d1.get_time_zone(),"NZ"), 0);
 
  d1.parse( "5:44PM 6 15 90", 1);
  TEST("5:44pm 6 15 90", tst(d1,1990,6,15,17,44,0),1);
  TEST ("d1.get_time_zone()", strcmp (d1.get_time_zone(),"NZ"), 0);

  d1.set_country(UNITED_STATES);
  d1.set_time_zone(US_CENTRAL);
  char buf[] = "5:44pm United States US/Central10 15 86";
  d1.parse( buf, 1);
  TEST("5:44pm US/Central10 15 86", tst(d1,1986,10,15,17,44,0),1);
  TEST ("d1.get_time_zone()", strcmp (d1.get_time_zone(),"US/Central"), 0);

  d1.parse("04/21/63, 4:30am");
  TEST ("04/21/63, 4:30am", tst(d1,1963,4,21,4,30,0), 1);
 
  d1.parse("24 May 1965, 4:30 pm");
  TEST ("24 May 1965, 4:30 pm", tst(d1,1965,5,24,16,30,0),1);
 
  d1.parse( "5:44PM 6 15 1890", 1);
  TEST("5:44pm 6 15 1890", tst(d1,1890,6,15,17,44,0),1);

  d1.set_local_time();
  struct tm* t;                              // Temporary pointer variable
  time_t sys_seconds = time ((time_t *)0);           // System GMT time in seconds
  t = localtime (&sys_seconds);              // Convert seconds to local time

  TEST ("d1.get_sec()", (d1.get_sec() < 60 &&
                         d1.get_sec() >= 0 &&
                         d1.get_sec() == t->tm_sec), 1);
  TEST ("d1.get_min()", (d1.get_min() < 60 &&
                         d1.get_min() >= 0 &&
                         d1.get_min() == t->tm_min), 1);
  TEST ("d1.get_hour()", (d1.get_hour() < 24 &&
                          d1.get_hour() >= 0 &&
                          d1.get_hour() == t->tm_hour), 1);
  TEST ("d1.get_mon()", (d1.get_mon() < 12 &&
                         d1.get_mon() >= 0 &&
                         d1.get_mon() == t->tm_mon), 1);
  TEST ("d1.get_mday()", (d1.get_mday() < 32 &&
                          d1.get_mday() >= 0 &&
                          d1.get_mday() == t->tm_mday), 1);
  TEST ("d1.get_year()", (d1.get_year() >= 1989 &&
                          d1.get_year() == t->tm_year+1900), 1);
  TEST ("d1.get_yday()", (d1.get_yday() < 367 &&
                          d1.get_yday() >= 0 &&
                          d1.get_yday() == t->tm_yday), 1);
  TEST ("d1.is_day_light_savings()", (d1.is_day_light_savings() ? 1 : 0),
        t->tm_isdst);
  TEST ("d1.set_local_time()",(d1.get_min() == t->tm_min &&
                               d1.get_hour() == t->tm_hour &&
                               d1.get_mday() == t->tm_mday &&
                               d1.get_mon() == t->tm_mon &&
                               d1.get_year() == t->tm_year+1900 &&
                               d1.get_wday() == t->tm_wday &&
                               d1.get_yday() == t->tm_yday), 1);
  // d1.parse("24 May 1993, 4:30 pm");
  CoolDate_Time d2 = d1;
  TEST ("CoolDate_Time d2 = d1", 1, 1);
  TEST ("d1+=SECOND", (d1+=SECOND, d1-d2 == SECOND), 1);
  d1 = d2;
  TEST ("d1+=MINUTE", (d1+=MINUTE, d1-d2 == MINUTE), 1);
  d1 = d2;
  TEST ("d1+=HOUR", (d1+=HOUR, d1-d2 == HOUR), 1);
  d1 = d2;
  TEST ("d1+=DAY", (d1+=DAY, d1-d2 == DAY), 1);
  d1 = d2;
  TEST ("d1+=WEEK", (d1+=WEEK, d1-d2 == WEEK), 1);
  d1 = d2;
  TEST ("d1+=YEAR", (d1+=YEAR, d1-d2 == YEAR), 1);
  d1 = d2;
  TEST ("d1-=SECOND", (d1-=SECOND, d2-d1 == SECOND), 1);
  d1 = d2;
  TEST ("d1-=MINUTE", (d1-=MINUTE, d2-d1 == MINUTE), 1);
  d1 = d2;
  TEST ("d1-=HOUR", (d1-=HOUR, d2-d1 == HOUR), 1);
  d1 = d2;
  TEST ("d1-=DAY", (d1-=DAY, d2-d1 == DAY), 1);
  d1 = d2;
  TEST ("d1-=WEEK", (d1-=WEEK, d2-d1 == WEEK), 1);
  d1 = d2;
  TEST ("d1-=YEAR", (d1-=YEAR, d2-d1 == YEAR), 1);
  d1 = d2;
  TEST ("d1.incr_sec(5)",(d1.incr_sec(5), d1-d2 == SECOND*5), 1);
  d1 = d2;
  TEST ("d1.incr_min(5)",(d1.incr_min(5), d1-d2 == MINUTE*5), 1);
  d1 = d2;
  TEST ("d1.incr_hour(5)",(d1.incr_hour(5), d1-d2 == HOUR*5), 1);
  d1 = d2;
  TEST ("d1.incr_day(5)",(d1.incr_day(5), d1-d2 == DAY*5), 1);
  d1 = d2;
  TEST ("d1.incr_week(5)",(d1.incr_week(5), d1-d2 == WEEK*5), 1);
  d1 = d2;
  TEST ("d1.decr_sec(5)",(d1.decr_sec(5), d2-d1 == SECOND*5), 1);
  d1 = d2;
  TEST ("d1.decr_min(5)",(d1.decr_min(5), d2-d1 == MINUTE*5), 1);
  d1 = d2;
  TEST ("d1.decr_hour(5)",(d1.decr_hour(5), d2-d1 == HOUR*5), 1);
  d1 = d2;
  TEST ("d1.decr_day(5)",(d1.decr_day(5), d2-d1 == DAY*5), 1);
  d1 = d2;
  TEST ("d1.decr_week(5)",(d1.decr_week(5), d2-d1 == WEEK*5), 1);
  TEST ("d1 == d2", d1 == d2, FALSE);
  TEST ("d1 != d2", d1 != d2, TRUE);
  TEST ("d1 < d2", d1 < d2, TRUE);
  TEST ("d1 <= d2", d1 <= d2, TRUE);
  TEST ("d1 > d2", d1 > d2, FALSE);
  TEST ("d1 >= d2", d1 >= d2, FALSE);
  d1 = d2;
  d1 += ((2*YEAR) + (14*WEEK) + (4*DAY) +
         (15*HOUR) + (27*MINUTE) + (43*SECOND));
  TEST ("d1.ascii_duration(d2)", strcmp (d1.ascii_duration(d2),
        "2 years, 14 weeks, 4 days, 15 hours, 27 minutes, 43 seconds"), 0);
}
 
void test_leak() {
  for (;;) {
    test_date_time();
  }
}


int main (void) {
  START("CoolDate_Time");
  test_date_time();
#if LEAK
  test_leak();
#endif
  SUMMARY();
  return 0;
}
