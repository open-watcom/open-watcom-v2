int z;
struct S1 {
    int *S1;
    S1();
    ~S1();
};
struct S2 {
    S2();
    S2( S2 & );
    ~S2();
};
S2::S2( S2 & )
{
    z = 2;
}
struct S3 {
    ~S3();
    int *S3;
    int S3();
    void S3(int);
    S3(char *);
};
struct S4 {
    S4();
    S4( S4 );
    ~S4();
};
S4::S4( S4 )
{
    z = 1;
}
struct S5 {
    S5() const;
    int S5;
    S5( int ) volatile;
    virtual S5( char * );
};
