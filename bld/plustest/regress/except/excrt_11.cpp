// 94/06/09 -- jww -- fixed R/T system

#include "fail.h"
#include <except.h>

void my_terminate()
{
   throw __FILE__ ": should never see this printed!" ;
}

int main()
{
   set_terminate(my_terminate) ;
   try
   {
      throw 1 ;  // no catch block
   }
   catch(const char* message)
   {
      printf( "%s\n", message );
      _fail;
   }
   _PASS;
}
