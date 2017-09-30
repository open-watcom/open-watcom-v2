// STMTCONS -- test processing of constants in IF, FOR, DO-WHILE, WHILE-DO
//             statements
//
// 92/07/01 -- J.W.Welch        -- defined


int f1()
{
    for( ;0; ) {    // test expr == 0
        return 1;
    }
}                   // no return

int f2()
{
    for( ;1; ) {
        return 1;
    }
}

int f3()
{
    for( ;1; ) {
        break;
    }
}                   // missing return

int f4()
{
    for( ;1; ) {
        break;
    }
    return 1;
}

int f5()
{
    while( 0 ) {    // always 0
        return 1;   // unreachable
    }
}                   // missing return

int f6()
{
    while( 1 ) {
    }
}                   // currently no errors

int f7()
{
    while( 1 ) {
        return 1;
    }
}

int f8()
{
    while( 1 ) {
        break;
    }
}                   // missing return

int f9()
{
    while( 1 ) {
        break;
    }
    return 1;
}

int f10()
{
    while( 0 ) {    // always false
        return 1;   // unreachable
    }
    return 1;
}

int f11()
{
    if( 1 ) {       // always true
        return 1;
    }
}

int f12()
{
    if( 0 ) {       // always false
        return 1;   // unreachable
    }
}                   // missing return

int f13()
{
    if( 0 ) {       // always false
        return 1;   // unreachable
    }
    return 1;
}

int f14()
{
    if( 0 ) {       // always false
    } else {
        return 1;
    }
}

int f15()
{
    if( 0 ) {       // always false
        return 1;   // unreachable
    } else {
        return 1;
    }
}

int f16( int a )
{
    if( a ) {
        return 1;
    } else {
        return 2;
    }
}

int f17( int a )
{
    switch( a ) {
      case 1 :
        return 1;
      case 2 :
        return 2;
    }
}                   // missing return

int f18( int a )
{
    switch( a ) {
      case 1 :
        return 1;
      case 2 :
        return 2;
      default :
        return 3;
    }
}

int f19( int a )
{
    do{
        if( a ) return 1;
        -- a;
    } while( 1 );
}

int f20( int a )
{
    do{
        if( a ) break;
        -- a;
    } while( 1 );
}                   // missing return

int f21( int a )
{
    do{
        if( a ) break;
        -- a;
    } while( 1 );
    return a;
}

int f22( int a )
{
    do{
        if( a ) break;
        -- a;
    } while( 0 );   // always zero
    return 3;
}
