#include "fail.h"

#ifdef __WATCOM_NAMESPACE__
namespace { int i = 1; }// unique::i
int f() { return i++; }	// unique::i++

namespace A {
    namespace {
	int i = 2;	// A::unique::i
	int j = 3;	// A::unique::j
    }
    int g() { return i++; }  // A::unique::i++
}

using namespace A;

int main() {
    if( f() != 1 ) _fail;
    if( f() != 2 ) _fail;
    if( A::i++ != 2 ) _fail;
    if( A::i++ != 3 ) _fail;
    if( A::i++ != 4 ) _fail;
    if( j++ != 3 ) _fail;
    if( j++ != 4 ) _fail;
    _PASS;
}
#else
ALWAYS_PASS
#endif
