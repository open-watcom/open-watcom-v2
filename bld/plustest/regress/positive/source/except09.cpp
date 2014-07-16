#include "fail.h"
// 93/11/18 (jww) works in 10.0


// page faults (-ox -d1) (AFS)
// reported by a customer
// appears to be a problem of not popping exception-spec in all ctors/dtors
#include <stdio.h>

int ctored = 0;
int dtored = 0;

typedef int exception;

class item {
    int x;
public:
    item();
    ~item();
};

class test_class {
    item x;
public:
    test_class( const char * filename = 0 ) throw (exception);
    ~test_class();
};
  

int main(int argc, char *argv[]) {
  {
    test_class x;
  }
    int errors = 0;
    if( ! ctored ) {
	fail(__LINE__);
    }
    if( ! dtored ) {
	fail(__LINE__);
    }
    _PASS;
}

item::item() { x=0; }
item::~item() {}

test_class::test_class ( const char *filename) throw (exception) {
#if 0
    printf("Calling constructor\n");
#else
    ctored = 1;
#endif
}

test_class::~test_class() {
#if 0
    printf("Calling destructor\n");
    printf("Bye now!\n");
#else
    dtored = 1;
#endif
}
