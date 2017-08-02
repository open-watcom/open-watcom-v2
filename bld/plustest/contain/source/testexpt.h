#define test_except( code, except, string )			\
    try{							\
	code;							\
	cout << "supposed to happen exception not thrown\n";	\
    } catch( WCExcept::failure oops ) {			\
        WCExcept::wc_state cause = oops.cause();	\
        if( cause & WCExcept::except ) {			\
            cout << string << " (supposed to happen)\n";	\
        } else {						\
	    cout << "wrong except\n";				\
	}							\
    } catch( ... ) {						\
	cout << "should not get here!\n";			\
    }

#define not_happen_except( code, string )			\
    try{							\
	code;							\
    } catch( WCExcept::failure oops ) {			\
        WCExcept::wc_state cause = oops.cause();	\
        if( cause & WCExcept::check_all ) {		\
            cout << string << " (supposed to happen)\n";	\
	}							\
    } catch( ... ) {						\
	cout << "should not get here!\n";			\
    }
