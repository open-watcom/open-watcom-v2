Version 10.0 Level C Patches
****************************


::::94/09/06 (AFS)

    Allow:
    
        extern "C" int __cdecl x;
        
    Must be extern "C" for __cdecl to take effect since variables
    have their type mangled into the name for "C++" linkage.

    Work-around: fabricate a #pragma that sets the underscore


===========================================================
::::94/09/06 (AFS)

    Removed warning for "always true/false" expressions if the
    sub-expressions are constant values.
    
    Work around: #pragma warning 689 10
                 #pragma warning 690 10

===========================================================
::::94/09/07 (AFS)

    Added support for:
    
        #pragma pack(push,4);
        #pragma pack(push);
        #pragma pack(pop)
    
===========================================================
::::94/09/07 (AFS)

    Added support for:
        #pragma comment(lib,"mylib.lib")
        
    same semantics as:
        
        #pragma library( "mylib.lib" )
        
    
===========================================================
::::94/09/07 (AFS)

    Added support for expanding macros in code_seg/data_seg
    pragmas:
        
        #define DATA_SEG_NAME "MYDATA"
        #define CODE_SEG_NAME "MYCODE"
        
        #pragma data_seg( DATA_SEG_NAME )
        int x = 3;
        
        #pragma code_seg( CODE_SEG_NAME )
        int fn() {
            return x;
        }
    
===========================================================
::::94/09/08 (AFS)

    Fixed the 16-bit compiler so that it matches the MS 16-bit
    C compiler for the following cases:
    
        if a pascal function is defined when compiling for
        Windows, use the fat Windows prologue in the function
        
        if a cdecl function is defined when compiling for
        Windows, use the fat Windows prologue in the function
    
===========================================================
::::94/09/14 (AFS)

    Fixed compiler so that #include </dir/file.h> works
    as expected (was searching along INCLUDE path only).
    
===========================================================
::::94/09/15 (AFS)

    Fixed a problem where an import was generated in the
    object file for a virtual function call.  This will
    reduce the size of executables under certain circumstances.
    
===========================================================
::::94/09/15 (AFS)

    Removed prohibition of pointer to array of unknown size
    declarations.
    
    Example:
    
        int (*p)[];
    
===========================================================
::::94/09/25 (AFS)

    Fixed diagnosis of lexical problems during macro
    expansion to remove spurious warnings.

    Example:
    
        #define stringize( x )  #x
        
        stringize( 2131231236172637126371273612763612731 )
    
===========================================================
::::94/10/03 (AFS)

    Corrected check for too many bytes in #pragma for
    assembler style aux #pragmas.
    
===========================================================
::::94/10/03 (AFS)

    Undeclared class names in elaborated class specifiers
    are now declared in the nearest enclosing non-class
    scope.  Undeclared classes are also allowed in arguments
    now.
    
    Example:
    
        struct S {
            // used to declared ::S::N but now declares ::N
            struct N *p;
        };
        
        void foo( struct Z *p );    // declares ::Z
    
===========================================================
::::94/10/07 (AFS)

    Fixed unduly harsh restriction on virtual ...-style
    functions.  They are now allowed in single inheritance
    hierarchies as long as the return type is not changed
    when the virtual function is overridden.  In multiple
    inheritance hierarchies, an implementation restriction
    is still present for generating a 'this' adjustment
    thunk for virtual ...-style functions.
    
===========================================================
::::94/10/07 (AFS)

    Fixed line # info for multi-line statment expressions
    in some weird cases.
    
===========================================================
::::94/10/11 (AFS)

    Fixed function template parsing of user defined
    conversions that use an uninstantiated class in
    their operator name.
    
    Example:
        void ack( int );
        
        template <class T>
            struct S {
                S( T x )
                {
                    ack( x );
                }
            };
        
        template <class T>
            struct W {
                operator S<T>();
            };
        
        template <class T>
            W<T>::operator S<T>() {
                return 0;
            }
    
===========================================================
::::94/10/12 (AFS)

    Fixed compiler problem that caused a linker warning
    "lazy reference for <virtual-fn> has different default
    resolutions" in cases where compiler or programmer
    optimized virtual function calls to direct calls
    in modules that also contained virtual calls.
    
    T.H
        struct S {
            virtual int foo() { return __LINE__; }
        };
        struct T : S {
            virtual int foo() { return __LINE__; }
        };
    
    T1.CPP
        #include "t.h"
        struct Q : T {
            virtual int foo() { return S::foo() + __LINE__; }
        };
        
        void foo( T *p )
        {
            Q y;
            y.foo();
            p->foo();
        }
    T2.CPP
        #include "t.h"
        
        void foo( T *p );
        
        void ack( T *p ) {
            p->foo();
            foo(p);
        }
        
        main() {
            T q;
            ack( &q );
        }
    
