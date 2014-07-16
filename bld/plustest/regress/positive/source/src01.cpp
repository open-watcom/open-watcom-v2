#include "fail.h"

int a[5] = {
#include "src01.h"
#include "src01.h"
#include "src01.h"
#include "src01.h"
};

int main() {
    if( (sizeof(a)/sizeof(a[0])) != 5 ) fail(__LINE__);
    if( a[0] != 1 ) fail(__LINE__);
    if( a[1] != 1 ) fail(__LINE__);
    if( a[2] != 1 ) fail(__LINE__);
    if( a[3] != 1 ) fail(__LINE__);
    if( a[4] != 0 ) fail(__LINE__);
    _PASS;
}
