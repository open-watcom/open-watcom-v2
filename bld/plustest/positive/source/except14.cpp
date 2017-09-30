// except14 -- test destruction from blocks with goto, return

#include "fail.h"

int ctored_exc1 = 0;
struct exc1 {
    ~exc1() { --ctored_exc1; }
    exc1() { ++ctored_exc1; };
    exc1( const exc1& ) { ++ctored_exc1; }
};

int ctored_exc2 = 0;
struct exc2 {
    ~exc2() { --ctored_exc2; }
    exc2() { ++ctored_exc2; };
    exc2( const exc2& ) { ++ctored_exc2; }
};

int ctored_exc3 = 0;
struct exc3 {
    ~exc3() { --ctored_exc3; }
    exc3() { ++ctored_exc3; };
    exc3( const exc3& ) { ++ctored_exc3; }
};

int ctored_exc4 = 0;
struct exc4 {
    ~exc4() { --ctored_exc4; }
    exc4() { ++ctored_exc4; };
    exc4( const exc4& ) { ++ctored_exc4; }
};

int ctored_exc5 = 0;
struct exc5 {
    ~exc5() { --ctored_exc5; }
    exc5() { ++ctored_exc5; };
    exc5( const exc5& ) { ++ctored_exc5; }
};

int ctored_exc6 = 0;
struct exc6 {
    ~exc6() { --ctored_exc6; }
    exc6() { ++ctored_exc6; };
    exc6( const exc6& ) { ++ctored_exc6; }
};


void burp( const char* msg )
{
    ++errors;
    printf( __FILE__ ": failed %s", msg );
}
    

void foo( int v )
{
    switch( v ) {
      case 1: throw exc1();
      case 2: throw exc2();
      case 3: throw exc3();
      case 4: throw exc4();
      case 5: throw exc5();
      case 6: throw exc6();
      default:
        burp( "bad testing value" );
    }
}
    

void goo( int v )
{
    goto l1;
  l2:
    goto lab;
  l1:    
    try {
        foo( v );
    } catch( exc1& ) {
        return;
    } catch( exc2& ) {
        goto lab;
    } catch( exc3& ) {
        goto l2;
    } catch( exc4& ) {
        {
            return;
        }
    } catch( exc5& ) {
        {
            goto lab;
        }
    } catch( exc6& ) {
        {
            goto l2;
        }
    }
  lab:
    return;
}


int main()
{
    goo(1);
    goo(2);
    goo(3);
    goo(4);
    goo(5);
    goo(6);
    if( ctored_exc1 ) burp( "failed test[1]" );
    if( ctored_exc2 ) burp( "failed test[2]" );
    if( ctored_exc3 ) burp( "failed test[3]" );
    if( ctored_exc4 ) burp( "failed test[4]" );
    if( ctored_exc5 ) burp( "failed test[5]" );
    if( ctored_exc6 ) burp( "failed test[6]" );
    _PASS;
}
