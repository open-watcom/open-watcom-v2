struct D {
    int a;
};

struct U;
struct S : U, S, D {
    int a;
};

struct Q {
    struct X : Q {
    };
};

template <class T>
    struct S3;

template <class T>
    struct S1 : S3<T> {
    };

template <class T>
    struct S2 : S1<T> {
    };

template <class T>
    struct S3 {
	S2<T> *link;
    };

S3<int> x;
