#include <signal.h>
#include <sys/magic.h>

void __FPEhandler( void )
{
    kill( __MAGIC.my_pid, SIGFPE );
}
