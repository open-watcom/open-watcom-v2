#include <wcvector.h>
#include <stdlib.h>
#include <iostream.h>

void test1();
void test2();
void test3();
void test4();

int main(){
    cout << "There should be 4 supposed to happen exceptions\n";
    test1();
    test2();
    test3();
    test4();
    cout.flush();
    return 0;
}

void test1(){
    int i;

    WCValVector<long> 	vect( 5, 0 );

    try{
	vect[ -1 ] = 25;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::index_range ) {
            cout << "\n** index range exception ** (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
	}
    }
    try{
	vect[ 6 ] = 100;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::index_range ) {
            cout << "\n** index range exception ** (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }
    vect[ 5 ] = 9999999;
    vect.exceptions( WCExcept::check_all );
    try{
	vect[ 7 ] = 25;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nresize_required exception (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }

    for( i = 0; i < vect.length(); i++ ){
	cout << vect[ i ] << ' ';
    }
    cout << "\n";
    vect.clear();
}

void test2(){
    WCValVector< int >	intvect;
    intvect.exceptions( WCExcept::check_all );

    try{
	intvect[ 5 ] = 25;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nresize_required exception (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }
    intvect.clear();
}


void test3(){
    int i;

    long a = 0;
    long b = 25;
    long c = 100;
    WCPtrVector<long> 	vect( 5, &a );
    vect.exceptions( WCExcept::check_all );

    try{
	vect[ -1 ] = &b;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::index_range ) {
            cout << "\n** index range exception ** (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }
    try{
	vect[ 5 ] = &c;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nresize_required exception (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }
    vect.exceptions( WCExcept::check_all );
    try{
	vect[ 6 ] = &b;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nresize_required exception (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }

    for( i = 0; i < vect.length(); i++ ){
	cout << *vect[ i ] << ' ';
    }
    cout << "\n";
    vect.clear();
}


void test4(){
    WCPtrVector< int >	intvect( -5 );
    int	a = 25;

    intvect.exceptions( WCExcept::check_all );
    try{
	intvect[ 0 ] = &a;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nresize_required exception (supposed to happen)\n";
        } else {
	    cout << "wrong except!!\n";
        }
    }
    intvect.clear();
}
