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

void test1();
void test2();
void test3();
void test4();

int main(void) {
    test1();
    test2();
    test3();
    test4();
    cout.flush();
    return 0;
}

void test1() {
	WCIsvDList< int_ddata > list;
	WCIsvDListIter< int_ddata > iter;

	cout << "round 1\n";
        if( --iter != 0 ) cout << "-- wrong return\n";
	if( ( iter-= 1 ) != 0 ) cout << "-= wrong return\n";
	if( iter.insert( &dd[1 ] ) ) cout << "insert wrong\n";
	if( iter.append( &dd[ 1 ] ) ) cout << "append wrong\n";
	if( iter.container() != 0 ) cout << "container wrong\n";
	if( iter.current() != 0 ) cout << "current wrong\n";
	if( iter() != 0 ) cout << "() wrong\n";
	if( ++iter != 0 ) cout << "++ wrong\n";
	if( ( iter+= 1 ) != 0 ) cout << "+= wrong\n";
	cout.flush();

	iter.exceptions( WCIterExcept::check_all );

	cout << "round 2\n";
        test_except( --iter, undef_iter, "--" );
	test_except( iter-= 1, undef_iter, "-= " );
	test_except( iter.insert( &dd[1 ]), undef_iter, "insert" );
	test_except( iter.append( &dd[ 1 ] ), undef_iter, "append" );
	test_except( iter.container(), undef_iter, "container" );
	test_except( iter.current(), undef_item, "current" );
	test_except( iter(), undef_iter, "() " );
	test_except( ++iter, undef_iter, "++" );
	test_except( iter+= 1, undef_iter, "+=" );
	cout.flush();
       
        iter.reset( list );
	iter.exceptions( 0 );

	cout << "round 3\n";
        if( --iter != 0 ) cout << "-- wrong return\n";
	if( ( iter-= 1 ) != 0 ) cout << "-= wrong return\n";
	if( iter.insert( &dd[1 ] ) ) cout << "insert wrong\n";
	if( iter.append( &dd[ 1 ] ) ) cout << "append wrong\n";
	if( iter.current() != 0 ) cout << "current wrong\n";
	if( iter.container() == 0 ) cout << "container wrong\n";
	if( iter() != 0 ) cout << "() wrong\n";
	if( ++iter != 0 ) cout << "++ wrong\n";
	if( ( iter+= 1 ) != 0 ) cout << "+= wrong\n";
	cout.flush();

	cout << "round 4\n";
	iter.reset();
	iter.exceptions( WCIterExcept::check_all );
        test_except( --iter, undef_iter, "--" );
	test_except( iter-= 1, undef_iter, "-= " );
	test_except( iter.insert( &dd[1 ]), undef_iter, "insert" );
	test_except( iter.append( &dd[ 1 ] ), undef_iter, "append" );
	not_happen_except( iter.container(), "container" );
	test_except( iter.current(), undef_item, "current" );
	not_happen_except( iter(), "()" );
	test_except( iter(), undef_iter, "() " );
	test_except( ++iter, undef_iter, "++" );
	test_except( iter+= 1, undef_iter, "+=" );
	cout.flush();
       
	list.insert( &dd[ 1 ] );
	list.insert( &dd[ 2 ] );

	cout << "round 5\n";
	iter.reset();
	if( iter() == 0 ) cout << "() wrong\n";
	if( ++iter == 0 ) cout << "++ wrong\n";
        if( --iter == 0 ) cout << "-- wrong return\n";
	if( (iter+= 1) == 0 ) cout << "+= wrong\n";
	if( (iter-= 1) == 0 ) cout << "-= wrong return\n";
	if( !iter.insert( &dd[3 ] ) ) cout << "insert wrong\n";
	if( !iter.append( &dd[ 4 ] ) ) cout << "append wrong\n";
	if( iter.container() == 0 ) cout << "container wrong\n";
	if( iter.current() == 0 ) cout << "current wrong\n";
	cout.flush();

	list.get();
	list.get();
	list.get();
	list.get();

	list.append( &dd[ 1 ] );
	list.append( &dd[ 2 ] );

	iter.reset();
	
	cout << "round 6\n";
	iter.exceptions( 0 );

        if( --iter != 0 ) cout << "-- wrong return\n";
	if( (iter-= 1) != 0 ) cout << "-= wrong return\n";
	if( iter.insert( &dd[1 ] ) ) cout << "insert wrong\n";
	if( iter.append( &dd[ 1 ] ) ) cout << "append wrong\n";

	iter.exceptions( WCIterExcept::check_all );
	iter.reset();
	if( iter() != &dd[ 1 ] ) cout << "() wrong\n";
	if( iter() != &dd[ 2 ] ) cout << "() wrong\n";
	if( iter() != 0 ) cout << "() wrong\n";
	if( --iter != &dd[ 2 ] ) cout << "-- wrong\n";
	if( --iter != &dd[ 1 ] ) cout << "-- wrong\n";
	if( --iter != 0 ) cout << "-- wrong\n";
	if( ++iter != &dd[ 1 ] ) cout << "++ wrong\n";
	if( ++iter != &dd[ 2 ] ) cout << "++ wrong\n";
	if( ++iter != 0 ) cout << "++ wrong\n";
	if( (iter -= 1) != &dd[ 2 ] ) cout << "-- wrong\n";
	if( (iter -= 1) != &dd[ 1 ] ) cout << "-- wrong\n";
	if( (iter -= 1) != 0 ) cout << "-- wrong\n";
	if( (iter+= 1) == 0 ) cout << "+= wrong\n";
	if( (iter+= 1) == 0 ) cout << "+= wrong\n";
	test_except( iter += 2, iter_range, "+= 2" );
	if( --iter != &dd[ 2 ] ) cout << "-- wrong\n";
	test_except( iter += 3, iter_range, "+= 3" );
	if( --iter != &dd[ 2 ] ) cout << "-- wrong\n";
	if( --iter != &dd[ 1 ] ) cout << "-- wrong\n";
	test_except( iter -= 2, iter_range, "-= 2" );
	if( ++iter != &dd[ 1 ] ) cout << "++ wrong\n";
	test_except( iter -= 3, iter_range, "-= 3" );

	iter.exceptions( 0 );
	if( iter.current() != 0 ) cout << "current wrong\n";
	cout.flush();
	
	list.clear();
}


