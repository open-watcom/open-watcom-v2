#include "fail.h"

extern int var1;
extern int var2;
extern int var3;
extern int var4;
int real_var;

#if defined( __SW_3R ) || defined( _M_I86 )
  #pragma alias( var1, "_real_var" );
  #pragma alias( "_var2", "_real_var" );
  #pragma alias( "_var3", real_var );
#else
  #pragma alias( var1, "real_var" );
  #pragma alias( "var2", "real_var" );
  #pragma alias( "var3", real_var );
#endif
#pragma alias( var4, real_var );

int main( void ) 
{
    var1 = 1;
    if( real_var != 1 ) fail(__LINE__);
    var2 = 2;
    if( real_var != 2 ) fail(__LINE__);
    var3 = 3;
    if( real_var != 3 ) fail(__LINE__);
    var4 = 4;
    if( real_var != 4 ) fail(__LINE__);
    _PASS;
}
