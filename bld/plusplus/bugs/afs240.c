// doesn't parse
#include <iostream.h>

class C {
    int val; public:
    typedef double my_type;
    C (int i) : val (i) {}
    my_type change () {return val;}
};

/*
    Currently, the return type cannot be dependent on the
    argument generic type since we "snap" the true type
    of the function (once we bind the generic types)
    directly from the function template type; we would
    have to represent the generic '::' chain in the type system
    using the current methodology
*/
template <class T>
    T::my_type funct (T x) {
        return x.change();
    }

int main () {
    C c (3);
    cout << funct (c) << endl;
    return 0;
}
