#include "fail.h"

#if __WATCOM_REVISION__ >= 8
namespace X {
    int i = 3;
};
#else
struct X {
    static int i;
};
int X::i = 3;
#endif
