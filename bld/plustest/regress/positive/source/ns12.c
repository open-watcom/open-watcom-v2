#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace A {
    int i = 'A';
};
namespace B {
    int i = 'I';
    int j = 'J';
    namespace C {
	namespace D {
	    using namespace A;
	    int j = 'D';
	    int k = 'K';
	    int a = i;	// B::i hides A::i
	};
	using namespace D;
	int k = 89;
	int m = i;	// B::i hides A::i
	int n = j;	// D::j hides B::j
    };
};

int main() {
    if( A::i != 'A' ) _fail;
    if( B::i != 'I' ) _fail;
    if( B::j != 'J' ) _fail;
    if( B::C::D::j != 'D' ) _fail;
    if( B::C::D::k != 'K' ) _fail;
    if( B::C::D::a != 'I' ) _fail;
    if( B::C::k != 89 ) _fail;
    if( B::C::m != 'I' ) _fail;
    if( B::C::n != 'D' ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
