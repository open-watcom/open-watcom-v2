#include <wcskip.h>
#include <iostream.h>

void print_val( int s, void * ) {
    cout << s << " ";
}

void print_ptr( int *s, void * ) {
    cout << *s << " ";
}

void test1();
void test2();

int main() {
    test1();
    cout << "\n\n";
    test2();
    cout.flush();
    return 0;
};


void test1() {
    WCValSkipList<int> skip_list;
    int temp;

    for( int i = 0; i < 20; i++ ) {
	temp = 0;
	skip_list.insert( temp );
	if( skip_list.occurrencesOf( temp ) != i + 1 ) cout << i << "wrong\n";
    };
    temp = -1;
    skip_list.insert( temp );
    temp = 1;
    skip_list.insert( temp );

    skip_list.forAll( print_val, 0 );
    cout << "\nremoving 1 and 0's\n";
    skip_list.removeAll( temp );
    temp = 0;
    skip_list.removeAll( temp );
    skip_list.forAll( print_val, 0 );
}

void test2() {
    WCPtrSkipList<int> skip_list( WCSKIPLIST_PROB_HALF, 4 );
    int array[ 22 ];

    for( int i = 0; i < 20; i++ ) {
	array[ i ] = 0;
	skip_list.insert( &array[ i ] );
	if( skip_list.occurrencesOf( &array[ i ] ) != i + 1 ) cout << i << "wrong\n";
    };
    array[ 20 ] = -1;
    skip_list.insert( &array[ 20 ] );
    array[ 21 ] = 1;
    skip_list.insert( &array[ 21 ] );

    skip_list.forAll( print_ptr, 0 );
    cout << "\nremoving 1 and 0's\n";
    skip_list.removeAll( &array[ 21 ] );
    skip_list.removeAll( &array[ 0 ] );
    skip_list.forAll( print_ptr, 0 );
}
