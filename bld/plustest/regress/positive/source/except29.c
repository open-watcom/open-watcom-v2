#include "fail.h"

#if __WATCOMC__ > 1060

int throws;
int catches;
int coverage;

struct X {
    int sig;
    X( int x ) : sig(x) {
    }
};

void f( int i )
{
    switch( i ) {
      case 23 :
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                throw 789;
            } catch( int c ) {
                if( 789 != c ) fail( __LINE__ );
                try {
                    break;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
        }
        break;
      case 22:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            try {
                throw 789;
            } catch( int c ) {
                if( 789 != c ) fail( __LINE__ );
                break;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail( __LINE__ );
        }
        break;
      case 21:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                throw 789;
            } catch( int c ) {
                if( 789 != c ) fail( __LINE__ );
                try {
                    throw 123;
                } catch( int j ) {
                    if( j != 123 ) {
                        fail(__LINE__);
                        return;
                    }
                    break;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
        }
        break;
      case 20:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            throw 789;
        } catch( int c ) {
            if( 789 != c ) fail( __LINE__ );
            try {
                throw 123;
            } catch( int j ) {
                if( j != 123 ) {
                    fail(__LINE__);
                    return;
                }
                break;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail(__LINE__);
        }
        break;
      case 19:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                throw 789;
            } catch( int c ) {
                if( 789 != c ) fail( __LINE__ );
                try {
                    goto label;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
            break;
        }
        break;
      case 18:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            throw 789;
        } catch( int c ) {
            if( 789 != c ) fail( __LINE__ );
            try {
                goto label;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail( __LINE__ );
        }
        break;
      case 17:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                throw 789;
            } catch( int c ) {
                if( 789 != c ) fail( __LINE__ );
                try {
                    throw 123;
                } catch( int j ) {
                    if( j != 123 ) {
                        fail(__LINE__);
                        return;
                    }
                    goto label;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
            break;
        }
      case 16:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            throw 789;
        } catch( int c ) {
            if( 789 != c ) fail( __LINE__ );
            try {
                throw 123;
            } catch( int j ) {
                if( j != 123 ) {
                    fail(__LINE__);
                    return;
                }
                goto label;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail( __LINE__ );
        }
        break;
      case 15 :
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                try {
                    break;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
        }
        break;
      case 14:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            try {
                break;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail( __LINE__ );
        }
        break;
      case 13:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                try {
                    throw 123;
                } catch( int j ) {
                    if( j != 123 ) {
                        fail(__LINE__);
                        return;
                    }
                    break;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
        }
        break;
      case 12:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            try {
                throw 123;
            } catch( int j ) {
                if( j != 123 ) {
                    fail(__LINE__);
                    return;
                }
                break;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail(__LINE__);
        }
        break;
      case 11:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                try {
                    goto label;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
            break;
        }
        break;
      case 10:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            try {
                goto label;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail( __LINE__ );
        }
        break;
      case 9:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                try {
                    throw 123;
                } catch( int j ) {
                    if( j != 123 ) {
                        fail(__LINE__);
                        return;
                    }
                    goto label;
                } catch( X ) {
                    fail(__LINE__);
                    return;
                } catch( ... ) {
                    fail(__LINE__);
                    return;
                }
            } catch( ... ) {
                fail( __LINE__ );
            }
            break;
        }
      case 8:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            try {
                throw 123;
            } catch( int j ) {
                if( j != 123 ) {
                    fail(__LINE__);
                    return;
                }
                goto label;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        } catch( ... ) {
            fail( __LINE__ );
        }
        break;
      case 7:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                break;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        }
        break;
      case 6:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            break;
        } catch( X ) {
            fail(__LINE__);
            return;
        } catch( ... ) {
            fail(__LINE__);
            return;
        }
      case 5 :
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                throw 123;
            } catch( int j ) {
                if( j != 123 ) {
                    fail(__LINE__);
                    return;
                }
                break;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        }
        break;
      case 4 :
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            throw 123;
        } catch( int j ) {
            if( j != 123 ) {
                fail(__LINE__);
                return;
            }
            break;
        } catch( X ) {
            fail(__LINE__);
            return;
        } catch( ... ) {
            fail(__LINE__);
            return;
        }
      case 3:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                goto label;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        }
      case 2:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            goto label;
        } catch( X ) {
            fail(__LINE__);
            return;
        } catch( ... ) {
            fail(__LINE__);
            return;
        }
      case 1:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        for(;;) {
            try {
                throw 123;
            } catch( int j ) {
                if( j != 123 ) {
                    fail(__LINE__);
                    return;
                }
                goto label;
            } catch( X ) {
                fail(__LINE__);
                return;
            } catch( ... ) {
                fail(__LINE__);
                return;
            }
        }
      case 0:
        if( coverage != i ) fail( __LINE__ );
        ++ coverage;
        try {
            throw 123;
        } catch( int j ) {
            if( j != 123 ) {
                fail(__LINE__);
                return;
            }
            goto label;
        } catch( X ) {
            fail(__LINE__);
            return;
        } catch( ... ) {
            fail(__LINE__);
            return;
        }
    }
label:
    ++ throws;
    throw X(-35);
}

void test( int i ) {
    try {
        f( i );
    } catch( X x ) {
        if( x.sig != -35 ) fail(__LINE__);
        ++ catches;
    } catch( ... ) {
        fail(__LINE__);
    }
}

int main() {
    int ctr;
    for( ctr = 0; ctr < 24; ++ctr ) test( ctr );
    if( coverage != ctr ) fail( __LINE__ );
    if( throws != ctr ) fail( __LINE__ );
    if( catches != ctr ) fail( __LINE__ );
    _PASS;
}

#else

ALWAYS_PASS

#endif
