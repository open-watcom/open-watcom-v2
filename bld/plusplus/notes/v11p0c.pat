Version 11.0 Level C Patches (from 11.0, patch A, Classic Release)
****************************


===========================================================
::::97/08/21 (JWW)

    An error was missing in the following case:
        extern void f( int a, char *b );
        extern void f( int a );
        void *pf = f;
    
    Work around: none

===========================================================
::::97/08/21 (JWW)

    Anonymous structs or classes within unions could not always
    be initialized properly.
        struct s
        {
            union
            {
                struct
                {
                    float a, b;
                };
                float c[2];
            };
        };
        s d = { 0.0, 0.1 };
    
    Work around: Set values in object manually.

===========================================================
::::97/08/25 (AFS)

    Output a warning if a #endif isn't in the same source
    file as its companion #if.
    
    Example:

    --- foo1.h ---
    #ifndef FOO1
    #define FOO1
    #endif
    #endif	// warning
    --- foo2.h ---
    #ifndef FOO2
    #define FOO2
    #include "foo1.h"
    #endif
    --- foo.c ---
    #include "foo2.h"

===========================================================
::::97/08/21 (LMW)

               A
              /
         D   B
          \ /
           C
	   
    We no longer prefer the conversion from C->D over the conversion 
    from C->A as the number of levels of conversion does not matter 
    when D and A are not related.   

===========================================================
::::97/10/07 (AFS)

    Added level 3 warning for unary '-' of an unsigned
    operand (result type is still unsigned). Cast operand
    to signed type to remove warning.
    
    Example:
    
    void bar( int );
    void bar( unsigned );
    ...
    // selects bar( unsigned )
    foo( -2147483648 );

===========================================================
::::97/10/07 (AFS)

    Improved PCH reading of error message levels so that
    header file changes are added to current message
    levels rather than performing a complete replace of
    the error message levels and ignoring any command
    line adjustments.
    
===========================================================
::::97/10/15 (AFS)

    Fixed problem with -d2s that caused run-time error
    "undefined constructor or destructor called!" to occur.
    
===========================================================
::::97/10/15 (JWW)

    Was not generating correct code for some default functions
    in classes with modifiers.
    
    Work around: write definitions for required constructors,
    	destructors, and operator = functions

===========================================================
::::97/10/15 (JWW)

    Was not generating correct code for the default operator =
    when the class contained an array of elements where an element
    could be byte-for-byte copied and also had virtual functions.

===========================================================
::::97/10/20 (JWW)

    Destruction sometimes was omitted following a delete which
    followed a block closure which contained a destruction.
    
    Work around: introduce a destructable temporary before
    the delete.

===========================================================
::::97/10/21 (AFS)

    Fixed scanning of large strings with escaped characters.
    
    Example:
	
	char *p = "\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\
	\n";

    Work around: break string up into smaller strings and
    let compiler concatenate them
    "1234567890" --> "12345" "67890"

===========================================================
::::97/10/23 (AFS)

    Added warning from scanner whenever a trigraph
    is processed.
    
    Example:
	
	// string expands to "(?]?~????"!
	char *e = "(???)???-????";
	// work around:
	char *f = "(" "???" ")" "???" "-" "????";
	char *g = "(\?\?\?)\?\?\?-\?\?\?\?";

===========================================================
::::97/10/30 (AFS)

    Fixed analysis of shift operators with regard to
    result type.  Previously the result type was the
    same as other binary arithmetic operators.  Now
    the result type is the promoted type of the left
    hand operand.
    
    Example:
	
	int f( int x, unsigned s ) {
	    // should do signed right shift
	    return x >> s;
	}

===========================================================
::::97/11/02 (AFS)

    Compiler incorrectly warned that new/delete operators
    were declared in a namespace when they were declared
    as friends of a class.

    Example:
	
	class N {
	    friend void *operator new( unsigned );
	    friend void operator delete( void * );
	    friend void *operator new[]( unsigned );
	    friend void operator delete[]( void * );
	};
	
    Work around: disable warning

