// WCValSkipListList
class testVSLNoEquiv1 {
private:
    int i;
public:
    testVSLNoEquiv1( int data ) : i( data ) {};
    int data() { return( i ); };
    testVSLNoEquiv1() : i( 0 ) {};
    testVSLNoEquiv1( const testVSLNoEquiv1 & o ) { i = o.i; };
    int operator== ( const testVSLNoEquiv1 & r ) { return( i == r.i ); };
    int operator< ( const testVSLNoEquiv1 & r )const { return( i < r.i ); };
    testVSLNoEquiv1 &operator = ( const testVSLNoEquiv1 & o ) { i = o.i; return *this; };
};

class testVSLNoEquiv2 {
private:
    int i;
public:
    testVSLNoEquiv2( int data ) : i( data ) {};
    int data() { return( i ); };
    testVSLNoEquiv2() : i( 0 ) {};
    testVSLNoEquiv2( const testVSLNoEquiv2 & o ) { i = o.i; };
    int operator== ( testVSLNoEquiv2 & r )const { return( i == r.i ); };
    int operator< ( const testVSLNoEquiv2 & r )const { return( i < r.i ); };
    testVSLNoEquiv2 &operator = ( const testVSLNoEquiv2 & o ) { i = o.i; return *this; };
};

class testVSLNoLess1 {
private:
    int i;
public:
    testVSLNoLess1( int data ) : i( data ) {};
    int data() { return( i ); };
    testVSLNoLess1() : i( 0 ) {};
    testVSLNoLess1( const testVSLNoLess1 & o ) { i = o.i; };
    int operator== ( const testVSLNoLess1 & r )const { return( i == r.i ); };
    int operator< ( const testVSLNoLess1 & r ) { return( i < r.i ); };
    testVSLNoLess1 &operator = ( const testVSLNoLess1 & o ) { i = o.i; return *this; };
};

class testVSLNoLess2 {
private:
    int i;
public:
    testVSLNoLess2( int data ) : i( data ) {};
    int data() { return( i ); };
    testVSLNoLess2() : i( 0 ) {};
    testVSLNoLess2( const testVSLNoLess2 & o ) { i = o.i; };
    int operator== ( const testVSLNoLess2 & r )const { return( i == r.i ); };
    int operator< ( testVSLNoLess2 & r )const { return( i < r.i ); };
    testVSLNoLess2 &operator = ( const testVSLNoLess2 & o ) { i = o.i; return *this; };
};

// WCPtrSkipListTable
class testPSLNoEquiv1 {
private:
    int i;
    testPSLNoEquiv1() : i( 0 ) {};
    testPSLNoEquiv1( const testPSLNoEquiv1 & o ) { i = o.i; };
    testPSLNoEquiv1 &operator = ( const testPSLNoEquiv1 & o ) { i = o.i; return *this; };
public:
    testPSLNoEquiv1( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( const testPSLNoEquiv1 & r ) { return( i == r.i ); };
    int operator< ( const testPSLNoEquiv1 & r )const { return( i < r.i ); };
};

class testPSLNoEquiv2 {
private:
    int i;
    testPSLNoEquiv2() : i( 0 ) {};
    testPSLNoEquiv2( const testPSLNoEquiv2 & o ) { i = o.i; };
    testPSLNoEquiv2 &operator = ( const testPSLNoEquiv2 & o ) { i = o.i; return *this; };
public:
    testPSLNoEquiv2( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( testPSLNoEquiv2 & r )const { return( i == r.i ); };
    int operator< ( const testPSLNoEquiv2 & r )const { return( i < r.i ); };
};

class testPSLNoLess1 {
private:
    int i;
    testPSLNoLess1() : i( 0 ) {};
    testPSLNoLess1( const testPSLNoLess1 & o ) { i = o.i; };
    testPSLNoLess1 &operator = ( const testPSLNoLess1 & o ) { i = o.i; return *this; };
public:
    testPSLNoLess1( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( const testPSLNoLess1 & r )const { return( i == r.i ); };
    int operator< ( const testPSLNoLess1 & r ) { return( i < r.i ); };
};

class testPSLNoLess2 {
private:
    int i;
    testPSLNoLess2() : i( 0 ) {};
    testPSLNoLess2( const testPSLNoLess2 & o ) { i = o.i; };
    testPSLNoLess2 &operator = ( const testPSLNoLess2 & o ) { i = o.i; return *this; };
public:
    testPSLNoLess2( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( const testPSLNoLess2 & r )const { return( i == r.i ); };
    int operator< ( testPSLNoLess2 & r )const { return( i < r.i ); };
};
