#include "fail.h"

#define ppn1 0xae+0x51

unsigned x = ppn1;	// should evaluate to 0xff

int main() {
    if( x != 0xff ) fail(__LINE__);
    if( (ppn1) != 0xff ) fail(__LINE__);
    _PASS;
}
