// test array delete
//
// 93/02/03 -- J.W.Welch        -- defined

// #pragma on (dump_ptree)
// #pragma on (dump_emit_ic)
// #pragma on (dump_lines)
// #pragma on (dump_tokens)

extern "C" char* printf( const char* ... );

int ctr = 0;
int dted = 0;

struct S {
    int s;

    S()
            {   s = ctr++;
                print();
            }
    ~S()
            {   print();
                if( 0 == --dted ) {
                    printf( " throw" );
                    throw 567;
                }
            }
    print()
            {   printf( " %d", s );
            }
};


int errors = 0;

void error( const char *msg )
{
    printf( "\n**** %s ****\n", msg );
    ++errors;
}



int main( )
{
    int i;

    for( i = 1; i < 12; ++i ) {
        try {
            S* p;
            dted = i;
            ctr = 0;
            printf( "\nS()" );
            p = new S[10];
            printf( "\n~S()" );
            delete[] p;
            printf( "\n\n" );
        } catch( int k ) {
            if( k == 567 ) {
                printf( " caught" );
            } else {
                error( "bad catch value" );
            }
        }
    }
    return( 0 );
}


// #pragma on ( dump_scopes )
// #pragma on (dump_exec_ic)
