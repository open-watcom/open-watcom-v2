// 95/09/12 -- J.W.Welch        -- rename String class

#include "fail.h"

class StringClass {
};

struct TimeOut{
    int value;
    TimeOut() : value(1) {}
};

int main()
{
    int catches;
    StringClass thing;

    catches = 0;
    for( int i=0; i<10; i++ ){
        try {
            throw TimeOut();
        } catch( TimeOut ){
            ++catches;
        } catch( ... ) {
            fail(__LINE__);
        }
    }
    if( catches != 10 ) fail(__LINE__);
    _PASS;
}