===========================================================
::::94/10/11 (JWW)

    When a class value is returned and is immediately (in the same
    expression) used to call a member function, the value may not 
    be stored in memory.
    
    Work around: introduce a temporary
    
    struct S {
        int v;
        int member();
    };
    
    S foo();
    
    void example( void )
    {
        // foo().member();  // replace this line with:
        S temp = foo();
        temp.member();
    }
    
===========================================================
::::94/10/12 (JWW)

    Throwing pointers to functions did not work when the
    size of a function pointer is greater than the size
    of a data pointer.
    
    Work around: place the function pointer in a class and
        throw the class object.

===========================================================
::::94/10/14 (AFS)

    Fixed default arg processing for const references to
    an abstract class.  The following example would not
    compile properly:
    
        struct A {
            virtual int foo() = 0;
        };
        
        A &foo();
        
        void ack( A const &r = foo() );
        
        void bar() {
            ack();
        }

===========================================================
::::94/10/17 (AFS)

    Made "DllMain" default to extern "C" linkage for VC++
    compatibility.

===========================================================
::::94/10/18 (AFS)

    Duplicated a VC++ language extension that is
    necessary to parse the Win95 SDK header files.
    
    Example:
    
        typedef struct S {
        } S, const *CSP;
             ^^^^^- not allowed in ANSI C or current WP for C++

===========================================================
::::94/10/19 (AFS)

    Don't warn about starting a nested comment if the comment
    is just about to end.
    
    Also fixed the code that figures out where a comment was
    started so that a nested comment warning is more helpful.
    
    Example:
    
        /*/////////*/
                   ^-
    
===========================================================
::::94/10/20 (AFS)

    Fixed problem where extra info notes were not being
    printed for the error message that exceeded the error
    message limit.
    
    Example:
    
        // compile -e2
        struct S {
            void foo();
        };
        
        void foo( S const *p )
        {
            p->foo();
            p->foo();
            p->foo();
            p->foo();
        }
    
===========================================================
::::94/10/20 (AFS)

    Fixed problem where the line # for an error message
    was incorrect.
    
    Example:
    
        struct S {
            void foo() const;
            void bar();
        };
        
        void S::foo() const
        {
            bar();
            
            this->bar();
            
        }
    
===========================================================
::::94/10/24 (AFS)

    Fixed output of browser information for instantiated
    function template typedefs.
    
===========================================================
::::94/10/25 (AFS)

    Upgrade C++ parser so that casts and member pointer
    dereferences can appear on the left hand side of the
    assignment expression without parens.
    
    Example:
    
        p->*mp = 1;
        (int&)x = 1;
    
===========================================================
::::94/10/25 (JWW)

    In several cases, when a function return or a construction
    was immediately dotted in an expression, the generated code
    was incorrect:
    
    Example:
    
        struct S {
          int x;
          int foo();
        };
        
        extern S gorf();
        
        void bar()
        {
            gorf().foo();
        }
        
    Work around:
    
        Break the statement in two:
        
            S temp = gorf();
            temp.foo();

===========================================================
::::94/10/25 (JWW)

    In several cases, when a function return or a construction
    was immediately addressed in an expression, the generated code
    was incorrect:
    
    Example:
    
        struct S {
          int x;
        };
        
        extern void fun( S* );
        
        extern S gorf();
        
        void bar()
        {
            fun( &gorf() );
        }
        
    Work around:
    
        Break the statement in two:
        
            S temp = gorf();
            fun( &temp );
            
===========================================================
::::94/11/01 (AFS)

    Added support for:
    
        #pragma error "error message"
        
    Use the ANSI method because it is more portable and
    cleaner (MS header files use the less portable
    #pragma when there is a perfectly fine portable way
    to issue a message).
    
    Portable, clean method:
    
        #error "error message"

===========================================================
::::94/11/02 (AFS)

    Added support for __declspec(dllexport) and
    __declspec(dllimport) for Win32 (i.e., WinNT 3.5
    and Win95) programs.  Here are some examples:
    
    __declspec(dllexport) int a;        // export 'a' variable
    __declspec(dllexport) int b()       // export 'b' function
    {
    }
    
    struct __declspec(dllexport) S {
        static int a;                   // export 'a' static member
        void b();                       // export 'b' member fn
    };
    
    extern __declspec(dllimport) int a; // import 'a' from a .DLL
    extern __declspec(dllimport) int b();//import 'b' from a .DLL
    
    struct __declspec(dllimport) I {
        static int a;                   // import 'a' static member
        void b();                       // import 'b' member fn
    };

