// #pragma on (dump_ptree)
// #pragma on (dump_emit_ic)
// #pragma on (dump_exec_ic)
// #pragma on (dump_lines)
// #pragma on (print_ptree)


struct S1 {
    operator int();
};

struct S2 {
    operator int&();
};

struct S3 {
    S3(int);
};

struct S4 {
    operator int();
    S3(int);
};

struct S5 {
    operator int&();
    S3(int);
};

int i1, i2, i3, i4;


void foo1( S1 &s1, S2& s2, S3 s3 )
{
    int& i = s2;
    i1 = i1 ? i1 : s1;
    i2 = i1 ? i1 : s2;
    S3 s3_auto = i1 ? i1 : s3;
}


void foo2( S4 &s1, S5& s2, S3 s3 )
{
    int& i = s2;
    i1 = i1 ? i1 : s1;
    i2 = i1 ? i1 : s2;
    S3 s3_auto = i1 ? i1 : s3;
}


#error this is the only error
    

// #pragma on (dump_tokens)
// #pragma on ( dump_scopes )
// #pragma on ( dump_exec_ic )
// #pragma on ( dump_stab )
