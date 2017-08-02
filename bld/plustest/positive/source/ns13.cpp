#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
    namespace B {
    };
    namespace C {
	namespace D {
	    using namespace B;
	};
    };
    int i = 'a';
    namespace B {
	using namespace C::D;
	int i = 'b';
	int k = i;
    };
};

int main() {
    if( A::i != 'a' ) _fail;
    if( A::B::i != 'b' ) _fail;
    if( A::B::k != 'b' ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
