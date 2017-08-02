struct T {
    struct S1 {
	static int S1;
    };
    struct S2 {
	typedef int S2;
    };
    struct S3 {
	enum { S3 };
    };
    union {
	int T;
    };
};
