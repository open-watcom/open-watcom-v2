#include "fail.h"

struct A {
    A() {
        if( ctor_counter == except_trigger ) throw 0;
	ctor_counter++;
	magic = 0xbeefcafeUL;
    }

    ~A() {
        if( magic != 0xbeefcafeUL ) fail( __LINE__ );
	magic = 0xafafafafUL;
        dtor_counter++;
    }

    unsigned long magic;

    static void reset( const int new_except_trigger ) {
        except_trigger = new_except_trigger;
	ctor_counter = dtor_counter = 0;
    }

    static void check( const int counter, const int line ) {
      if( ( A::ctor_counter != counter )
       || ( A::dtor_counter != counter ) ) {
	  fail( line );
      }
    }

    static int except_trigger;
    static int ctor_counter;
    static int dtor_counter;
};

int A::except_trigger = -1;
int A::ctor_counter = 0;
int A::dtor_counter = 0;

struct B {
    A a1, a2, a3, a4;
    A aa[5];
    A a5;
};


int main() {
  A::reset( -1 );
  try {
      A a[10];
  } catch (const int &) {
      fail( __LINE__ );
  }
  A::check( 10, __LINE__ );

  A::reset( -1 );
  try {
      A a[10] = { A(), A(), A() };
  } catch (const int &) {
      fail( __LINE__ );
  }
  A::check( 10, __LINE__ );

  A::reset( 2 );
  try {
      A a[10] = { A(), A(), A() };
      fail( __LINE__ );
  } catch (const int &) {
  }
  A::check( 2, __LINE__ );

  A::reset( 5 );
  try {
      A a[10] = { A(), A(), A() };
      fail( __LINE__ );
  } catch (const int &) {
  }
  A::check( 5, __LINE__ );

  A::reset( -1 );
  try {
      B b;
  } catch (const int &) {
      fail( __LINE__ );
  }
  A::check( 10, __LINE__ );

  A::reset( -1 );
  try {
      B b = { A(), A(), A(), A(), { A(), A(), A(), A(), A() }, A() };
  } catch (const int &) {
      fail( __LINE__ );
  }
  A::check( 10, __LINE__ );

// compiler doesn't pass the following test-cases
#if 0
  A::reset( -1 );
  try {
      B b = { A(), A(), A() };
  } catch (const int &) {
      fail( __LINE__ );
  }
  A::check( 10, __LINE__ );

  A::reset( 2 );
  try {
      B b = { A(), A(), A() };
      fail( __LINE__ );
  } catch (const int &) {
  }
  A::check( 2, __LINE__ );

  A::reset( 5 );
  try {
      B b = { A(), A(), A() };
      fail( __LINE__ );
  } catch (const int &) {
  }
  A::check( 5, __LINE__ );
#endif

  _PASS;
}
