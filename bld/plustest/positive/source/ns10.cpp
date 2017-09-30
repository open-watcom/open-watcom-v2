#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace M {
    int i ='m';
    int j = 'q';
}

namespace N {
    int i = 'n';
    using namespace M;
}

int main() {
    if( N::i != 'n' ) _fail;
    N::i = 7;
    using namespace N;                                      
    if( j != 'q' ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
