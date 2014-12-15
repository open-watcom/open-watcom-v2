.func nosound
.synop begin
#include <i86.h>
void nosound( void );
.ixfunc2 '&DosFunc' &funcb
.synop end
.desc begin
The
.id &funcb.
function turns off the PC's speaker.
.im privity
.desc end
.return begin
The
.id &funcb.
function has no return value.
.return end
.see begin
.seelist nosound delay sound
.see end
.exmp begin
#include <i86.h>

void main()
  {
    sound( 200 );
    delay( 500 );  /* delay for 1/2 second */
    nosound();
  }
.exmp end
.class Intel
.system
