#include "fail.h"

struct CD {
    static int counter;

    unsigned nsig;
    CD *me;
    unsigned sig;

    void connect_up ();
    CD();
    CD(const CD &);
    void operator =(const CD &);
    ~CD();
};
int CD::counter;

void CD::connect_up () {
    ++counter;
    sig = counter;
    nsig = -counter;
    me = this;
}

CD::CD () {
//  printf( "CD(%p)\n", this );
    connect_up();
}

CD::CD (const CD &s ) {
//  printf( "CD(%p,%p)\n", this, &s );
    connect_up();
}

void CD::operator = (const CD &src) {
//  printf( "op=(%p,%p)\n", this, &src );
}

CD::~CD () {
//  printf( "~CD(%p)\n", this );
    if( counter == 0 ) fail(__LINE__);
    --counter;
    if( sig != -nsig ) fail(__LINE__);
    if( this != me ) fail(__LINE__);
    me = NULL;
}

struct B : CD {
    int v;
    B(int);
};

B::B(int x) : v(!!x) {
}

int ne( B l, B r, B, B ) {
    return l.v != r.v;
}

int i( int i ) {
    return i;
}

int test_and_0() {
    if( !i(1) && ne( B(1), B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_and_1() {
    if( i(0) && ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_and_2() {
    if( i(1) && ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_and_3() {
    if( i(1) && ! ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_and_4() {
    if( !i(0) && ! ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_and_5() {
    if( !i(0) && ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_or_0() {
    if( i(1) || ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_or_1() {
    if( i(0) || ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_or_2() {
    if( i(0) || !( ne( B(1) , B(0), B(0), B(0) ) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_or_3() {
    if( !i(1) || ne( B(1) , B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_or_4() {
    if( !i(1) || !( ne( B(1) , B(0), B(0), B(0) ) ) ) {
        return 1;
    } else {
        return 0;
    }
}

int test_quest_0() {
    if( i(1) ? ! ne( B(1) , B(0), B(0), B(0) ) : ! ne( B(1), B(0), B(0), B(0) ) ) {
        return 1;
    } else {
        return 0;
    }
}

void use( int *a ) {
    a[0] = -1;
    a[1] = -1;
    a[2] = -1;
    a[3] = -1;
    a[4] = -1;
    a[5] = -1;
    a[6] = -1;
    a[7] = -1;
}

void zap() {
    int a[8];
    a[0] = 0;
    a[1] = 0;
    a[2] = 0;
    a[3] = 0;
    a[4] = 0;
    a[5] = 0;
    a[6] = 0;
    a[7] = 0;
    use( a );
}

int main () {
    zap();
    if( test_quest_0() != 0 ) fail(__LINE__);
    zap();
    if( test_and_0() != 0 ) fail(__LINE__);
    zap();
    if( test_and_1() != 0 ) fail(__LINE__);
    zap();
    if( test_and_2() != 1 ) fail(__LINE__);
    zap();
    if( test_and_3() != 0 ) fail(__LINE__);
    zap();
    if( test_and_4() != 0 ) fail(__LINE__);
    zap();
    if( test_and_5() != 1 ) fail(__LINE__);
    zap();
    if( test_or_0() != 1 ) fail(__LINE__);
    zap();
    if( test_or_1() != 1 ) fail(__LINE__);
    zap();
    if( test_or_2() != 0 ) fail(__LINE__);
    zap();
    if( test_or_3() != 1 ) fail(__LINE__);
    zap();
    if( test_or_4() != 0 ) fail(__LINE__);
    if( CD::counter != 0 ) fail(__LINE__);
    _PASS;
}
