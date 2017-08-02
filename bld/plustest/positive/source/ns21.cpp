#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace Company_with_very_long_name {
    int x = 'x';
}
namespace CWVLN = Company_with_very_long_name;
namespace CWVLN = Company_with_very_long_name;  // ok: duplicate
namespace CWVLN = CWVLN;

int main() {
    if( CWVLN::x++ != 'x' ) _fail;
    if( CWVLN::x++ != 'y' ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
