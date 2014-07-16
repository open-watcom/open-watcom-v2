#include "fail.h"

template<class T>
    struct CC {
	T i;
	CC(T x) : i(x) {}
    };

class CC<int> xx(-1);

int main()
{
    if( xx.i != -1 ) fail(__LINE__);
    _PASS;
}
