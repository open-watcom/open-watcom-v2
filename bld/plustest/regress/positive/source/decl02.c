#include "fail.h"

#if __WATCOM_REVISION__ >= 8

struct bar {
    int x;
    bar ();
};
struct foo {
  foo ();
  bar x;
  const bar y;
};

int c;

foo::foo () {
    ++c;
}

const bar x;
const foo y;

bar::bar () {
    ++c;
}

#else
int c = 4;
#endif

int main() {
    if( c != 4 ) _fail;
    _PASS;
}
