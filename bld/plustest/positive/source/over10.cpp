#include "fail.h"

typedef char stringtype[10];

void charptr_function(char *string)
{
    if( strcmp( string, "test" ) != 0 ) fail(__LINE__);
}

void string_function(stringtype &string)
{
    if( strcmp( string, "test" ) != 0 ) fail(__LINE__);
}

int main(void)
{
   stringtype string;

   strcpy(string, "test");
   charptr_function(string);
#if __WATCOM_REVISION__ >= 8
   string_function(string);
#endif
   _PASS;
}
