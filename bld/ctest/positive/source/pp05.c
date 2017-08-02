#include "fail.h"

int main()
{
    {{{
#if 0
/*			\
    equivalent to:	\
    #if 0		\
    #else		\
    }			\
    #endif		\
*/			\
#else
    }
#endif
    }}
    _PASS;
}
