#include <stdio.h>
// completeness test for operator overloading


#define binop( op )                                         \
    S& operator op( int d )                                 \
    {   printf(" %d.operator " #op " ( %d )\n", id, d );    \
        return *this;                                       \
    }

#define bintest( op )                                       \
    s1 op __LINE__ ;

#define unop( op )                                          \
    S& operator op()                                        \
    {   printf(" %d.operator " #op " ()\n", id );           \
        return *this;                                       \
    }

#define untest( op )                                        \
    printf( "%d ", __LINE__ ); op s1;

#define posttest( op )                                      \
    printf( "%d ", __LINE__ ); s1 op;

struct S
{
    int id;

    S( int id )
    {   this->id = id;
    }

    binop( + );     unop( + );
    binop( - );     unop( - );
    binop( * );     unop( * );
    binop( / );
    binop( % );
    binop( ^ );
    binop( & );  // unop( & ); defined below
    binop( | );
                    unop( ~ );

                    unop( ! );
    binop( = );
    binop( > );
    binop( < );
    binop( += );
    binop( -= );
    binop( *= );
    binop( /= );
    binop( %= );

    binop( ^= );
    binop( &= );
    binop( |= );
    binop( >> );
    binop( << );
    binop( >>= );
    binop( <<= );
    binop( == );
    binop( != );

    binop( <= );
    binop( >= );
    binop( && );
    binop( || );
    binop( ++ );    unop( ++ );
    binop( -- );    unop( -- );
    binop( ->* );

    S& operator ,( int d )
    {   printf(" %d.operator , ( %d )\n", id, d );
        return *this;
    }

    S* operator ->( )
    {   printf(" %d.operator -> ( )\n", id );
        return this;
    }

    S* operator &( )
    {   printf(" %d.operator & ( )\n", id );
        return this;
    }

    S* operator () ( )
    {   printf(" %d.operator () ( )\n", id );
        return this;
    }

    binop( [] );
};

S s1(1);


main()
{
    bintest( + );     untest( + );
    bintest( - );     untest( - );
    bintest( * );     untest( * );
    bintest( / );
    bintest( % );
    bintest( ^ );
    bintest( & );     untest( & );
    bintest( | );
                      untest( ~ );

                      untest( ! );
    bintest( = );
    bintest( > );
    bintest( < );
    bintest( += );
    bintest( -= );
    bintest( *= );
    bintest( /= );
    bintest( %= );

    bintest( ^= );
    bintest( &= );
    bintest( |= );
    bintest( >> );
    bintest( << );
    bintest( >>= );
    bintest( <<= );
    bintest( == );
    bintest( != );

    bintest( <= );
    bintest( >= );
    bintest( && );
    bintest( || );
                      untest( ++ );     posttest( ++ );
                      untest( -- );     posttest( -- );
    bintest( ->* );

    s1 , __LINE__;
    printf( "%d %d.operator->\n", __LINE__, (&s1)->id );
    s1 [ __LINE__ ];
    posttest( () );
    return 0;
}
