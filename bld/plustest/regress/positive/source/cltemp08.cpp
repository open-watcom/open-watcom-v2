#include "fail.h"

template<class T> struct S;

template<>
struct S<char> {
    friend struct S<int>;
};

template<>
struct S<int> {
};

template<>
struct S<short>;

template<>
struct S<short> {
};



struct A;

template< typename T, int N > struct B;
typedef B< A, 1 > C;

template< typename T, int N >
struct B
{
  static int *ptr1;
  static int *ptr2;
  static int *ptr3;
  static int *ptr4;
};

int *C::ptr1 = 0;

template< >
int *C::ptr2 = 0;

int *B< A, 1 >::ptr3 = 0;

template< >
int *B< A, 1 >::ptr4 = 0;



int main() {
    S<char> s1;
    S<int> s2;
    S<short> s3;


    if( C::ptr1 != 0 ) fail( __LINE__ );
    if( C::ptr2 != 0 ) fail( __LINE__ );
    if( B< A, 1 >::ptr3 != 0 ) fail( __LINE__ );
    if( B< A, 1 >::ptr4 != 0 ) fail( __LINE__ );

    _PASS;
}
