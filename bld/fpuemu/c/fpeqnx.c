#include <signal.h>
#include <sys/magic.h>


extern void __FPEhandler( void );

void __FPEhandler( void )
{
    kill( __MAGIC.my_pid, SIGFPE );
}
