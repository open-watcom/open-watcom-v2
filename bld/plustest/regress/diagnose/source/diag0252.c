// compiled -ze

struct S {

    static int foo( double ); 	// (2)
    int foo( int );		// (1)
    int moo( int ); 		// (3)

    static int goo( double ); 	// (4)

    int goo( int (S::*)( int ) );	// will match (1)
    int goo( int (*)( double ) );	// will match (2)

    void fun() {
        goo( S::foo );	// ambig with extensions enabled 
    }
};
