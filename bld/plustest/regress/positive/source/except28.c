#include "fail.h"
#include <string.h>

int count;
char *check_msg = "CCCC";

struct A {
    virtual char *msg() const = 0;
protected:
    A() : sig(++count) {
    }
public:
    A( A const &s ) : sig(s.sig) {
    }
private:
    int sig;
};
struct C : A {
    char *msg() const {
	return check_msg;
    }
};

void test() {
    throw C();
}

int main() {
    try {
	test();
    } catch( A const &r ) {
	if( r.msg() != check_msg ) fail(__LINE__);
	if( strcmp( r.msg(), "CCCC" ) != 0 ) fail(__LINE__);
    } catch( ... ) {
	fail(__LINE__);
    }
    _PASS;
}
