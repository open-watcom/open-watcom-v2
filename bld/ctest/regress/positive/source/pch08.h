#if defined(AFS_CM)
#define TEST "def'd"
#else
#define TEST "undef'd"
#endif
#include "fail.h"
