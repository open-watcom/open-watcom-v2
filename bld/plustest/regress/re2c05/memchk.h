#ifndef __MEMCHK_H
#define __MEMCHK_H

#define MEMCHK_STATUS( e, s )	e,

enum memcheck_error {
#include "memchkst.h"
};

#endif