void test2() {
	WCPtrDList< int > list;
	WCPtrDListIter< int > iter;
	int i = 1;
	int j = 2;

	cout << "test2: round 1\n";
        if( --iter != 0 ) cout << "-- wrong return\n";
	if( ( iter-= 1 ) != 0 ) cout << "-= wrong return\n";
	if( iter.insert( &i ) ) cout << "insert wrong\n";
	if( iter.append( &i ) ) cout << "append wrong\n";
	if( iter.container() != 0 ) cout << "container wrong\n";
	iter.current();
	if( iter() != 0 ) cout << "() wrong\n";
	if( ++iter != 0 ) cout << "++ wrong\n";
	if( ( iter+= 1 ) != 0 ) cout << "+= wrong\n";
	cout.flush();

	iter.exceptions( WCIterExcept::check_all );

	cout << "round 2\n";
        test_except( --iter, undef_iter, "--" );
	test_except( iter-= 1, undef_iter, "-= " );
	test_except( iter.insert( &i ), undef_iter, "insert" );
	test_except( iter.append( &i ), undef_iter, "append" );
	test_except( iter.container(), undef_iter, "container" );
	test_except( iter.current(), undef_item, "current" );
	test_except( iter(), undef_iter, "() " );
	test_except( ++iter, undef_iter, "++" );
	test_except( iter+= 1, undef_iter, "+=" );
	cout.flush();
       
        iter.reset( list );
	iter.exceptions( 0 );

	cout << "round 3\n";
        if( --iter != 0 ) cout << "-- wrong return\n";
	if( ( iter-= 1 ) != 0 ) cout << "-= wrong return\n";
	if( iter.insert( &i ) ) cout << "insert wrong\n";
	if( iter.append( &i ) ) cout << "append wrong\n";
	iter.current();
	if( iter.container() == 0 ) cout << "container wrong\n";
	if( iter() != 0 ) cout << "() wrong\n";
	if( ++iter != 0 ) cout << "++ wrong\n";
	if( ( iter+= 1 ) != 0 ) cout << "+= wrong\n";
	cout.flush();

	cout << "round 4\n";
	iter.reset();
	iter.exceptions( WCIterExcept::check_all );
        test_except( --iter, undef_iter, "--" );
	test_except( iter-= 1, undef_iter, "-= " );
	test_except( iter.insert( &i ), undef_iter, "insert" );
	test_except( iter.append( &i ), undef_iter, "append" );
	not_happen_except( iter.container(), "container" );
	test_except( iter.current(), undef_item, "current" );
	not_happen_except( iter(), "()" );
	test_except( iter(), undef_iter, "() " );
	test_except( ++iter, undef_iter, "++" );
	test_except( iter+= 1, undef_iter, "+=" );
	cout.flush();
       
	list.insert( &i );
	list.insert( &j );

	cout << "round 5\n";
	iter.reset();
	if( iter() == 0 ) cout << "() wrong\n";
	if( ++iter == 0 ) cout << "++ wrong\n";
        if( --iter == 0 ) cout << "-- wrong return\n";
	if( (iter+= 1) == 0 ) cout << "+= wrong\n";
	if( (iter-= 1) == 0 ) cout << "-= wrong return\n";
	if( !iter.insert( &i ) ) cout << "insert wrong\n";
	if( !iter.append( &j ) ) cout << "append wrong\n";
	if( iter.container() == 0 ) cout << "container wrong\n";
	if( iter.current() == 0 ) cout << "current wrong\n";
	cout.flush();

	list.get();
	list.get();
	list.get();
	list.get();

	list.append( &i );
	list.append( &j );

	iter.reset();
	
	cout << "round 6\n";
	iter.exceptions( 0 );

        if( --iter != 0 ) cout << "-- wrong return a\n";
	if( (iter-= 1) != 0 ) cout << "-= wrong return b\n";
	if( iter.insert( &i ) ) cout << "insert wrong c\n";
	if( iter.append( &i ) ) cout << "append wrong d\n";

	iter.exceptions( WCIterExcept::check_all );
	iter.reset();
	if( iter() == 0 ) cout << "() wrong e\n";
	if( iter.current() != &i ) cout << "() wrong f\n";
	if( iter() == 0 ) cout << "() wrong g\n";
	if( iter.current() != &j ) cout << "() wrong h\n";
	if( iter() != 0 ) cout << "() wrong i\n";
	if( --iter == 0 ) cout << "-- wrong j\n";
	if( iter.current() != &j ) cout << "-- wrong j\n";
	if( --iter == 0 ) cout << "-- wrong l\n";
	if( iter.current() != &i ) cout << "-- wrong m\n";
	if( --iter != 0 ) cout << "-- wrong n\n";
	if( ++iter == 0 ) cout << "++ wrong o\n";
	if( iter.current() != &i ) cout << "++ wrong p\n";
	if( ++iter == 0 ) cout << "++ wrong q\n";
	if( iter.current() != &j ) cout << "++ wrong r\n";
	if( ++iter != 0 ) cout << "++ wrong s\n";
	if( (iter -= 1) == 0 ) cout << "-- wrong t\n";
	if( iter.current() != &j ) cout << "-- wrong u\n";
	if( (iter -= 1) == 0 ) cout << "-- wrong v\n";
	if( iter.current() != &i ) cout << "-- wrong w\n";
	if( (iter -= 1) != 0 ) cout << "-- wrong x\n";
	if( (iter+= 1) == 0 ) cout << "+= wrong y\n";
	if( (iter+= 1) == 0 ) cout << "+= wrong z\n";
	test_except( iter += 2, iter_range, "+= 2  A" );
	if( --iter == 0 ) cout << "-- wrong B\n";
	if( iter.current() != &j ) cout << "-- wrong C\n";
	test_except( iter += 3, iter_range, "+= 3 D" );
	if( --iter == 0 ) cout << "+=3wrong E\n";
	if( iter.current() != &j ) cout << "+=3wrong F\n";
	if( --iter == 0 ) cout << "-- wrong G\n";
	if( iter.current() != &i ) cout << "-- wrong H\n";
	test_except( iter -= 2, iter_range, "-= 2 I" );
	if( ++iter == 0 ) cout << "-=2 wrong J\n";
	if( iter.current() != &i ) cout << "-=2 wrong K\n";
	test_except( iter -= 3, iter_range, "-= 3 L" );
	if( ++iter == 0 ) cout << "-=3 wrong M\n";
	if( iter.current() != &i ) cout << "-=3 wrong N\n";

	iter.exceptions( 0 );
	iter.current();
	cout.flush();
	
	list.clear();
}