===========================================================
::::94/11/03 (AFS)

    C++ compiler generates better error messages for in-class
    initializations and pure virtual functions.
    
        struct S {
            static int const a = 0;
            static int const b = 1;
            void foo() = 0;
            void bar() = 1;
            virtual void ack() = 0;
            virtual void sam() = 1;
        };

===========================================================
::::94/11/04 (AFS)

    Fixed macro processing code so that the following program
    compiles correctly.  The compiler was not treating
    "catch" as a keyword after the expansion of "catch_all".
    
        #define catch(n) catch(n &exception)
        #define xall (...)
        #define catch_all catch xall
        
        main()
        {
            try{
            }
            catch_all{
            }
        }

===========================================================
::::94/11/04 (AFS)

    Fixed a problem where #pragma code_seg caused a page
    fault in the compiler when the code_seg was empty.

===========================================================
::::94/11/07 (AFS)

    Fixed a rare problem where a #include of a file that
    was previously included caused the primary source file
    to finish up if the CR/LF pair for the line the #include
    was on, straddled the C++ compiler's internal buffering
    boundary.

===========================================================
::::94/11/07 (AFS)

    Added support for #pragma message( "message text" ).
    Outputs message to stdout when encountered.  Used
    in MS SDK header files to warn about directly including
    header files and obsolete files.

===========================================================
::::94/11/11 (AFS)

    Fixed #pragma code_seg/data_seg to properly set the
    class name of the new segment in the object file.

===========================================================
::::94/11/13 (AFS)

    Fixed a problem with the -zm -d2 options that caused
    a compiler fault in some circumstances.

===========================================================
::::94/11/17 (AFS)

    Fixed default library records in .OBJ file so that user
    libraries are ahead of default compiler libraries in
    the linker search order.

===========================================================
::::94/11/18 (AFS)

    Fixed handling of intrinsic math functions so that code
    generator will treat functions like sqrt as an operator.

===========================================================
::::94/11/18 (JWW)

    Added support for using OS-specific exception handling
    mechanisms for C++ exception handling during code
    generation.  Enable with the new -zo option.

===========================================================
::::94/11/21 (AFS)

    __stdcall functions now have MS Visual C/C++ compatible
    name mangling.  The -zz option can be used to get the
    old 10.0GA naming convention for __stdcall.

===========================================================
::::94/11/21 (AFS)

    __cdecl functions now preserve EBX for MS Visual C/C++
    compatibility.

===========================================================
::::94/11/22 (AFS)

    Fixed processing of option response files with more than
    one line in them.

===========================================================
::::94/11/22 (AFS)

    __far16 __cdecl and __pascal function definitions are
    compiled with the best register save set now (previous
    save set was conservative)

===========================================================
::::94/11/24 (AFS)

    Diagnose the following code when compiling for strict
    ANSI mode:
    
        void foo();
        static void foo();

===========================================================
::::94/12/06 (AFS)

    Fixed a case where the compiler optimized a virtual call
    to a direct call in such a way that a weak extern was
    used to import the function name thus libraries would
    not be searched for the virtual function.  The actual
    code generated was correct so if the function was resolved
    through other means, the program functioned properly.

    struct B {
        virtual void foo( int = 0 );
        virtual void bar( int );
    };
    
    void g( B x )
    {
        x.foo();
        x.bar(1);
    }

===========================================================
::::94/12/06 (AFS)

    Fixed a compiler optimization where in large code models,
    a static function was made into a NEAR function under
    certain conditions.  Inline functions (which default
    to 'static' storage class) can be generated out of line.
    If two modules have out-of-line inline functions and one
    module optimized the inline function to NEAR, then depending
    on what module is chosen by the linker to supply the out of
    line code, one of the callers does not match the function
    actually called.  The fix was to never optimize the out-of-line
    function to NEAR so that all modules agree on the calling
    convention.

    // module #1
    // compiler decides out-of-line copy is NEAR
    inline int fact( int x ) {
        if( x == 0 ) {
            return 1;
        }
        return x + fact( x - 1 );
    }
    void bar( void ) {
        int x = fact( 5 );      // calls a NEAR function
    }
    
    // module #2
    // compiler decides out-of-line copy is FAR
    inline int fact( int x ) {
        if( x == 0 ) {
            return 1;
        }
        return x + fact( x - 1 );
    }
    void sam( void ) {
        int x = fact( 5 );      // calls a FAR function
    }

