struct S {
    S(int);
    S();
};
void takes_S( S & );
void takes_cS( S const & );

S v;
S &r = v;
S const &cr = v;
S f();
S const fc();
S &fr();
S const &frc();
S (S::*mp)();
S (*pf)();

S &x10 = 1;
S &x11 = v;
S &x12 = r;
S &x13 = cr;
S &x14 = f();
S &x15 = fc();
S &x16 = fr();
S &x17 = frc();
S &x18 = S(1);
S &x19 = (v.*mp)();
S &x1a = (pf)();

S const &x20 = 1;
S const &x21 = v;
S const &x22 = r;
S const &x23 = cr;
S const &x24 = f();
S const &x25 = fc();
S const &x26 = fr();
S const &x27 = frc();
S const &x28 = S(1);
S const &x29 = (v.*mp)();
S const &x2a = (pf)();

void x34()
{
    takes_S( 1 );
    takes_S( v );
    takes_S( r );
    takes_S( cr );
    takes_S( f() );
    takes_S( fc() );
    takes_S( fr() );
    takes_S( frc() );
    takes_S( S(1) );
    takes_S( (v.*mp)() );
    takes_S( (pf)() );
    
    takes_cS( 1 );
    takes_cS( v );
    takes_cS( r );
    takes_cS( cr );
    takes_cS( f() );
    takes_cS( fc() );
    takes_cS( fr() );
    takes_cS( frc() );
    takes_cS( S(1) );
    takes_cS( (v.*mp)() );
    takes_cS( (pf)() );
}
