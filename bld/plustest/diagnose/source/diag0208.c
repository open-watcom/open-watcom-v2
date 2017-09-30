// test optimized copy ctor still diagnoses correctly.

struct S {
    int s;
#if 0
    S( const S& );
    S( int );
#endif
};

S src_1                 = { 1 };
S const src_2           = { 1 };
S volatile src_3        = { 1 };
S const volatile src_4  = { 1 };

void foo( )
{
    S                tgt_1 = src_1;      // ok
    S       volatile tgt_2 = src_1;      // ok
    S const          tgt_3 = src_1;      // ok
    S const volatile tgt_4 = src_1;      // ok
    S                tgt_5 = src_2;      // ok
    S       volatile tgt_6 = src_2;      // ok
    S const          tgt_7 = src_2;      // ok
    S const volatile tgt_8 = src_2;      // ok
    S                tgt_9 = src_3;      // ERR
    S       volatile tgt_A = src_3;      // ERR
    S const          tgt_B = src_3;      // ERR
    S const volatile tgt_C = src_3;      // ERR
    S                tgt_D = src_4;      // ERR
    S       volatile tgt_E = src_4;      // ERR
    S const          tgt_F = src_4;      // ERR
    S const volatile tgt_G = src_4;      // ERR
}


//   #pragma on ( dump_exec_ic )
//   #pragma on ( dump_stab )
//   #pragma on ( callgraph )
