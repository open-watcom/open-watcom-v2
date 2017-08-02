#include <wclistit.h>
#include <iostream.h>
#include <strdata.h>

#define test_except( code, except, string )			\
    try{							\
	code;							\
	cout << string << " exception not thrown when supposed to\n";	\
    } catch( WCIterExcept::failure oops ) {			\
        WCIterExcept::wciter_state cause = oops.cause();	\
        if( ! ( cause & WCIterExcept::except ) ) {		\
	    cout << "wrong except\n";				\
	}							\
    }

#define not_happen_except( code, string )			\
    try{							\
	code;							\
    } catch( WCIterExcept::failure oops ) {			\
        WCIterExcept::wciter_state cause = oops.cause();	\
        if( cause & WCIterExcept::check_all ) {			\
            cout << string << " (NOT supposed to happen)\n";	\
	}							\
    }

class int_ddata : public WCDLink {
public:
	inline int_ddata() : info( 0 ) {};
	inline int_ddata(int datum) : info (datum) {};

	int info;
};

#define NENTR	10

int_ddata	 dd[NENTR];
int_ddata		*d1;
int_ddata		dapp, dapp2, dapp3;

void test5();
void test6();
void test7();
void test8();

int main(void) {
    test5();
    test6();
    test7();
    test8();
    cout.flush();
    return 0;
}



void test5() {
    WCPtrSList< str_data > list;
    WCPtrConstSListIter< str_data > iter;
    str_data i = "1";
    str_data j = "2";
    const WCPtrSList<str_data> &const_list = list;

    cout << "test PtrSlist\n";
    if( ++iter != 0 ) cout << "++ should have failed\n";
    if( iter() != 0 ) cout << "() should have failed\n";
    if( iter.container() != 0 ) cout << "container != 0\n";
    if( (iter+= 1) != 0 ) cout << "+= 1 should have failed\n";
    iter.current();

    list.append( &i );
    list.append( &j );

    iter.reset( const_list );
    if( ++iter == 0 ) cout << "++ failed\n";
    if( iter() == 0 ) cout << "() failed\n";
    if( iter.current() != &j ) cout << "current failed\n";
    if( ( iter += 1 ) != 0 ) cout << "+= should have failed\n";

    iter.reset();

    WCPtrConstSListIter< str_data > iter2( const_list );
    iter2 += 1;
    if( iter2.current() != &i ) cout << "current failed\n";
}


void test6() {
    WCPtrDList< str_data > list;
    WCPtrConstDListIter< str_data > iter;
    str_data i = "1";
    str_data j = "2";
    const WCPtrDList< str_data > &const_list = list;

    cout << "test PtrDlist\n";
    if( ++iter != 0 ) cout << "++ should have failed\n";
    if( iter() != 0 ) cout << "() should have failed\n";
    if( iter.container() != 0 ) cout << "container != 0\n";
    if( (iter+= 1) != 0 ) cout << "+= 1 should have failed\n";
    if( --iter != 0 ) cout << "-- should have failed\n";
    if( ( iter -= 1 ) != 0 ) cout << "-= should have failed\n";
    iter.current();

    list.append( &i );
    list.append( &j );

    iter.reset( const_list );
    if( ++iter == 0 ) cout << "++ failed\n";
    if( iter() == 0 ) cout << "() failed\n";
    if( ( iter += 1 ) != 0 ) cout << "+= should have failed\n";
    if( --iter == 0 ) cout << "-- failed\n";
    if( ( iter -= 1 ) == 0 ) cout << "-1 failed\n";
    if( iter.current() != &i ) cout << "current failed\n";

    iter.reset();

    WCPtrConstDListIter< str_data > iter2( const_list );
    iter2 += 1;
    if( iter2.current() != &i ) cout << "current failed\n";
}

void test7() {
    WCIsvSList< str_sdata > list;
    WCIsvConstSListIter< str_sdata > iter;
    str_sdata i = "1";
    str_sdata j = "2";
    const WCIsvSList<str_sdata> &const_list = list;

    cout << "test IsvSlist\n";
    if( ++iter != 0 ) cout << "++ should have failed\n";
    if( iter() != 0 ) cout << "() should have failed\n";
    if( iter.container() != 0 ) cout << "container != 0\n";
    if( (iter+= 1) != 0 ) cout << "+= 1 should have failed\n";
    if( iter.current() != 0 ) cout << "current failed\n";

    list.append( &i );
    list.append( &j );

    iter.reset( const_list );
    if( ++iter == 0 ) cout << "++ failed\n";
    if( iter() == 0 ) cout << "() failed\n";
    if( iter.current() != &j ) cout << "current failed\n";
    if( ( iter += 1 ) != 0 ) cout << "+= should have failed\n";

    iter.reset();

    WCIsvConstSListIter< str_sdata > iter2( const_list );
    iter2 += 1;
    if( iter2.current() != &i ) cout << "current failed\n";
}


void test8() {
    WCIsvDList< str_ddata > list;
    WCIsvConstDListIter< str_ddata > iter;
    str_ddata i = "1";
    str_ddata j = "2";
    const WCIsvDList< str_ddata > &const_list = list;

    cout << "test IsvDlist\n";
    if( ++iter != 0 ) cout << "++ should have failed\n";
    if( iter() != 0 ) cout << "() should have failed\n";
    if( iter.container() != 0 ) cout << "container != 0\n";
    if( (iter+= 1) != 0 ) cout << "+= 1 should have failed\n";
    if( --iter != 0 ) cout << "-- should have failed\n";
    if( ( iter -= 1 ) != 0 ) cout << "-= should have failed\n";
    if( iter.current() != 0 ) cout << "current failed\n";

    list.append( &i );
    list.append( &j );

    iter.reset( const_list );
    if( ++iter == 0 ) cout << "++ failed\n";
    if( iter() == 0 ) cout << "() failed\n";
    if( ( iter += 1 ) != 0 ) cout << "+= should have failed\n";
    if( --iter == 0 ) cout << "-- failed\n";
    if( ( iter -= 1 ) == 0 ) cout << "-1 failed\n";
    if( iter.current() != &i ) cout << "current failed\n";

    iter.reset();

    WCIsvConstDListIter< str_ddata > iter2( const_list );
    iter2 += 1;
    if( iter2.current() != &i ) cout << "current failed\n";
}
