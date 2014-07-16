#include "fail.h"

#ifdef __WATCOM_RTTI__

#include <typeinfo.h>
class D {};
D d1;
const D d2;
int main() {
    if( typeid(d1) != typeid(d2) ) _fail;
    if( typeid(D)  != typeid(const D) ) _fail;
    if( typeid(D)  != typeid(d2) ) _fail;
    if( typeid(D)  != typeid(const D&) ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
