#include "fail.h"

#define RPLOW(reg)              (reg & ~0x40)
#define ER_LOW(reg)             (reg & ~0x80 | 0x40)

unsigned short a;

void test()
{
    a = RPLOW(ER_LOW(a));
}

int main() {
    _PASS;
}
