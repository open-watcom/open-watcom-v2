#include "fail.h"

#define G640x480x2   9
#define OneModeEntry(res) G##res
int x = OneModeEntry( 640x480x2) ;

int main() {
    if( x != 9 ) fail(__LINE__);
    _PASS;
}
