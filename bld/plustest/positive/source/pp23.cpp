#include "fail.h"

#if 0
#if 1
#illegal directive (no error should be reported)
#else
#another illegal directive (no error should be reported)
#endif
#endif

ALWAYS_PASS
