void xxx( int, int );
void xxx( char, char );
void xxx( double, double );

template <class T>
    struct S {
        T a;
    };

template <class T>
    void foo( S<T>, void (*)( T, T ) );

template <class T>
    void foo( void (*)( T, T ), S<T> );

template <class T>
    void bar( register void (*)( T, T ) );

void main()
{
    S<char> i;

    foo( i, &xxx );     // OK "void xxx( char, char );"
    foo( &xxx, i );     // OK "void xxx( char, char );"
    bar( &xxx );        // ambiguous
}
