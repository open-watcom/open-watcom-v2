#include "fail.h"

#if __WATCOM_REVISION__ >= 7
#define G640x480x2   9
#define OneModeEntry(res) G##res
int x = OneModeEntry( 640x480x2) ;
#else
int x = 9;
#endif

int main() {
    if( x != 9 ) fail(__LINE__);
    _PASS;
}
