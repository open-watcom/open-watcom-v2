#include <signal.h>
#include <sys/magic.h>
#include "fpexcept.h"


void __FPE_exception( int fpe )
{
    /* unused parameters */ (void)fpe;

    kill( __MAGIC.my_pid, SIGFPE );
}
