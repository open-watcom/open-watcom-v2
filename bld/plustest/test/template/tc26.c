template <class DD,class Z>
    struct S {
	DD a;
	void foo( DD, Z x );
    };

void ack( double * );

template <class Z,class II>
    void S<Z,II>::foo( Z, II t )	// 'Z' and 'II' should always be visible
    {					// Borland lets 'DD' peek through also
	DD q = t;			// CFRONT & MetaWare do not allow this
	Z aa = t;

	ack( &aa );
	a = q;
    }

S<double,int> q;
