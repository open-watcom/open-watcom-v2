// template parsing problem
#include <stddef.h>
#include <iostream.h>

#pragma on(dump_parse);

template <size_t len, class T>
    class C {
    public:
        T val[len];
        operator T ();
    };

template <size_t len, class T>
    C<len,T>::operator T () {
        return val[0];
    }

int main () {
    return 0;
}
