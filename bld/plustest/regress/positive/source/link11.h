template <unsigned n>
    struct TC {
	unsigned v;
	inline TC( void );
	unsigned inline sig( void );
    };

template <unsigned n>
    TC<n>::TC( void ) : v(n) {
    }

template <unsigned n>
    unsigned TC<n>::sig( void ) {
	return v;
    }