===========================================================
::::94/12/06 (AFS)

    Fixed diagnosis of anonymous union member unions that
    collide with a type name in the enclosing scope.
    
    struct S {
        typedef int T;
        union { int T; };
    };

===========================================================
::::94/12/08 (AFS)

    Added support for (req'd for Win32 OLE C++ headers):
    
        void foo( int, int );
        class S {
            void foo();
            // global 'foo' not member 'foo'
            friend void ::foo( int, int );
            int m;
        };

===========================================================
::::94/12/08 (AFS)

    Correctly diagnose macro references that span multiple
    lines.

        #define f(a,b) a+b
        #if f(1,
                2)
        #endif
        
===========================================================
::::94/12/12 (AFS)

    Fixed compiler so that the common type of a 0 constant
    and a near pointer is a far pointer in large data models.
    Also applies to code pointers.
    
    e.g.,
        // -ml or -mc
        int __near i;
        int *p;
        p = ( b ? &i : 0 );  // common type was 'int near *'
        if( p == NULL ) {
        }
        
===========================================================
::::94/12/13 (AFS)

    Tweaked compiler so that if a #define of a macro is
    different than a previous definition, the new definition
    is used and a warning is emitted.  In strict ANSI mode,
    this is still a hard error.
    
    e.g.,
    
        // redefinition warning but error if strict ANSI
        #define FOO 123
        #define FOO 0xa0a
        
        int x = FOO;
        
===========================================================
::::94/12/15 (AFS)

    Fixed 16-bit compiler so that DLL exported and imported
    data defaults to far in large memory models (-mc -ml -mh).
    
    e.g.,
        // -mc, -ml, -mh
        struct __export S {
            int static a;
        };
        
        void foo() {
            S::a = 1; // access as far
        }
        
===========================================================
::::95/01/03 (JWW)

    Sometimes elimination of a copy ctor, when the copied item was
    constructor with non-default calling convention, did not work.
    
    Work around: do the copy in two steps
    
    enum E {
        A,B,C
    };
    
    struct __pascal S {
        E x;
        S( E x ) : x(x) {
        }
    };
    
    S const foo()
    {
        return S( B );  // did not work
    }
    
    Replace "foo" with
    
    S const foo()
    {
        S retn( B );
        return retn;
    }

        
