// compiled -za

// class->scalar->class
struct S1 {
    operator double();
};

struct T1 {
    T1( int );
};

// class->class->class
struct INTER {
    int i;
    INTER(int);
};

struct S2 {
    operator INTER();
};

struct T2 {
    T2( INTER );
};

extern S1 s1;
extern S2 s2;

void main()
{
#if __WATCOM_REVISION__ >= 8
    T1 t1 = s1;	 	// error for -za
    T2 t2 = s2; 	// error for -za
#else
#error at least one
#endif
}
