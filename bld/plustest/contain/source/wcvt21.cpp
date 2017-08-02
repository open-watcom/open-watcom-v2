#include <wcvector.h>
#include <iostream.h>


void test_find( int to_go ) {
    WCPtrOrderedVector<int> vect;
    cout << vect.first() << " ";
    if( to_go != 0 ) {
	test_find( to_go - 1 );
    }
}

int main() {
    test_find( 50 );
    cout << "\n";
    return 0;
}