===========================================================
::::97/11/03 (AFS)

    Compiler incorrectly diagnosed an invalid constant
    when inside a preprocessing #if region that should
    have been excluded from such errors.

    Example:
	
	#if 0
	Version 35I63
	#endif
	
    Work around: put constant in a comment (if possible)
    
===========================================================
::::97/11/13 (LMW)

    Compiler now correctly evaluated all preprocessor 
    expressions involving binary operators of the same
    precedence.

    Example:
	
	#if (0 < 2 - 1 + 3)

    now correctly evaluates to TRUE.  It previously 
    evaluated as if the expressions was 
    
    ( 0 < 2 - ( 1 + 3) )  	
    
    Note: this bug only occurred in preprocessor expressions.

===========================================================
::::97/11/13 (AFS)

    Compiler now initializes member pointers at link-time
    in more cases than before (notably array initialization).

===========================================================
::::97/11/13 (AFS)

    Constant folding of 64-bit arithmetic would incorrectly
    warn about overflow if the expression involved two 32-bit
    quantities multiplied together to produce a 64-bit quantity.

===========================================================
::::97/11/23 (AFS)

    wpp: -zff and -zgf now generate code that doesn't save
    fs/gs in the prologue/epilogue of the function (matches
    behaviour of C compiler and ds/es).  The pop of fs/gs
    would fault if the selector had been freed.

===========================================================
::::97/12/08 (AFS)

    Fixed error recovery for the following code fragment:
    
    template <class T : public Base > struct S {};

===========================================================
::::97/12/08 (AFS)

    wpp: optimized -3 code to use size override prefixes
    when dealing with push and mov immed instructions.

===========================================================
::::97/12/08 (JWW)

    Under several combinations of options ?: did not generate
    proper code to select static member functions.
    
    	typedef void (*void_func)();
    
	class A
	{
	  public:
	     static void f();
	     static void g();
	};
	
	void_func func(bool b)
	{
	    return b ? A::f : A::g;
	}
    
    Work around: Avoid use of ?:, for example,
	
	void_func func(bool b)
	{
	    if( b )
	    	return A::f;
	    else
	    	return A::g;
	}

===========================================================
::::98/01/12 (AFS)

    Fixed a problem where the compiler went into an infinite
    loop generating Dwarf debug information with namespaces
    that contained typedefs referencing types in other
    namespaces.
    
    Work around: use -d3 instead of -d2
    
    Example:
	class A {
	public:
	    virtual ~A() {}
	};
	
	class B {};
	
	namespace N {
	    class C {};
	
	    typedef B T;
	
	    class D {
	    public:    
		virtual ~D() {}
	    };
	    
	    class E : public D, public A {
	    public:
		E() {}
		B* getB() {
		    return 0;
		}
		C* getC() {
		    return 0;
		}
	    };
	}

===========================================================
::::98/02/18 (AFS)

    Fixed a problem where function template instantiations
    were not being set to C++ linkage.
    
    Work around: don't use function templates in extern "C"
    		 blocks or functions
    
    Example:
	template <class T>
	    void tf(T) {
		return;
	    }
	
	struct A { } a;
	
	struct B { } b;
	
	extern "C" void bar() {
	    tf(a);
	    tf(b);
	}

===========================================================
::::98/03/02 (AFS)

    Allow bool op= bool expressions
    
    Work around: break up expression A op= B into A = A op B
    
    Example:
	void foo( bool a, bool b ) {
	    a |= b;
	}

===========================================================
::::98/03/02 (AFS)

    Fixed compiler GPF in situations where an abstract class
    tries to create or return itself.  The GPF was caused by
    the compiler trying to output a good error message with
    all the unimplemented abstract methods.
    
    Example:
	class A {
	    virtual ~A() { }
	    virtual void *fn( char *name ) = 0;
	    A Next() { return( _next ); }
	};
	void foo( A *it )
	{
	    it->Next();
	}

===========================================================
::::97/10/15 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
