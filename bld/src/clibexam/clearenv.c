
#include <env.h>

void main()
  {
    clearenv();
    setenv( "TZ", "EST5EDT", 0 );
  }

