#include <wcvector.h>
#include <iostream.h>

void test1();
void test2();
void test3();
void test4();
void test5();

int main(){
    cout << "Should get 7 supposed to happen exceptions\n";
    test1();
    test2();
    test3();
    test4();
    cout << "The next test should *NOT* cause a run time error\n";
    cout.flush();
    test5();
    return 0;
}

void test1() {

    WCValOrderedVector<long> * 	vect = new WCValOrderedVector<long>( 0 );

    vect->exceptions( WCExcept::check_all );
    try{
	vect->first();
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::empty_container ) {
            cout << "\nempty_container exception (supposed to happen)\n";
        } else {
	    cout << "wrong except1a\n";
	}
    }

    try{
	vect->last();
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::empty_container ) {
            cout << "\nempty_container exception (supposed to happen)\n";
        } else {
	    cout << "wrong except1b\n";
	}
    }

    vect->exceptions( WCExcept::all_fine );
    try {
	vect->insert( 50 );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
	    cout << "length exception (*NOT* supposed to happen)\n";
	}
    }
    
    cout << "valOrd len " << vect->entries() << "\n";

    vect->clear();
    delete( vect );
}

void test2() {

    long 	b = 50;
    WCPtrOrderedVector<long> 	vect( 0 );

    vect.exceptions( WCExcept::check_all );
    try {
	vect.insert( &b );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
	    cout << "length exception (supposed to happen)\n";
	}
    }

    try{
	vect[ 5 ] = &b;
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::empty_container ) {
            cout << "\nempty_container exception (supposed to happen)\n";
        } else {
	    cout << "wrong except2b\n";
	}
    }

    cout << "ptrOrd len " << vect.entries() << "\n";
    vect.clear();
}


void test3() {
    WCValSortedVector<long> 	vect( 0 );

    try {
	vect.insert( 10000 );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
	    cout << "length exception (*NOT* supposed to happen)\n";
	}
    }
    vect.clear();
}

void test4() {
    long	a = 25;
    long 	b = 50;
    long	c = 0;
    long 	d = 1234;
    WCPtrSortedVector<long> 	vect( 0 );

    vect.exceptions( WCExcept::check_all );
    try{
	vect.first();
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::empty_container ) {
            cout << "\nempty_container exception (supposed to happen)\n";
        } else {
	    cout << "wrong except4a\n";
	}
    }

    try{
	vect.last();
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::empty_container ) {
            cout << "\nempty_container exception (supposed to happen)\n";
        } else {
	    cout << "wrong except4b\n";
	}
    }
    vect.exceptions( WCExcept::all_fine );
    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &b );
    vect.insert( &c );
    vect.insert( &a );
    vect.exceptions( WCExcept::check_all );
    try {
	vect.insert( &b );
    } catch( WCExcept::failure oops ) {
        WCExcept::wc_state cause = oops.cause();
        if( cause & WCExcept::resize_required ) {
	    cout << "length exception (supposed to happen)\n";
	}
    }
    cout << "ptrsort len " << vect.entries() << "\n";
    vect.clear();
}

void test5() {
    WCValSortedVector<long> 	vect( 0 );
    long a;
    a = vect[ 5 ];
}
