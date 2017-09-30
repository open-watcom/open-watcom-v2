enum foo_enum { FOO, BAR, BAZ = 1234 }; /* force underlying type to 'short' */
typedef enum foo_enum foo_enum_t;

extern int foo( char e );

int foo( foo_enum_t e ) /* Does not match declaration. */
{
    return( e );
}

int bar( char *pc, foo_enum_t *pe )
{
    int     d = pc - pe;        /* Various pointer type mismatches. */
    pc = pe;
    ++d;
    return( pc == pe );
}

/* The integer type that the foo_enum_t type is compatible with; different
 * in 16-bit and 32-bit compilers, and also affected by -ei!
 */
#ifdef _M_I86
typedef int eint_t;
#else
typedef short eint_t;
#endif

extern int baz( eint_t e );

extern int baz( foo_enum_t e )  /* Compatible types, declaration OK. */
{
    return( e );
}

int bat( eint_t *pc, foo_enum_t *pe )
{
    int     d = pc - pe;        /* Pointer ops are OK. */
    pc = pe;
    ++d;
    return( pc == pe );
}
