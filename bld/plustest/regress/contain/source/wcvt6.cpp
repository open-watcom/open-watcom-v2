// test out_of_memory exception where possible

#include <wcvector.h>
#include <iostream.h>
#include <malloc.h>

extern int memchk_fail;

void test1();
void test2();

int main(){
    cout << "There should be 4 supposed to happen exceptions\n";
    test1();
    test2();
    cout.flush();
    return 0;
}

void test1(){
    WCValVector<long> 	vect( 0, 0 );

    vect[ 4 ] = 5;
    memchk_fail = 1;
    vect[ 9 ] = 5;
    vect.resize( 9 );
    vect.exceptions( WCExcept::check_all );

    try{
	vect[ 9 ] = 5;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nresize_required exception (supposed to happen)\n";
        } else {
	    cout << "wrong except1\n";
	}
    }

    try{
	vect.resize( 9 );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::out_of_memory ) {
            cout << "\nout of mem exception (supposed to happen)\n";
        } else {
	    cout << "wrong except2\n";
	}
    }

    cout << "Vector length" << vect.length() << "\n";
    vect.clear();
}


void test2(){
    WCValOrderedVector<long> 	vect( 0, 2 );

    memchk_fail = 0;
    vect.insert( 255 );
    vect.insert( 75 );

    memchk_fail = 1;
    vect[ 9 ] = 5;
    vect.resize( 9 );
    vect.exceptions( WCExcept::check_all );

    memchk_fail = 0;
    try{
	vect.insert( 5 );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
            cout << "\nvector_length range exception (supposed to happen)\n";
        } else {
	    cout << "wrong except3\n";
	}
    }

    memchk_fail = 1;
    try{
	vect.resize( 9 );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::out_of_memory ) {
            cout << "\nout_of_mem exception (supposed to happen)\n";
        } else {
	    cout << "wrong except4\n";
	}
    }
    cout << "Vector length" << vect.entries() << "\n";
    vect.clear();
}
