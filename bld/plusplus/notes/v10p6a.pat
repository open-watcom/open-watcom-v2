Version 10.6 Level A Patches
****************************

===========================================================
::::96/08/20 (AFS)

    Fixed code generated for a virtual base adjustment in
    -d2 -mc code that caused a spurious load of a word
    just past the end of the virtual base offset table.
    Code functioned properly as long as the virtual base
    offset tables wasn't at the end of a segment in which
    case the code faulted.

===========================================================
::::96/08/16 (AFS)

    Fixed a problem where a #include of an absolute pathname
    did not produce a diagnostic if the file didn't exist.

===========================================================
::::96/08/16 (AFS)

    Corrected segment alignment so that if a segment includes
    aligned doubles, the doubles will be aligned.

===========================================================
::::96/07/16 (AFS)

    The following code now generates a warning rather than
    a hard error since some OS vendors ship header files
    with this incorrect C++ construct in them.

    typedef struct S {
    };

===========================================================
::::96/07/16 (AFS)

    Following code is now parsed properly:
    
    typedef int T;
    struct S {
        unsigned T : 1;
    };

===========================================================
::::96/07/16 (AFS)

    Fixed a crash caused by complex macro expansions.

===========================================================
::::96/07/12 (AFS)

    Fixed preprocessing of the following source:
    
        #define P(x) x
        
        extern int get P((enum aba a));
        
        extern int get P((enum aba
        a,
        void *b));
        
    when the source has LF instead of CR/LF line delimiters.

===========================================================
::::96/04/02 (AFS)

    Fixed overload ranking to ignore access and ambiguity
    when ranking derived classes.

===========================================================
::::96/04/15 (AFS)

    Fixed #line nnn "ssss" for cases where the "sss" is
    immediately followed by newline

===========================================================
::::96/04/09 (AFS)

    Fixed compiler so that a runtime call to
    destruct/destruct_all after a setjmp is not emitted
    if there is nothing to destruct in the current function.

===========================================================
::::96/04/05 (AFS)

    Fixed #pragma warning nnn level to verify nnn is a valid
    error message number.

===========================================================
::::96/04/11 (AFS)

    Changed parser disambiguation warnings to level 9.

===========================================================
::::96/04/24 (AFS)

    Fixed PCH handling for static destructible objects.

===========================================================
::::96/04/24 (AFS)

    Fixed multiple #include of a file of the form:
    
    #if 1
    #if 0
    #endif
    #endif
    
    (would report missing #endif)

===========================================================
::::96/04/29 (AFS)

    Fixed PCH handling for function prototypes instantiated
    from function templates.

===========================================================
::::96/08/14 (JWW)

    Was not converting properly class rvalues returned from
    functions, when the class has constructors or a
    destructor, when the temporary value used as an argument
    in a function call.
    
    Work around: Eliminate the temporary by assigning the
    result from the function call to an automatic variable.

===========================================================
::::95/12/13 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
