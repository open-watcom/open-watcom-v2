#include <signal.h>
#include <sys/magic.h>

#ifdef __386__
#pragma aux __FPE_handler "_*_";
#else
#pragma aux __FPE_handler "_*";
#endif

void __FPE_handler( void )
{
    kill( __MAGIC.my_pid, SIGFPE );
}
