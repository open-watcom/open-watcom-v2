Version 11.0 Level A Patches
****************************


===========================================================
::::96/08/07 (PLM)

    Fixed a minor bug that caused spurious warnings to be
    generated for pre-compiled headers containing integer
    constants.
    The following example, if used in a pre-compiled header,
    would cause a warning about truncating integer constants:
    
        class Foo {
            public:
                int Bar(int, int=1);    // warning generated here
        };
    
===========================================================
::::96/08/13 (JWW)

    Was mis-diagnosing conversions from types of the form
        Type cv1 * cv2 &
    to types of the form
        Type cv3 * cv4 &
    when cv3,cv4 are more qualified than cv1,cv2 respectively
    and cv4 does not contain "const" (it must for the
    conversion to be legal).
    
    Work around: none, the conversion is illegal.

===========================================================
::::96/08/14 (JWW)

    Changed the alignment of doubles and long doubles to be
    on 8-byte boundaries when not optimizing for space. This
    will increase performance on many hardware configurations.
===========================================================
::::96/09/11 (JWW)

    Incorrect code was being generated for "?:" sequences
    when the target is a class object and when the expression
    between the "?" and the ":" was converted to the type of
    the expression following the ":"
    
    work around: cast the expression between the "?" and the
                 ":" to the type of the expression following
                 the ":"
                 
    example:
    
        struct Cls {
            Cls( int );
            Cls( Cls const & );
        };
        
        extern Cls target;
        
        void foo( bool test, int int_val, Cls const & class_val )
        {
            target = test ? int_val : class_val; // generated bad code
        }
        
     The work-around is to re-code foo as follows:
        
        void foo( bool test, int int_val, Cls const & class_val )
        {
            target = test ? Cls( int_val ) : class_val;
        }

===========================================================
::::95/03/27 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
