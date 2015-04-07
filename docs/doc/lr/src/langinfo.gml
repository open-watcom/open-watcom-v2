.func nl_langinfo
.synop begin
#include <langinfo.h>
char *nl_langinfo( int item ); 
.synop end
.desc begin
The
.id &funcb.
function retrieves language information, purportedly based on the
current locale.  The
.arg item
argument specifies which language element to retrieve, and may be one
of the following:
.begterm 49
.termhd1 Member
.termhd2 Meaning
.term CODESET
The character encoding supported by this locale
.term D_T_FMT
The date and time format string
.term D_FMT
The date format string
.term T_FMT
The 24-hour time format string
.term T_FMT_AMPM
The 12-hour time format string
.term AM_STR
The string representing ante-meridiem time
.term PM_STR
The string representing post-meridiem time
.term DAY_1
The name of the first day of the week
.term DAY_2
The name of the second day of the week
.term DAY_3
The name of the third day of the week
.term DAY_4
The name of the fourth day of the week
.term DAY_5
The name of the fifth day of the week
.term DAY_6
The name of the sixth day of the week
.term DAY_7
The name of the seventh day of the week
.term ABDAY_1
The abbreviated name of the first day of the week
.term ABDAY_2
The abbreviated name of the second day of the week
.term ABDAY_3
The abbreviated name of the third day of the week
.term ABDAY_4
The abbreviated name of the fourth day of the week
.term ABDAY_5
The abbreviated name of the fifth day of the week
.term ABDAY_6
The abbreviated name of the sixth day of the week
.term ABDAY_7
The abbreviated name of the seventh day of the week
.term MON_1
The name of the first month of the year
.term MON_2
The name of the second month of the year
.term MON_3
The name of the third month of the year
.term MON_4
The name of the fourth month of the year
.term MON_5
The name of the fifth month of the year
.term MON_6
The name of the sixth month of the year
.term MON_7
The name of the seventh month of the year
.term MON_8
The name of the eighth month of the year
.term MON_9
The name of the ninth month of the year
.term MON_10
The name of the tenth month of the year
.term MON_11
The name of the eleventh month of the year
.term MON_12
The name of the twelfth month of the year
.term ABMON_1
The abbreviated name of the first month of the year
.term ABMON_2
The abbreviated name of the second month of the year
.term ABMON_3
The name of the third month of the year
.term ABMON_4
The abbreviated name of the fourth month of the year
.term ABMON_5
The abbreviated name of the fifth month of the year
.term ABMON_6
The abbreviated name of the sixth month of the year
.term ABMON_7
The abbreviated name of the seventh month of the year
.term ABMON_8
The abbreviated name of the eighth month of the year
.term ABMON_9
The abbreviated name of the ninth month of the year
.term ABMON_10
The abbreviated name of the tenth month of the year
.term ABMON_11
The abbreviated name of the eleventh month of the year
.term ABMON_12
The abbreviated name of the twelfth month of the year
.term RADIXCHAR
The radix character
.term THOUSEP
The separator for thousands
.term YESEXPR
The string for indicating affirmative responses
.term NOEXPR
The string for indicating negative responses
.endterm
In the default locale (C), some values may be U.S.A.-centric, specifically
the RADIXCHAR and THOUSEP.  Additionally, names are in English presently.
.desc end
.return begin
The function returns a pointer to a statically allocated string that must
not be freed.  If
.arg item
is either unsupported or unknown, an empty string will be returned.
.return end
.class POSIX
.system
