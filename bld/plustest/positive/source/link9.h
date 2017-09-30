struct C {
    static int count;
    C() {
	++count;
    }
    ~C() {
	--count;
    }
};

template <class T>
    struct S {
	static C qq;
    };

template <class T>
    static C S<T>::qq;
