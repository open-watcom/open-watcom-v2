Version 10.5 Level A Patches
****************************

!!! OBSOLETE use v10p5b.pat !!!


===========================================================
::::95/07/20 (JWW)

    Main procedure was not returing zero when it was declared using
    a default int and no return statements were given in the function.
    
    Work around: insert a "return 0;" statement(s) in the main
        procedure.

===========================================================
::::95/07/18 (AFS)

    Fixed processing of class inline function definitions
    so that inline functions are deferred properly in
    base class declarations.

    Example:
	template <class T>
	    struct equiv {
		friend bool operator ==( const T &l, const T &r ) {
		    return l.eq( r );
		}
	    };
	
	struct X : equiv<X> {
	    bool eq( X const &m ) const {
		return sig == m.sig;
	    }
	};
    
===========================================================
::::95/07/25 (AFS)

    Fixed a spurious diagnostic when dealing with protected
    functions with default arguments.
	
    Example:
	class X {
	protected:
	    void foo(char *p = 0 );
	};
	class Y;
	class Z {
	    void bar( Y * );
	};
	class Y : public X {
	    friend class Z;
	};
	void Z::bar( Y *p ) {
	    p->foo();
	}
    
===========================================================
::::95/07/25 (AFS)

    Added some more MSC++ compatible pre-defined macros.
    
    - _M_I86?M for memory models
    - __WINDOWS__/_WINDOWS if -bt=windows is enabled
    - __DOS__/_DOS if -bt=dos is enabled
    
===========================================================
::::95/07/25 (AFS)

    Allow "new __far char[10]" if default memory model is
    large data.
    
===========================================================
::::95/07/31 (AFS)

    Fixed a problem in pre-compiled header files when dealing
    with __based(__segname("_DFGH")) modifiers.
    
===========================================================
::::95/08/01 (AFS)

    Fixed a compiler fault when using external data
    with __based(__segname("_DFGH")) modifiers.
    
===========================================================
::::95/08/01 (AFS)

    Fixed a problem with inline asm code causing the compiler
    to think that a variable was initialized.
    
    Example:
    
    	extern int a;
	void f( int );
	#pragma aux f = "mov a,ax" parm caller [ax];
	int a = 3; // bug caused this to be an error!
    
===========================================================
::::95/08/01 (AFS)

    Fixed a problem with lazy references with different
    default resolutions (caused a WLINK warning).
    
===========================================================
::::95/08/10 (AFS)

    Allow -zo to work in non-flat (i.e., -mf) 386 memory models.
    
===========================================================
::::95/08/24 (AFS)

    Fixed a problem with class value default arguments that
    initialize a const reference combined with -d2 or no
    inlining.
    e.g.,  void foo( S const &r = S(0) );
    
===========================================================
::::95/08/30 (AFS)

    Fixed a problem with __stdcall WinMain() and the -zz
    option that caused an incorrect reference to be
    generated in the object file.
    
===========================================================
::::95/08/30 (AFS)

    Fixed a problem where a cast to a reference type of
    a class with a UDC generated incorrect code.
    e.g.,
    	struct S { operator int &(); };
	S v;
	((int&) v) -- doesn't call op int&()!
    
===========================================================
::::95/07/20 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
!!! OBSOLETE use v10p5b.pat !!!
