// test udcf for reference

#include "dump.h"

struct SRC;

struct TGT {
    TGT( SRC & ) {GOOD};
};

struct SRC {
    operator TGT (); // should not allow this, so should be no ambiguity
};

SRC src;


int main()
{
#if __WATCOM_REVISION__ >= 8
    TGT const & tgt = src;
#else
    FORCE_GOOD(8);
#endif
    CHECK_GOOD(8);
    return errors != 0;
}
