#include <signal.h>
#include <sys/magic.h>
#include "fpexcept.h"


void __FPE_exception( void )
{
    kill( __MAGIC.my_pid, SIGFPE );
}
