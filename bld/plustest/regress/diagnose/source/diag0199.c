// Test returning undefined type

struct UNDEF;

UNDEF foo1();
UNDEF& foo2();
const UNDEF foo3();
const UNDEF& foo4();
UNDEF* foo5();
const UNDEF* foo6();

void poo()
{
    foo1(); // error
    foo2();
    foo3(); // error
    foo4();
    foo5();
    foo6();
}
