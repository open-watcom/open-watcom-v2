/* Same as prag01.c, for PCH testing only */

#include "alias.h"

#include "fail.h"

#include "header.h"
#include "foo.h"
#include <foo.h>

int main( void ) 
{
    dummy = dummy2 = 3;
    _PASS;
}
