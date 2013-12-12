.func nosound
.synop begin
#include <i86.h>
void nosound( void );
.ixfunc2 '&DosFunc' &func
.synop end
.desc begin
The &func function turns off the PC's speaker.
.im privity
.desc end
.return begin
The &func function has no return value.
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
