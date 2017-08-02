#include "fail.h"
// test unary versus binary + and -

int main()
{

#if 8 * (3 * (( 7 - (( 1 << 8 ))))) - 1
#else
fail(__LINE__);
#endif

#if 8 * ( 1 ) - 1
#else
fail(__LINE__);
#endif

#if 8 * 1 - 1
#else
fail(__LINE__);
#endif

#if - 8 * 1 + 9
#else
fail(__LINE__);
#endif

#if 8 * (3 * (( 7 - (( 1 << 8 ))))) + 1
#else
fail(__LINE__);
#endif

#if 8 * ( 1 ) + 1
#else
fail(__LINE__);
#endif

#if 8 * 1 + 1
#else
fail(__LINE__);
#endif

#if + 8 * 1 - 7
#else
fail(__LINE__);
#endif

  _PASS;

}
