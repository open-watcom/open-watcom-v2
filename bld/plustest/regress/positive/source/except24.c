// PT166095
#include "fail.h"

class xErr {};

int ctored;

struct CD {
    CD();
    CD(CD const & );
    ~CD();
    void throwException();
    int sig;
};


CD::CD() : sig(++ctored) {
    //printf("%p %d -- constructor\n", this, sig);
}
CD::CD(CD const &s) : sig(++ctored) {
    //printf("%p %d -- copy constructor\n", this, sig);
}
CD::~CD() {
    //printf("%p %d -- destructor\n", this, sig);
    if( sig < 0 ) fail(__LINE__);
    sig = -1;
}
void CD::throwException() {
    throw xErr();
}


class X {
public:
        X(int i);
        ~X() {};
private:
        int _1;
        int _2;
};

X::X(int i) : _1(i), _2(0){
        return;
};



void test() {
    CD oTest1;
    X oX(5);

    try {
	//printf("-------In try\n");
	oTest1.throwException();
    } catch(xErr) {
	//printf("-------In catch\n");
    };
    //printf("-------Leaving main\n");
    return;
};

int main() {
    test();
    _PASS;
}
