#include "fail.h"

#if defined( __OS2__) && defined( _CPPUNWIND )

// Currently does not work on OS/2. With -xs, main() will register
// an exception handler that calls the destructor. When exit() is
// called, eventually the runtime calls DosExit. This will trigger
// XCPT_PROCESS_TERMINATE and the destructor will be run. Note that
// at that point the runtime has been shut down.

ALWAYS_PASS

#else

int ctor;

struct S {
    S() { ++ctor; }
    ~S() { fail(__LINE__); }
};

int main()
{
    S x;    // dtor should not be called!
    if( ctor != 1 ) fail(__LINE__);
    _PASS_EXIT;
}

#endif
