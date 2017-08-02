template <class T>
    struct S {
	typedef int X;
	static X foo( int );
	static X qq;
    };

template <class T>
    S<T>::X foo( int ) {	// parser thinks this is a member fn of S<T>!

	return foo( 2 );
    }

template <class T>
    S<T>::X qq = 1;

S<int> x;