===========================================================
::::95/01/08 (AFS)

    Diagnose ''' character constants as illegal.
        
===========================================================
::::94/01/10 (AFS)

    Fixed a problem with += or ++ of an enumerated type
    that could cause incorrect code to be generated or
    a compiler fault.
    
    e.g.,
    
        enum E { A, B, C };
        
        void fn() {
            E x;
            
            x++;
        }
        
===========================================================
::::95/01/10 (AFS)

    Improved the location for some diagnostics.
    
    e.g.,
    
        int fn( int far x,
                int far y )
        {
        }
        
===========================================================
::::95/01/23 (AFS)

    Added support for #pragma data_seg() and #pragma code_seg().
    
    e.g.,
        #pragma data_seg( "_MYDATA" )
        
        int x = 3;      // in _MYDATA segment
        
        #pragma data_seg() // restore default data segment
        
        int y = 4;      // in _DATA segment

===========================================================
::::95/01/23 (AFS)

    Fixed a problem with derived classes that declare
    virtual functions with the same name as a base class.
    
    e.g.,
        struct B { B(); };
        struct D : B {
            virtual B();
        };

===========================================================
::::95/01/23 (JWW)

    Sometimes the compiler missed diagnosing the impossibility
    of generating a default constructor.
    
    struct S {
        S();
    };
    
    struct F {
        int _0;
        S x;
        F(int);
    };
    
    struct B : F {
        B();
    };
    
    B::B() {            // should diagnose F() as impossible
    }                   // to create
    
    Work around: none

===========================================================
::::95/01/31 (AFS)

    Allow "operator ()" to have default arguments unless
    we are compiling for strict ISO/ANSI C++.

===========================================================
::::95/01/31 (AFS)

    Fixed compiler so that it will not rescan a class
    template if it was used in a base class reference.
    
    Example:

        template <class T> struct W;
        template <class T> struct V : W<T> {
            W<T> *p;
            short x;
            V(short x=2) : p(this), x(x), W<T>(x) {
            }
        };
        template <class T> struct W {
            V<short> *q;
            short zz;
            W( short x = 3 ) : q( 0 ), zz(x) {
            }
        };
    
        W<double> *q;
        V<short> x;
    
===========================================================
::::95/02/01 (AFS)

    Fixed compiler so that it properly diagnoses the
    following code as erroneous:
    
        int a[10];
        
        void foo() {
            a += 2;
        }
    
===========================================================
::::95/02/02 (AFS)

    Fixed compiler so that it defers the check of the
    return type of operator -> until it is used.  This
    is important for class templates.
    
    Example:
    
        template <class T>
            struct S {
                // may never be used
                T operator ->();
            };

===========================================================
::::95/02/02 (AFS)

    Fixed compiler so that it doesn't complain about
    weird identifiers in #ifndef #ifdef directives
    that are being skipped.
    
    Example:
    
        #if 0
        #ifdef id_with_a_$_in_it
        #endif
        #endif

===========================================================
::::95/02/16 (AFS)

    Fixed compiler so that virtual functions defined in
    extern "C" regions can match identical virtual functions
    in extern "C++" regions.
    
    Example:

        extern "C" {
            struct b {
                virtual int foo( int, int (*)( int ) ) = 0;
            };
        }
        struct a : b {
            virtual int foo( int, int (*)( int ) );
        };
        a x; // OK

===========================================================
::::95/02/20 (AFS)

    Removed restriction that prevented the use of '-fi'
    and '-fh' option together (Force including a file
    from the command line and using pre-compiled headers).

===========================================================
::::95/02/20 (AFS)

    Fixed handling of longjmp and setjmp for some weird
    cases where destructors had to be called to maintain
    the integrity of the exception handling stack.

===========================================================
::::95/02/20 (AFS)

    Functions that had the __saveregs attribute did not
    properly save the segment registers in the generated
    code.
    
    Example:
    
        // compile -ml
        int __saveregs foo( const char * filename)
        {
           return filename[0] == 0;
        }

===========================================================
::::95/02/21 (JWW)

    Extern "C" functions that were passed class values did not
    process those arguments correctly.
    
    Work around: recode the functions to accept pointers to the
        class values instead.

===========================================================
::::95/02/21 (AFS)

    Fixed references to extern __based(__segname("XXX"))
    variables and functions to be far references.
    
===========================================================
::::95/02/21 (AFS)

    Fixed a compiler fault problem where the effect of
    #pragma data_seg was not reset before code generation
    started.

===========================================================
::::95/02/24 (JWW)

    Compiler did not check for lvalues on left of +=, -=.
    
    Work around: None, rvalues should not be used in this context.

===========================================================
::::95/02/27 (AFS)

    Fixed compiler so that -fhq automatically implies -fh
    is used (if I want quiet pre-compiled headers then I
    probably want pre-compiled headers).
    
    Work around: use -fhq together with -fh

===========================================================
::::95/02/27 (AFS)

    Fixed compiler so that some missing information is
    present in the pre-compiled headers.

===========================================================
::::95/03/03 (AFS)

    Fixed compiler so that file names and directory names
    are compared as case insensitive since FAT/HPFS/NTFS
    are case insensitive file systems (although HPFS and
    NTFS are case preserving).

===========================================================
::::95/02/27 (AFS)

    Fixed some strict ISO/ANSI problems in preprocessing
    expressions for #if directives.

===========================================================
::::95/03/27 (JWW)

    In light of latest changes to the C++ draft standard,
    the following are no longer diagnosed:
        (1) throwing a class with ambiguous bases (the ambiguous
            bases cannot be caught)
        (2) no conversions to abstract base classes of thrown
            objects are attempted.
    
===========================================================
::::95/03/28 (AFS)

    *p++; no longer warns you about not using the value of
    the expression (the <stdio.h> putchar macro must return
    a value that is almost never used in practice; so the
    C++ compiler would warn you about this).
    
    Work-around:
    
    (void) putchar( '\n' );
    
===========================================================
::::95/04/03 (AFS)

    Fixed the compiler so that code that deals with references to
    undefined classes works properly.
    
    Example:
        struct U;
        extern U x;
        
        U &foo()
        {
            return x;
        }
        U const volatile &bar()
        {
            return x;
        }
    
===========================================================
::::95/04/03 (AFS)

    Added pre-defined macro SOMDLINK to C++ compiler for
    data references.
    
===========================================================
::::95/04/18 (AFS)

    Fixed a problem where a pre-compiled header file was
    created incorrectly if the last thing in the header
    file was a class template or member function template.
    
    Work-around:
    
    Add a dummy declaration to the end of the header file.
    i.e., extern int __work_around;
    
===========================================================
::::95/04/20 (AFS)

    Fixed a problem where the compiler was inlining a global
    function that was only called once from within the module.
    Compiler was supposed to only inline static functions
    that are called once so a check has been added.
    
===========================================================
::::95/04/20 (AFS)

    Fixed function template binding so that trivial conversions
    on identical parm and argument types are allowed.
    
    Example:
        struct Foo {
            Foo &operator <<(const char *string);
        };
        
        struct Bar {
            char *temp;
        };
        
        template <class Type>
            Type &operator <<(Type &type,const Bar &bar)
            { type << bar.temp; return type; }
        
        int temp( void )
        {
            Foo foo;
            Bar bar;
            foo << bar; // should be OK
            return 0;
        }
    
===========================================================
::::95/04/28 (AFS)

    Fixed compiler so that it knows member pointers will
    never point to "C" linkage functions.  Spurious errors
    were diagnosed if a member ptr typedef occurred in a
    extern "C" region.
    
===========================================================
::::95/05/15 (AFS)

    Fixed a problem with pre-compiled header files that
    are used in headers that overload the postfix ++/--
    operator.
    
===========================================================
::::95/05/19 (AFS)

    Allow __export to be added in subsequent declarations
    and definitions.
    
        int foo( int );
        
        int __export foo( int ) {
            return 0;
        }
    
===========================================================
::::95/05/19 (AFS)

    Fixed 386 compiler so that the module initialization
    function is properly indicated as far if the user set
    the memory model to -ml or -mm.  This will not work
    with the Watcom C/C++ run-time support since large
    code models are not supported directly for 386 code
    but it would be easy to code the appropriate code
    in embedded system applications based on the Watcom
    run-time source code.
    
===========================================================
::::95/05/25 (AFS)

    Disallow the use of enumeration constants in #pragma
    aux code bursts.
    
===========================================================
::::95/05/29 (AFS)

    Fixed -p option so that certain cases of macro expansion
    followed by text do not result in different tokens being
    scanned if the output is compiled.
    
    Example:
    
        #define macro() int
        macro()i;  // should be "int i;" not "inti;"
        void main() {
                i = 0;
        }
    
===========================================================
::::95/05/30 (AFS)

    Fixed some problems with -pl preprocessed output
    (preprocessed output with #line directives).
    Also fixed -pw=0 so that lines are not wrapped.
    
===========================================================
::::95/06/06 (AFS)

    Fixed some problems with collecting macro parms
    that are null. (e.g., foo(a,,c,,) or foo() )
    
===========================================================
::::95/06/08 (JWW)

    Fixed reference or lvalue expressions in ?: to mimic
    pointer conversions.  This is an extension to the strict
    ANSI standard but will probably be fixed in the future.
   
    Example:
    
        The code fragment

        int i;
        const int j;
        
        const int & foo( int val )
        {
            return val ? i : j;
        }
        
        previously produced incorrect results, since the ? 
        expression resulted in an rvalue because i and j had 
        different types. The updated compiler now produces
        an reference expression of type (int const &).
        
    Work around: assign to properly typed tempories:

        int i;
        const int j;
        
        const int & foo( int val )
        {
            const int & i_ref = i;
            const int & j_ref = j;
            return val ? i_ref : j_ref;
        }
        
    
===========================================================
::::95/06/12 (AFS)

    Set compiler so that if -pl is used without -pw then
    infinite line length is used so that the #line directives
    will be correct.
    
===========================================================
::::95/06/12 (AFS)

    Fixed infinite recursion error in compiler when dealing
    with some strange class definitions with errors in them.
    
===========================================================
::::95/06/15 (JWW)

    Unaccessible destructors and constructors in base classes
    were being mistakenly diagnosed.
   
    Example:
    
        class Base {
            ~Base();
        public:
            Base();
        };
        
        class Derived : public Base {
        public:
            Derived();
            ~Derived();
        };
        
        void foo()
        {
            throw Derived();
        }
        
    Work around: make the destructor/constructors accessible.
    
        class Base {
        public:
            ~Base();
            Base();
        };
        
        ...
    
===========================================================
::::95/06/27 (AFS)

    Added necessary function template support so that Watcom C++
    can compile the STL library properly.
    
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
::::95/03/27 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
