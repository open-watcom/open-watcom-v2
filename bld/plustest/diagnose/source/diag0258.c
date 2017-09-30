#define CHAR_MAX (127)

typedef char T;
struct Q {
int h( int t= (4+(3-7), // missing parenthesis
);
};

struct S {
    void f( T t= (CHAR_MAX+1) ); // thinks T is char, warns about truncation
    typedef int T;
    void g( T t= (CHAR_MAX+1) ); // OK
};

void S::f(T t)	// complains that S::f(int) not member
		// (it only knows about S::f(char)
{
    int i;

    i = t;
}

void S::g(T t) //OK
{
    int i;

    i = t;
}
