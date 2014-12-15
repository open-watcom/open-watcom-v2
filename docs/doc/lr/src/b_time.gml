.func _bios_timeofday
.synop begin
#include <bios.h>
int _bios_timeofday( int service, long *timeval );
.ixfunc2 '&BiosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function uses INT 0x1A to get or set the current
system clock value.
The values for service are:
.begterm 12
.termhd1 Value
.termhd2 Meaning
.term _TIME_GETCLOCK
Places the current system clock value in the location pointed to by
.arg timeval
.ct .li .
The function returns zero
if midnight has not passed since the last time the system clock was
read or set; otherwise, it returns 1.
.term _TIME_SETCLOCK
Sets the system clock to the value in the location pointed to by
.arg timeval
.ct .li .
.endterm
.desc end
.return begin
A value of -1 is returned if neither _TIME_GETCLOCK nor _TIME_SETCLOCK
were specified; otherwise 0 is returned.
.return end
.exmp begin
#include <stdio.h>
#include <bios.h>

void main()
  {
    long time_of_day;
.exmp break
    _bios_timeofday( _TIME_GETCLOCK, &time_of_day );
    printf( "Ticks since midnight: %lu\n", time_of_day );
  }
.exmp output
Ticks since midnight: 762717
.exmp end
.class BIOS
.system
