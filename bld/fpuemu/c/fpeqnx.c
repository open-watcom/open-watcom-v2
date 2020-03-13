#include <signal.h>
#include <sys/magic.h>
#include "fpexcept.h"


void __FPE_exception( int fpe_type )
{
    /* unused parameters */ (void)fpe_type;

    kill( __MAGIC.my_pid, SIGFPE );
}
