// BADSTAT.C - TEST ERROR CASES OF STATEMENTS
//
// 91/09/27     -- Ian McHardy          -- defined

void function( void )
{
label1:                         // OK
function_label:                 // OK
    int a;
    a = 1;
}

class test_class{               // OK
    public:
        int data;
};

void main( void )
{
    int i;
    float f;
    unsigned long int uli;
    void *pv;
    double *pd;
    long double ld;
    test_class tc;

// 6.1

label1:
case:                           // case out of a switch
default:                        // default out of a switch
case 7:                         // ditto
label1:                         // redefinition
label2:                         // OK

// 6.4
    i = 1;
    if( i ) int j;              // conditionally declaring j (error)
    if( i ){
        f = 3.14;
    }else
        int j;                  // ditto
    switch( i ) case( 1 ): int j;//ditto

// 6.4.1
    if( tc ) i = 1;             // expression cannot be a class

// 6.4.2
    switch( f ) case 1:i = 0;   // switch operand must be integral
    switch( pv ) case 0:i = 1;  // ditto
    switch( function ) case 0:i = 2;// ditto
    switch( 1 ) case 1:i = 3;   // OK
    switch( tc ) case 1: i = 4; // bad operand
    switch( i ){
    case 1:
        uli = 1;
    case 2:
        uli = 5;
    case 1:                     // duplication of case
        uli = 32;
    }
    switch( i ){
    case 1:
        uli = 1;
    default:
        uli = 17;
    case 3:
        uli = 4235;
    default:                    // duplication of default
        uli = 321;
    }
    switch( i ){
    case 1:
        uli = 1;
    case 2:
        uli = 2;
    case 3:
        uli = 3;
    default:
        int i2;                 // OK
        i2 = 14;
    }
    switch( i ){
    case 1:
        for( uli = 1; uli != 5; uli++ ){
            int j;              // OK
            j = 4;
        }
    default:
        i = 1;
    }

// 6.5
    for( ; ; ) int j;           // statement cannot be a declaration
    do int j; while( 1 );       // ditto
    while( 1 ) int j;           // ditto

// 6.5.1
    while( tc );                // expression can't be class
    while( function );          // OK (function is a ptr)

// 6.5.2
    do; while( function );      // ditto
    do; while( tc );            // expression can't be class

// 6.5.3
    for( ; function; );         // OK (function is a ptr)
    for( ; tc; );               // expression can't be a class
    for( ; ; ){
        int j1;                 // OK (within block scope)
        j1 = 5;
    }
    i = j1;                     // j1 has not be declared in scope
    for( int j1 = 1; ; ){       // OK
        i = j1;
    }
    for( int j1 = 1; ; ){       // redeclaration
        i = j1;
    }

// 6.6.1
    break;                      // break must be in for, while, do, switch
    if( i ){
        break;                  // ditto
    }

// 6.6.2
    continue;                   // continue must be in for, while, do
    if( i ){
        continue;               // ditto
    }
    switch( i ){
    case 1: continue;           // ditto
    }

// 6.6.4
    goto junk;                  // label not defined
    goto function_label;        // function_label defined out of function

// 6.7
before_declaration:
    const volatile unsigned long int far *const volatile     cvulifpcvf;
    goto after_declaration;     // jumping over declaration
    int k;
after_declaration:
    goto before_declaration;    // OK
    for( ; ; ){
        int l;                  // OK
        l = 1;
    }
    l = 2;                      // l not defined in scope
    goto after_decl2;           // this should not give an error since m in {}
    for( ; ; ){
        int m;
        m = 1;
    }
after_decl2:
}

// 6.6.3
void t1()
{
    return( 1 );                // return value when return type void
}

int *t2()
{
    int a;

    return( a );                // return types don't match
}

void t3()
{
    return;                     // OK
}

int t4()
{
    int a;

    a = 1;                      // no value returned
}

int t5()
{
    int a;

    a = 1;
    return;                     // no value returned
}

int t6()
{
    int a;
    float b;
    int *d;

    return( a );
    return( b );
    return( d );
}
