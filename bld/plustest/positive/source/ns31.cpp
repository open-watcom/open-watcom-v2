#include "fail.h"

#if __WATCOM_REVISION__ >= 8
namespace U {
    int j = 2;
};
namespace CFSupp {
    using namespace U;
    using namespace U;
    using namespace U;
    namespace {
	int i = 3;
    };
};
namespace CFSupp {
    int foo() {
	return ++j + ++i;
    }
};

int main() {
    if( CFSupp::foo() != 7 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
