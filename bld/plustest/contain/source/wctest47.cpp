#include <iostream.h>
#include "strdata.h"
#include <wclist.h>

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    cout << "There should be 6 sets of 3 (supposed to happen)\n";

    test1();
    test2();
    test3();
    test4();
    test5();
    test6();
    return 0;
}


void test1(){
    WCIsvDList<str_ddata> list1;

    if( 0 != list1.find( 0 ) ) cout << "not NULL 1\n";
    if( 0 != list1.findLast() ) cout << "not NULL  2\n";
    if( 0 != list1.get( 0 ) ) cout << "not NULL   3\n";
    if( !list1.isEmpty() ) cout << "List should be empty\n";

    list1.exceptions( WCListExcept::check_all );

    try {
	list1.find( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen 1)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list1.findLast();
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen  2)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list1.get( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen   3)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    list1.clear();
    list1.clearAndDestroy();
    cout.flush();
}
	

void test2(){
    WCIsvSList<str_sdata> list1;

    if( 0 != list1.find( 0 ) ) cout << "not NULL 1\n";
    if( 0 != list1.findLast() ) cout << "not NULL  2\n";
    if( 0 != list1.get( 0 ) ) cout << "not NULL   3\n";
    if( !list1.isEmpty() ) cout << "List should be empty\n";

    list1.exceptions( WCListExcept::check_all );

    try {
	list1.find( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen 1)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list1.findLast();
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen  2)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list1.get( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen   3)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    list1.clear();
    list1.clearAndDestroy();
    cout.flush();
}
	
void test3(){
    str_data s = str_data( "Hello" );

    WCPtrDList<str_data> list1;

    list1.find( 0 );
    list1.findLast();
    list1.get( 0 );
    if( !list1.isEmpty() ) cout << "List should be empty\n";

    list1.exceptions( WCListExcept::check_all );

    WCPtrDList<str_data> list2;

    list2.insert( &s );

    list2 = list1;
    try {
	list2.find( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen 1)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.findLast();
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen  2)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.get( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen   3)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    list1.clear();
    list1.clearAndDestroy();
    cout.flush();
}
	

void test4(){
    str_data s = str_data( "Hello" );

    WCPtrSList<str_data> list1;

    list1.find( 0 );
    list1.findLast();
    list1.get( 0 );
    if( !list1.isEmpty() ) cout << "List should be empty\n";

    list1.exceptions( WCListExcept::check_all );

    WCPtrSList<str_data> list2;

    list2.insert( &s );

    list2 = list1;
    try {
	list2.find( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen 1)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.findLast();
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen  2)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.get( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen   3)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    list1.clear();
    list1.clearAndDestroy();
    cout.flush();
}
	
void test5(){
    str_data s = str_data( "Hello" );
    str_data t;

    WCValDList<str_data> list1;

    if( t != list1.find( 0 ) ) cout << "not init 1\n";
    if( t != list1.findLast() ) cout << "not init  2\n";
    if( t != list1.get( 0 ) ) cout << "not init   3\n";
    if( !list1.isEmpty() ) cout << "List should be empty\n";

    list1.exceptions( WCListExcept::check_all );

    WCValDList<str_data> list2;

    list2.insert( s );

    list2 = list1;
    try {
	list2.find( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen 1)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.findLast();
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen  2)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.get( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen   3)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    list1.clear();
    list1.clearAndDestroy();
    cout.flush();
}
	

void test6(){
    str_data s = str_data( "Hello" );
    str_data t;

    WCValSList<str_data> list1;

    if( t != list1.find( 0 ) ) cout << "not init 1\n";
    if( t != list1.findLast() ) cout << "not init  2\n";
    if( t != list1.get( 0 ) ) cout << "not init   3\n";
    if( !list1.isEmpty() ) cout << "List should be empty\n";

    list1.exceptions( WCListExcept::check_all );

    WCValSList<str_data> list2;

    list2.insert( s );

    list2 = list1;
    try {
	list2.find( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen 1)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.findLast();
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen  2)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    try {
	list2.get( 0 );
	cout << "Should not be here 1\n";
    } catch( WCListExcept::failure oops ) {
        WCListExcept::wclist_state cause = oops.cause();
        if( cause & WCListExcept::empty_container ) {
            cout << "(supposed to happen   3)\n";
        } else {
	    cout << "wrong except\n";
	}
    }
    list1.clear();
    list1.clearAndDestroy();
    cout.flush();
}
	
