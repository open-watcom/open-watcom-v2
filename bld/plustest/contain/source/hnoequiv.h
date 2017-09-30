// WCValHashTable
class testVHTNoEquiv1 {
private:
    int i;
public:
    testVHTNoEquiv1( int data ) : i( data ) {};
    int data() { return( i ); };
    testVHTNoEquiv1() : i( 0 ) {};
    testVHTNoEquiv1( const testVHTNoEquiv1 & o ) { i = o.i; };
    testVHTNoEquiv1 &operator = ( const testVHTNoEquiv1 & o ) { i = o.i; return *this; };
    int operator== ( const testVHTNoEquiv1 & r ) { return( i == r.i ); };

    static unsigned hash_fn( const testVHTNoEquiv1 &object ) { return( object.i ); }
};

class testVHTNoEquiv2 {
private:
    int i;
public:
    testVHTNoEquiv2( int data ) : i( data ) {};
    int data() { return( i ); };
    testVHTNoEquiv2() : i( 0 ) {};
    testVHTNoEquiv2( const testVHTNoEquiv2 & o ) { i = o.i; };
    testVHTNoEquiv2 &operator = ( const testVHTNoEquiv2 & o ) { i = o.i; return *this; };
    int operator== ( testVHTNoEquiv2 & r )const { return( i == r.i ); };

    static unsigned hash_fn( const testVHTNoEquiv2 &object ) { return( object.i ); }
};

// WCPtrHashTable
class testPHTNoEquiv1 {
private:
    int i;
    testPHTNoEquiv1() : i( 0 ) {};
    testPHTNoEquiv1( const testPHTNoEquiv1 & o ) { i = o.i; };
    testPHTNoEquiv1 &operator = ( const testPHTNoEquiv1 & o ) { i = o.i; return *this; };
public:
    testPHTNoEquiv1( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( const testPHTNoEquiv1 & r ) { return( i == r.i ); };

    static unsigned hash_fn( const testPHTNoEquiv1 &object ) { return( object.i ); }
};

class testPHTNoEquiv2 {
private:
    int i;
    testPHTNoEquiv2() : i( 0 ) {};
    testPHTNoEquiv2( const testPHTNoEquiv2 & o ) { i = o.i; };
    testPHTNoEquiv2 &operator = ( const testPHTNoEquiv2 & o ) { i = o.i; return *this; };
public:
    testPHTNoEquiv2( int data ) : i( data ) {};
    int data() { return( i ); };
    int operator== ( testPHTNoEquiv2 & r )const { return( i == r.i ); };

    static unsigned hash_fn( const testPHTNoEquiv2 &object ) { return( object.i ); }
};

// WCValHashSet
typedef testVHTNoEquiv1 testVHSNoEquiv1;
typedef testVHTNoEquiv2 testVHSNoEquiv2;
// WCPtrHashSet
typedef testPHTNoEquiv1 testPHSNoEquiv1;
typedef testPHTNoEquiv2 testPHSNoEquiv2;

// WCValHashDict Key
typedef testVHSNoEquiv1 testVHDKNoEquiv1;
typedef testVHSNoEquiv2 testVHDKNoEquiv2;

// WCValHashDict Value test
class testVHDV {
public:
    int i;
    testVHDV( int data ) : i( data ) {};
    int data() { return( i ); };

    testVHDV() : i( 0 ) {};
    testVHDV( const testVHDV & o ) { i = o.i; };
    testVHDV &operator = ( const testVHDV & o ) { i = o.i; return *this; };
};

// WCPtrHashDict Key
typedef testPHSNoEquiv1 testPHDKNoEquiv1;
typedef testPHSNoEquiv2 testPHDKNoEquiv2;

// WCPtrHashDict Value
class testPHDV {
private:
    int i;
    testPHDV() : i( 0 ) {};
    testPHDV( const testPHDV & o ) { i = o.i; };
    testPHDV &operator = ( const testPHDV & o ) { i = o.i; return *this; };
public:
    testPHDV( int data ) : i( data ) {};
    int data() { return( i ); };
};

    

