#include "fail.h"

struct WObject {
    virtual ~WObject() {}
private:
    long _verificationFlag;
};

void foo( const WObject verb = WObject( ) ) {};

int main()
{
    foo();
    _PASS;
}
