Version 11.0 Level B Patches (from 11.0 Classic Release)
****************************


===========================================================
::::97/03/04 (JWW)

    When the -za switch is not specified (i.e., when
    extensions are enabled) the compiler was extended to
    accept as member pointer values, class-qualified and naked
    non-static member functions.  The proposed standard
    requires that & precede a class-qualified name.
    
        struct S {
            int foo( float );
        };
        
        int (S::*mp)( float );  // a member-pointer variable
        
        ...
        
            // the following are all accepted:
            
            mp = &S::foo;       // standard
            mp = S::foo;        // extension
            mp = foo;           // extension (must be in scope)

===========================================================
::::97/03/12 (LMW)

    Fixed a bug that caused the compiler to crash under certain 
    conditions when writing a pre-compiled header.
    
===========================================================
::::97/03/18 (AFS)

    Fixed compiler so that floating point support is loaded
    if you call a routine with the address of a double.
    This is req'd if you need to skip floating point numbers
    but never use them.
    
    e.g.,
        scanf( "%d %f %d\n", &i1, &d1, &i2 );
    
===========================================================
::::97/03/19 (AFS)

    Fixed compiler so that -dXXX="a-/ b" is allowed.
    Compiler was terminating -d macro definition at
    a -, /, or white space.
    
===========================================================
::::97/03/19 (JWW)

    Fixed problem where a MI conversion was required before
    initializing a const reference temporary.

    Example:
    
    struct A { int a; };
    struct C { int c; };
    struct B : public C, public A { int b; };
    
    typedef A * T;
    
    T check;
    
    struct Array {
        void Append( const T & t ) { if( t != check ) _fail; }
        void Remove( const T & t ) { if( t != check ) _fail; }
    };
    
    void Foo( B * b )
    {
        Array       arr;
        A *         a = b;
        arr.Append( a );
        arr.Remove( b );
    }
    
    int main()
    {
        B   b;
        check = &b;
        Foo( &b );
        _PASS;
    }
    
===========================================================
::::97/03/19 (JWW)

    Fixed problem where direct calls to __stdcall dtors
    were incorrectly routed through a calling convention
    thunk normally used for table-driven destruction.
    
===========================================================
::::97/03/19 (AFS)

    When generating preprocessed output, the compiler now
    eliminates empty lines if you do not want #line
    directives.
    
===========================================================
::::97/03/24 (AFS)

    Correctly handle multibyte characters with the second
    byte equal to 0x5c ('\\') when they terminate a C++
    //-style comment (i.e., do not treat them as a line
    splice).
    
    Example:
    
    // <0x81><0x5c>
    
===========================================================
::::97/03/27 (AFS)

    A function that calls setjmp will have its local
    variables marked as "stay in memory" but this
    attribute incorrectly affected function template
    generic type binding.
    
    Example:
    
        #include <setjmp.h>
        
        template <class T>
            void delete_object( T*& object ) {
                if( object ) {
                    delete object;
                    object = NULL;
                }
            }
        
        struct CValue {
            int i;
        };
        
        void foo() {
            CValue *v;
            jmp_buf b;
        
            setjmp(b);
            delete_object(v);
        }
    
===========================================================
::::97/04/03 (AFS)

    Fixed problem with C++ empty base optimization that
    caused spurious errors to be diagnosed.

    Example:
    
        struct CClassBase {
        };
        
        struct CClass2 : public CClassBase {
            struct CClass1 {
                CClass1(int) {}
            };
            CClass1 m_c;
        protected: 
            CClass2(void);
        };
        CClass2::CClass2(void)
            : m_c(1) {
        }
    
===========================================================
::::97/04/11 (AFS)

    Fixed problem with macro expansion that triggered
    boundary conditions in the compiler which in turn
    caused incorrect macro expansion.

    Example:
    
        #define example( b ) b ## _X Y_ ## b ## _Z
        example( x123456789012345678901234567890 )
        example( x12345678901234567890123456789 )
        example( x1234567890123456789012345678 )
        example( x123456789012345678901234567 )
        example( x12345678901234567890123456 )
        #undef example
    
===========================================================
::::97/04/15 (AFS)

    Fixed problem with error recovery in complex
    source code with __stdcall classes.
    
===========================================================
::::97/04/20 (JWW)

    Return expressions with comma operators sometimes were compiled
    incorrectly.
    
    This resulted in a bad library for multi-threaded Netware when
    exceptions were used.
    
    Work around: assign the expression to a temporary and then
    return the temporary.
    
        example:
        
            int x;
            
            void foo( int y ) {
                return ( x = 1 , y + 1 );
            }
            
        replace by:
        
            int x;
            
            void foo( int y ) {
                int temp = ( x = 1 , y + 1 );
                return temp;
            }
    
===========================================================
::::97/04/23 (AFS)

    If a source file doesn't end in a new-line, the C++
    compiler now compensates for this to allow new-line
    terminated preprocessing directives to work properly.
    
    Example:
    
        #define C 3 // file doesn't end in '\n'
    
===========================================================
::::97/03/12 (WHO)

    Incorrect code was generated for an argument to an ellipsis
    function (one declared with '...' ) which was computed as an
    lvalue class value for a destructible class.
    
    Example:
    
        void foo( int, ... );
        
        class S {
            . . .
            ~S();
            . . .
        };
        
        S* p1;
        S* p2;
        bool b;
        
        . . .
        
            int i = foo( 1997, b ? *p1 : *p2 );
    
    Work around: use a cast to force underlying expressions
        to be rvalues:
        
            int i = foo( 1997, b ? (S)*p1 : (S)*p2 );

