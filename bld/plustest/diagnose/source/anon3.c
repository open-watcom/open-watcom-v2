void foo( double x )
{
    static union {
	double d;
	int i;
	float f;
    };
    union {
	long l;
	char c;
    };

    d = x;
    l = f;
    f = i + c;
}
