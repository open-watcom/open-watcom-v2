Version 10.5 Level B Patches
****************************



===========================================================
::::95/09/15 (AFS)

    Fixed processing of -fhq option to match the
    C compiler.  Added option '=' to -t option.
    
===========================================================
::::95/09/15 (AFS)

    Fixed some #line problems.
    
    (1) if "file" string contained \xhhh or \0ddd, the line
        number would get set to an incorrect value
    (2) if -plc option is used, #line in output for
        #line nn "file" input was one line off
    
===========================================================
::::95/09/27 (AFS)

    Fixed compiler so that some warnings and errors in
    preprocessing directives would only come out if they
    were not being skipped.
    
    Example:
        // no errors or warnings should be emitted
        #if 0
        #if 0
        #% // not a directive
        #else junk
        #endif more junk
        #endif
    
===========================================================
::::95/09/27 (AFS)

    Fixed generation of debugging info for functions that
    follow a class template member function.
    
    Example:
        template <class T>
            struct S {
                int foo( T );
            };
        
        template<class T>       // debugger thought that
            int S<T>::foo (T n) // 'main' started here!
            {
                return n << 1;
            }
        
        main() {
            static int i;
        
            while( i < 10 ) {
                ++i;
            }
            while( i < 20 ) {
                ++i;
            }
            while( i < 40 ) {
                ++i;
            }
            return i;
        }
    
===========================================================
::::95/10/04 (AFS)

    Added _STDCALL_SUPPORTED macro to 386 C++ compiler.
    
===========================================================
::::95/11/14 (AFS)

    Fixed __declspec(dllimport) classes when a virtual
    function table is laid out in memory.
    
===========================================================
::::95/11/15 (AFS)

    Fixed address of __declspec(dllimport) names so that
    they are not statically initialized.
    
===========================================================
::::95/11/20 (AFS)

    Fixed output of browser information for
    __based(__segname("_XXX")) types.
    
===========================================================
::::95/12/13 (JWW)

    Throw of 0 was not being caught by a catch block with
    int.  
    
    Work around: Re-code to not use 0.

===========================================================
::::96/03/04 (AFS)

    Fixed initialization of anonymous structs.
    
===========================================================
::::96/03/04 (AFS)

    Fixed handling of specific instantiations of class
    templates after the class has been declared.
    Example:
	template <class T>
	    struct S;
	
	void foo( S<char> );
	
	struct S<char> {
	    S<char> *p;
	    char d;
	};
	struct S<int> {
	    S<int> *p;
	    int i;
	};
	
	template <class T>
	    struct S {
		T x;
	    };
	
	void ack( S<char> & x ) {
	    foo( x );
	}
    
===========================================================
::::95/12/13 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
