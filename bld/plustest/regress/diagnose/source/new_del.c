typedef unsigned size_t;
int se;
unsigned n = 5;
struct E {
};
struct EN {
    void *operator new( size_t amt ) { return (void *) new char[amt+__LINE__]; }
    void operator delete( void *p ) { se=__LINE__;delete p; }
};
struct C {
    C(){se=__LINE__;}
};
struct CN {
    void *operator new( size_t amt ) { return (void *) new char[amt+__LINE__]; }
    void operator delete( void *p ) { se=__LINE__;delete p; }
    CN(){se=__LINE__;}
};
struct D {
    ~D(){se=__LINE__;}
};
struct DN {
    void *operator new( size_t amt ) { return (void *) new char[amt+__LINE__]; }
    void operator delete( void *p ) { se=__LINE__;delete p; }
    ~DN(){se=__LINE__;}
};
struct CD {
    CD(){se=__LINE__;}
    ~CD(){se=__LINE__;}
};
struct CDN {
    void *operator new( size_t amt ) { return (void *) new char[amt+__LINE__]; }
    void operator delete( void *p ) { se=__LINE__;delete p; }
    CDN(){se=__LINE__;}
    ~CDN(){se=__LINE__;}
};
void dummy( void ) {se=__LINE__;}
void main( void )
{
    E *e;
    EN *en;
    C *c;
    CN *cn;
    D *d;
    DN *dn;
    CD *cd;
    CDN *cdn;

    e = new E;
    delete e;
    e = new E[n];
    delete [] e;
    en = new EN;
    delete en;
    en = new EN[n];
    delete [] en;
    e = ::new E;
    ::delete e;
    e = ::new E[n];
    ::delete [] e;
    en = ::new EN;
    ::delete en;
    en = ::new EN[n];
    ::delete [] en;
    dummy();
    c = new C;
    delete c;
    c = new C[n];
    delete [] c;
    cn = new CN;
    delete cn;
    cn = new CN[n];
    delete [] cn;
    c = ::new C;
    ::delete c;
    c = ::new C[n];
    ::delete [] c;
    cn = ::new CN;
    ::delete cn;
    cn = ::new CN[n];
    ::delete [] cn;
    dummy();
    d = new D;
    delete d;
    d = new D[n];
    delete [] d;
    dn = new DN;
    delete dn;
    dn = new DN[n];
    delete [] dn;
    d = ::new D;
    ::delete d;
    d = ::new D[n];
    ::delete [] d;
    dn = ::new DN;
    ::delete dn;
    dn = ::new DN[n];
    ::delete [] dn;
    dummy();
    cd = new CD;
    delete cd;
    cd = new CD[n];
    delete [] cd;
    cdn = new CDN;
    delete cdn;
    cdn = new CDN[n];
    delete [] cdn;
    cd = ::new CD;
    ::delete cd;
    cd = ::new CD[n];
    ::delete [] cd;
    cdn = ::new CDN;
    ::delete cdn;
    cdn = ::new CDN[n];
    ::delete [] cdn;
    dummy();
}
