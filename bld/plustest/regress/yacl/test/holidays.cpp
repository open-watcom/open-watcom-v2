

// YACL demo to print out the holidays for a given year
//
// This program is based on the one in "Data abstraction and
// object-oriented programming in C++" by Gorlen, Orlow & Plexico (John
// Wiley, 1990), p. 74.

// Invocation:
//
//           holidays [year]
//
// For example,
//
//           holidays 1994
//
// prints the list of holidays in 1994. Without an argument, the list of
// holidays for the current year is printed.


// M. A. Sridhar, 5/30/94


#include "base/date.h"
#include <stdio.h>

#define HolidayCount 10

class Holidays {

    struct Holiday {
        CL_Date date;
        CL_String desc;
    } day [HolidayCount];
  
public:
    Holidays (short year);
    // Build an array of holidays for the given year.

    short Size() {return HolidayCount;};
    // Return the number of holidays in the year.
    
    const Holiday& operator[] (short i) {return day[i];};
    // Return the i-th holiday of the year.



};




Holidays::Holidays (short year)
{
    // New year's day:
    day[0].date = CL_Date (year, CL_Date::January, 1);
    day[0].desc = "New year's day";

    // M. L. King's birthday: third Monday of January
    day[1].date = CL_Date (year, CL_Date::January, 21).PreviousWeekday
        ("Monday");
    day[1].desc = "M. L. King's birthday";

    // Washington's birthday: third Monday of February
    day[2].date = CL_Date (year, CL_Date::February, 21).PreviousWeekday
        (CL_Date::Monday);
    day[2].desc = "Washington's birthday";

    // Memorial day: last Monday of May
    day[3].date = CL_Date (year, CL_Date::May, 31).PreviousWeekday ("Monday");
    day[3].desc = "Memorial day";

    // Independence day
    day[4].date = CL_Date (year, CL_Date::July, 4);
    day[4].desc = "Independence day";

    // Labor day: first Monday of September
    day[5].date = CL_Date (year, CL_Date::August, 31).NextWeekday ("Monday");
    day[5].desc = "Labor day";

    // Columbus day: second Monday of October
    day[6].date = CL_Date (year, CL_Date::October, 14).PreviousWeekday
        ("Monday");
    day[6].desc = "Columbus day";

    // Veteran's day: November 11th
    day[7].date = CL_Date (year, CL_Date::November, 11);
    day[7].desc = "Veteran's day";

    // Thanksgiving: fourth Thursday of November
    day[8].date = CL_Date (year, CL_Date::November, 28).PreviousWeekday
        ("Thursday");
    day[8].desc = "Thanksgiving";

    // Christmas
    day[9].date = CL_Date (year, CL_Date::December, 25);
    day[9].desc = "Christmas day";
}





main (int argc, char* argv[])
{
    short year;
    if (argc <= 1) 
        year = CL_Date::Today().Year ();
    else
        year = minl (1999, maxl (1901, CL_String (argv[1]).AsLong()));

    Holidays all_hols (year);
    for (short i = 0; i < all_hols.Size(); i++) {
        CL_Date d = all_hols[i].date;
        CL_String dt = d.AsString () + " ("
            + CL_Date::DayName (d.DayOfWeek()) + "): ";
        dt.PadTo (25);
        printf ("%s %s\n", dt.AsPtr(), all_hols[i].desc.AsPtr());
    }
    return 0;
}
