#include "fail.h"

int count;

struct T {
    char a[7];
    T() {++count;};
};

struct Q {
    int x;
    T a[10];
    int y;
    T b[10];
    int z;
    T c[10];
};

Q x;

int main()
{
    if( count != 30 ) fail(__LINE__);
    _PASS;
}
