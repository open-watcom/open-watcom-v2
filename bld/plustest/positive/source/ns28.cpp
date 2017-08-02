#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace {
    typedef int T;
};
static T x;

int main() {
    ++x;
    if( x != 1 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
