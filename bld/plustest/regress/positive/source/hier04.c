#include "fail.h"
// test name look-up
#include <stdio.h>

static void chk( int v1, int v2, char *text )
{
    if( v1 != v2 ) {
        printf( "%d %d %s\n", v1, v2, text );
	fail(__LINE__);
    }
}

struct S
{ int v;
  static int s_stat;
  void smemb();
  S();
  S(int);
};

int S::s_stat = 10;

S::S() : v(0)
{}

S::S( int p ) : v(p)
{}


struct T : virtual S
{ int t;
  static int t_stat;
  void tmemb();
  T();
  T(int,int);
};

int T::t_stat = 20;

T::T() : t(0), S(0)
{}

T::T( int v1, int v2 ) : t(v1), S(v2)
{}


struct U
{ int u;
  U();
  U(int);
};

U::U() : u(0)
{}

U::U( int v ) : u(v)
{}


int v = 1;
S sv(2);
T tv(3,4);
U tu(5);

void S::smemb()
{
    chk( v,       2, "S::smemb -- v"        );
    chk( S::v,    2, "S::smemb -- S::v"     );
    chk( ::v,     1, "S::smemb -- ::v"      );
    chk( sv.v,    2, "S::smemb -- sv.v"     );
    chk( sv.S::v, 2, "S::smemb -- sv.S::v"  );
    chk( tu.u,    5, "S::smemb -- tu.u"     );
}

void T::tmemb()
{
    chk( v,       4, "T::tmemb -- v"        );
    chk( S::v,    4, "T::tmemb -- S::v"     );
    chk( ::v,     1, "T::tmemb -- ::v"      );
    chk( sv.v,    2, "T::tmemb -- sv.v"     );
    chk( sv.S::v, 2, "T::tmemb -- sv.S::v"  );
    chk( tv.T::v, 4, "T::tmemb -- sv.T::v"  );
    chk( tu.u,    5, "T::tmemb -- tu.u"     );
}

void bare()
{
    chk( S::s_stat, 10, "bare -- S::s_stat"     );
    chk( T::t_stat, 20, "bare -- T::t_stat"     );
    chk( v,         1,  "bare -- v"             );
    chk( ::v,       1,  "bare -- ::v"           );
    chk( sv.v,      2,  "bare -- sv.v"          );
    chk( sv.S::v,   2,  "bare -- sv.S::v"       );
    chk( tv.v,      4,  "bare -- tv.v"          );
    chk( tv.S::v,   4,  "bare -- tv.S::v"       );
    chk( tv.T::v,   4,  "bare -- tv.T::v"       );
    chk( tu.u,      5,  "bare -- tu.u"          );
}

int main( void )
{
    bare();
    sv.smemb();
    tv.tmemb();

    _PASS;
}