===========================================================
::::97/05/16 (AFS)

    Compiler would crash when it could not generate RTTI
    info into a 64k segment (only applies to 16-bit targets).
    The error was reported properly but recovery resulted
    in a memory fault in the compiler.
    
===========================================================
::::97/05/16 (AFS)

    Compiler would crash (infinite loop or page fault) when
    inlining functions that contained certain usages of
    anonymous unions.

    Example:
    
        inline int ii( int i ) {
            union {
                int x;
                unsigned char y;
            };
            x = 0;
            y = i;
            return x;
        }
        ...
        ii( q );
    
    Work around: #pragma inline_depth(0)

===========================================================
::::97/05/30 (AFS)

    Tweaked scanning of pp-number tokens in non-ISO/ANSI mode
    so that old code continues to compile correctly.
    
    Example:
        #define A10     3
        #define A11     0x1e+A10
        int v = A11;    // 0x1e + 3

===========================================================
::::97/05/30 (AFS)

    Added -mfi option to indicate to the compiler that interrupt
    functions can assume the flat memory model is preserved.
    If this option is not used, interrupt functions have to
    assume that a different stack segment is being used and
    so auto data must be manipulated as far data.
    
===========================================================
::::97/06/03 (AFS)

    Fixed diagnostic of 'void' expression types in certain
    contexts.

    Example:
        
        void a();
        int __u( int i ) {
            return i == a() || a() == i;
        }
    
===========================================================
::::97/06/03 (AFS)

    Added diagnostic to identify cases where the calling
    convention of a virtual function override has changed.

    Example:
        
       struct B {
            void __stdcall foo( int, int );
       };
       struct D : B {
            void foo( int, int );
       };
    
===========================================================
::::97/06/05 (JWW)

    A function such as
        Composed* ctor( Composed const & src )
        {
            AnotherDtorable stuff_1;
            AnotherDtorable stuff_2;
            AnotherDtorable stuff_3;
            Composed* ptr = new( some_data ) Composed( src );
            return ptr;
        }
    where AnotherDtorable and Composed are destructable objects could cause
    the object located by ptr to also be destructed under some
    combinations of options.
    
    Work around: place the new statement in a separate function.

===========================================================
::::97/06/05 (JWW)

    The following code generated a spurious warning, claiming that
    fn was unreferenced.
        extern void import( const void * );
        static void fn() {}
        void external()
        {
            const void * foo = fn;
            import( foo );
        }
        
    Work around: ignore that warning or disable it
    
===========================================================
::::97/07/03 (AFS)

    Diagnose extra initialization arguments for parenthetical
    initialization of non-class types.
    
    Example:
	int h(1);
	
	int f(1,2,3);
	
	int g( int(1,2,3) );
	
	foo() {
	    int q = 1, // <- notice comma
	    f(1,2,3);
	}

===========================================================
::::97/07/03 (AFS)

    Diagnose definition of member functions that are not
    declared but match the signature of another member
    function with default arguments.

    Example:
    
	struct A {
	    void f1( int, bool = false );
	};
	
	void A::f1( int ) {	// error
	}

===========================================================
::::97/07/03 (AFS)

    Corrected problem where a inline template function
    was incorrectly diagnosed as undefined if it was
    inside a namespace.

    Example:
    
	namespace alpha {
	    template <class T>
		inline T & f (T & a) {
		    return a;
		}
	    inline long b ( long a ) {
		return f ( a );
	    }
	}
	
	void fn( void ) {
	    long a = 100;
	    long c;
	    
	    using namespace alpha;
	    
	    c = b ( a );
	}

===========================================================
::::97/07/09 (JWW)

    Corrected problems where certain class hierarchies
    caused problems in generating construction/destruction
    code or exception handling state tables.  The problems
    manifested themselves as incorrect code/data or compiler
    faults.

    Example:
    
	#include <fstream.h>
	#include <stdlib.h>
	#include <iostream.h>
	
	class TextFile {
	   public:
	   ifstream solidTextFile;
	};
	class SolidTextFile : public TextFile {
	};
	
	
	void main() {
	    SolidTextFile aFile;
	}

===========================================================
::::97/07/10 (LMW)

    Corrected problem where, in copy initialization, a 
    user-defined conversion function from source class to a 
    non-const reference of the target class would not be 
    preferred over another ambiguous conversion.  

    In this example, the conversion A::operator String() going
    to String::String( String const & ) should be preferred over 
    the ambiguous use of the String::String( char ) constructor
    from either A::operator int() or A::operator float().
    
    Example:
    
	struct String {
	    String();
	    String( char );        
	};
	
	struct A
	{
	    operator String() const;
	    operator int() const;
	    operator float() const;
	};
	
	struct B
	{
	    const A& GetitsA();
	};
	
	void foo(B b)
	{
	    String s;
	    s = b.GetitsA();
	}
	    

===========================================================
::::97/07/24 (AFS)

    Fixed reading and writing of the precompiled header
    file when it contained C++ EH information for classes.

===========================================================
::::97/06/05 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
