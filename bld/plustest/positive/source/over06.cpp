#include "fail.h"

#if __WATCOM_REVISION__ >= 8

typedef char stringtype[10];

int calls;

void charptr_function(char *string)
{
    ++calls;
}

void string_function(stringtype &string)
{
    ++calls;
}

int main()
{
   stringtype string;

   strcpy( string, "test" );
   charptr_function( string );
   string_function( string );
   if( calls != 2 ) fail(__LINE__);
   _PASS;
}

#else
ALWAYS_PASS
#endif
