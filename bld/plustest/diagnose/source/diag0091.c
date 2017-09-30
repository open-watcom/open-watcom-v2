// class template or function template?
template <class T>
    class C {
	T value;
    } fn( T x ) {
	C y;
	
	y.x = 0;
	return y;
    }