void test3() {
    WCValSList< str_data > list;
    WCValConstSListIter< str_data > iter;
    str_data i = "1";
    str_data j = "2";
    str_data default_obj;
    const WCValSList<str_data> &const_list = list;

    cout << "test ValSlist\n";
    if( ++iter != 0 ) cout << "++ should have failed\n";
    if( iter() != 0 ) cout << "() should have failed\n";
    if( iter.container() != 0 ) cout << "container != 0\n";
    if( (iter+= 1) != 0 ) cout << "+= 1 should have failed\n";
    if( iter.current() != default_obj ) cout << "current should have failed\n";

    list.append( i );
    list.append( j );

    iter.reset( const_list );
    if( ++iter == 0 ) cout << "++ failed\n";
    if( iter() == 0 ) cout << "() failed\n";
    if( iter.current() != j ) cout << "current failed\n";
    if( ( iter += 1 ) != 0 ) cout << "+= should have failed\n";

    iter.reset();

    WCValConstSListIter< str_data > iter2( const_list );
    iter2 += 1;
    if( iter2.current() != i ) cout << "current failed\n";
}


void test4() {
    WCValDList< str_data > list;
    WCValConstDListIter< str_data > iter;
    str_data i = "1";
    str_data j = "2";
    str_data default_obj;
    const WCValDList< str_data > &const_list = list;

    cout << "test ValDlist\n";
    if( ++iter != 0 ) cout << "++ should have failed\n";
    if( iter() != 0 ) cout << "() should have failed\n";
    if( iter.container() != 0 ) cout << "container != 0\n";
    if( (iter+= 1) != 0 ) cout << "+= 1 should have failed\n";
    if( --iter != 0 ) cout << "-- should have failed\n";
    if( ( iter -= 1 ) != 0 ) cout << "-= should have failed\n";
    if( iter.current() != default_obj ) cout << "current should have failed\n";

    list.append( i );
    list.append( j );

    iter.reset( const_list );
    if( ++iter == 0 ) cout << "++ failed\n";
    if( iter() == 0 ) cout << "() failed\n";
    if( ( iter += 1 ) != 0 ) cout << "+= should have failed\n";
    if( --iter == 0 ) cout << "-- failed\n";
    if( ( iter -= 1 ) == 0 ) cout << "-1 failed\n";
    if( iter.current() != i ) cout << "current failed\n";

    iter.reset();

    WCValConstDListIter< str_data > iter2( const_list );
    iter2 += 1;
    if( iter2.current() != i ) cout << "current failed\n";
}
