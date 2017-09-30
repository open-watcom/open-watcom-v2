// default arguments are not part of the type of a function
int (*f)( int, int = 1 );

// gaps in default arguments are never allowed
int (*g)( int, int = 1, int, int = 3 );
int sam( int = 3, int, int = 5 );

// overloaded operators cannot have default arguments
struct S {
    operator + ( int = 2 );
};

// should report "ambiguous defs" or something
int fn2( int, int );
int fn2( int, int, int = 1, int = 2 );

int fn3( int, int, int = 1 );
int fn3( int, int, int = 1, int = 2 );

// default arguments can be added
int bar( int    , int    , int    , int     );
int bar( int    , int    , int    , int = 3 );
int bar( int    , int    , int = 2, int     );
int bar( int    , int = 1, int    , int     );
int bar( int = 0, int    , int    , int     );
int bar( int    , int    , int    , int     );

// but not redefined!
int foo( int    , int    , int    , int     );
int foo( int    , int    , int    , int = 3 );
int foo( int    , int    , int = 2, int     );
int foo( int    , int = 1, int = 2, int = 3 );

// works with ellipse functions also
void fubar( int, int = 1, ... );
void fubar( int, int = 1, int = 2, ... );
