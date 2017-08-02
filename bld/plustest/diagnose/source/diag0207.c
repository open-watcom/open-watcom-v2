// test optimized op= still diagnoses correctly.

struct S {
    int s;
//  S& operator=( const S& );
};

S src_1                 = { 1 };
S const src_2           = { 1 };
S volatile src_3        = { 1 };
S const volatile src_4  = { 1 };
S tgt_1                 = { 1 };
S const tgt_2           = { 1 };
S volatile tgt_3        = { 1 };
S const volatile tgt_4  = { 1 };

void foo( )
{
    tgt_1 = src_1;      // ok
    tgt_2 = src_1;      // ERR
    tgt_3 = src_1;      // ERR
    tgt_4 = src_1;      // ERR
    tgt_1 = src_2;      // ok
    tgt_2 = src_2;      // ERR
    tgt_3 = src_2;      // ERR
    tgt_4 = src_2;      // ERR
    tgt_1 = src_3;      // ERR
    tgt_2 = src_3;      // ERR
    tgt_3 = src_3;      // ERR
    tgt_4 = src_3;      // ERR
    tgt_1 = src_4;      // ERR
    tgt_2 = src_4;      // ERR
    tgt_3 = src_4;      // ERR
    tgt_4 = src_4;      // ERR
}


//   #pragma on ( dump_exec_ic )
//   #pragma on ( dump_stab )
//   #pragma on ( callgraph )
