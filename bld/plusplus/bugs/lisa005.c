void foo1( char const * );
void foo2( char const * & );

void bar( char * p ) {
    foo1( p );
    foo2( p );                  // (1) weird error msg
                                // (2) MSC++ accepts this
                                //      BC++ informs you that it made a temp
                                //      EDG rejects this
    foo2( (char const * &) p );
}
