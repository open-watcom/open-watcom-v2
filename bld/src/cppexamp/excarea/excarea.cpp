#include <stddef.h>
#include <limits.h>

#if 0   // CANCEL DEFAULTS

#if INT_MAX == SHRT_MAX
#define EXC_SIZE 1024           // size of exception area
#else
#define EXC_SIZE 4096           // size of exception area
#endif

#else   // MODIFICATIONS START
#define EXC_SIZE 1024*10        // use 10k for example
#endif  // MODIFICATIONS END

struct exc_area
{
    size_t  area_size;
    char    work_area[ EXC_SIZE - sizeof( size_t ) ];
};

extern "C"
struct exc_area __EXC_AREA = { EXC_SIZE };
