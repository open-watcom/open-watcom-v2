// Borland and MetaWare create a file-scope class T friend!
// CFRONT changes template-arg T! (this is after we remove the
// use of 'T' as a base class (3.0.2 didn't implement it!))
struct U {
    int u;
    static void foo();
};

template <class T>
    struct S : T {
	friend class T;
	S<T> *n;
    private:
	int a;
    }
    ;

S<U> ZZ;

void U::foo()
{
    ZZ.a = 1;			// should work
}

struct T {
    static void foo()
    {
	ZZ.a = 1;		// should fail
    }
};
