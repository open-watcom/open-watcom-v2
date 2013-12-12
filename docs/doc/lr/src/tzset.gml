.func tzset
.synop begin
#include <time.h>
void tzset( void );
.synop end
.desc begin
The &func function sets the
global
variables
.kw daylight
.ct,
.kw timezone
and
.kw tzname
according to the value of the
.kw TZ
environment variable.
The section
.us The TZ Environment Variable
describes how to set this variable.
.if '&machsys' ne 'QNX' .do begin
.np
Under Win32, &func also uses operating system supplied time zone
information.
The
.kw TZ
environment variable can be used to override this information.
.do end
.np
The
global
variables have the following values after &func is executed:
.begterm 12
.term .mono daylight
Zero indicates that daylight saving time is not supported in the
locale; a non-zero value indicates that daylight saving time is
supported in the locale.
This variable is cleared/set after a call to the &func function
depending on whether a daylight saving time abbreviation is specified
in the
.kw TZ
environment variable.
.term .mono timezone
Contains the number of seconds that the local time zone is earlier
than Coordinated Universal Time (UTC) (formerly known as Greenwich
Mean Time (GMT)).
.term .mono tzname
Two-element array pointing to strings giving the abbreviations for the
name of the time zone when standard and daylight saving time are in
effect.
.endterm
.im tzref
.desc end
.return begin
The &func function does not return a value.
.return end
.see begin
.seelist tzset ctime Functions localtime mktime strftime
.see end
.cp 12
.exmp begin
#include <stdio.h>
#include <env.h>
#include <time.h>

void print_zone()
  {
    char *tz;
.exmp break
    printf( "TZ: %s\n", (tz = getenv( "TZ" ))
                    ? tz : "default EST5EDT" );
    printf( "  daylight: %d\n", daylight );
    printf( "  timezone: %ld\n", timezone );
    printf( "  time zone names: %s %s\n",
            tzname[0], tzname[1] );
  }
.exmp break
void main()
  {
    print_zone();
    setenv( "TZ", "PST8PDT", 1 );
    tzset();
    print_zone();
  }
.exmp output
TZ: default EST5EDT
  daylight: 1
  timezone: 18000
  time zone names: EST EDT
TZ: PST8PDT
  daylight: 1
  timezone: 28800
  time zone names: PST PDT
.exmp end
.class POSIX 1003.1
.system
