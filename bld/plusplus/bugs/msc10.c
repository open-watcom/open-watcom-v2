/* Zortech is the only compiler that diagnoses these errors */
typedef int S1;
typedef char Q1;

struct S {
    Q1 *m1;
    S1 *m2;
    void m3( Q1 x );
    void m4( S1 x );
    struct Q {
        Q1 *m1;
        S1 *m2;
        void m3( Q1 x );
        void m4( S1 x );
        typedef float Q1;       /* 'Q1' redefined after use */
    };
    typedef double S1;          /* 'S1' redefined after use */
};
