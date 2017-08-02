#include "fail.h"

namespace ns {
template<class T>
struct A {
    T member;
};
}

using ns::A;

int main()
{
    A<int> a;
    a.member = 1;

    _PASS;
}
