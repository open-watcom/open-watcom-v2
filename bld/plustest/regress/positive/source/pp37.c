#include "fail.h"

#if false
#error "false should really be false"
#endif

#if !true
#error "!true should also be false"
#endif

#if true && ! false
int main( ) {
    _PASS;
}
#endif
