// test Ordered insert, append, insertAt, prepend
#include <iostream.h>
#include <wcvector.h>
#include <string.hpp>

void test1();
void test2();

int main(){
    test1();
    test2();
    cout.flush();
    return 0;
}

void test1(){
    WCValOrderedVector< String > vect;

    cout << "Inserting one\n";
    vect.insert( "one" );
    cout << "Prepending two\n";
    vect.prepend( "two" );
    cout << "Appending three\n";
    vect.append( "three" );
    cout << "Inserting four at 1\n";
    vect.insertAt( 1, "four" );
    cout << "prepending five\n";
    vect.prepend( "five" );
    cout << "Inserting six\n";
    vect.insert( "six" );

    for( int i = 0; i < vect.entries(); i++ ){
	cout << vect[ i ] << ' ';
    }
    cout << "\n";
}

    
void test2(){
    WCPtrOrderedVector< String > vect;
    String one = "one";
    String two = "two";
    String three = "three";
    String four = "four";
    String five = "five";
    String six = "six";

    cout << "Inserting one\n";
    vect.insert( &one );
    cout << "Prepending two\n";
    vect.prepend( &two );
    cout << "Appending three\n";
    vect.append( &three );
    cout << "Inserting four at 1\n";
    vect.insertAt( 1, &four );
    cout << "prepending five\n";
    vect.prepend( &five );
    cout << "Inserting six\n";
    vect.insert( &six );

    for( int i = 0; i < vect.entries(); i++ ){
	cout << *vect[ i ] << ' ';
    }
    cout << "\n";
}
    
