#include "dump.h"

struct B
{
    char* p;
    B(char* q) : p(q) { GOOD; }
    B(const B & r) : p(r.p) {GOOD; }
};


int main()
{
    B b = 0;
    CHECK_GOOD(6);
    return errors != 0;
}
