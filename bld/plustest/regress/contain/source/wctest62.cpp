#include <wclist.h>
#include <iostream.h>

struct two_ints {
    int i1;
    int i2;
};

unsigned errors;

void fail( unsigned line )
{
    cout << "failure on line " << line << endl;
    ++errors;
}
int o=1;

int main() {
    if( o && sizeof( WCSLink ) != sizeof( void * ) ) fail(__LINE__);
    if( o && sizeof( WCDLink ) != 2*sizeof( void *) ) fail(__LINE__);
    if( o && WCValSListItemSize(two_ints) != sizeof(two_ints)+sizeof(WCSLink) ) fail(__LINE__);
    if( o && WCValDListItemSize(two_ints) != sizeof(two_ints)+sizeof(WCDLink) ) fail(__LINE__);
    if( o && WCPtrSListItemSize(two_ints) != sizeof(void*)*2 ) fail(__LINE__);
    if( o && WCPtrDListItemSize(two_ints) != sizeof(void*)*3 ) fail(__LINE__);
    if( !errors ) cout << "PASS " << __FILE__ << endl;
    return errors != 0;
};
