template <class T>
    struct Stack {
	T foo( T x )
	{
	    x.a = 1;
	}
    };

struct Stack<int> {	// can't cause Stack<int> to be generated
    int foo;
};

Stack<int> x;
Stack<double> y;	// causes errors
