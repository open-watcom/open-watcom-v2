Version 11.0 Level D Patches (from 11.0, patch B, Classic Release)
****************************
[really changes for V11.0 C-level patch but we started recording during beta(?)]

===========================================================
::::afs 1998/04/23 10:28:50 AM

    Fixed warning about #if's and #endif's in different
    source files being confused by #line directives.
    
    #if 1
    #line 2 "a.c"
    #endif
    
===========================================================
::::afs 1998/04/30 11:08:48 AM

    Fixed C++ runtime to free typeid().name() memory
    properly. Use typeid().raw_name() if you do not
    want memory allocated and can accept names that
    have not been demangled.
    
===========================================================
::::afs 1998/08/11 02:53:03 PM

    Fixed counting of base classes in lookup code for
    class qualified lookups.
    
    Example:
	struct A {
	};
	
	struct B {
		B () { m = 0; }
		int m;
	};
	
	struct C : public A, virtual private B {
	};
	
	struct D : virtual public B, protected C {
	};
	
	struct E : public B {
	};
	
	struct F : public E, public D {
		int get_m () const { return D::m; }
	};
    
===========================================================
::::98/08/14 (AFS)

    Fixed validation of precompiled header file problem
    where a #undef XXX in the source file before the
    first #include was incorrectly checked to see if it
    existed before allowing another source file to use the
    precompiled header file.  Would cause a problem since
    the precompiled header file was created without the
    macro being defined but all users of the precompiled
    header file had to have the macro defined.  Since the
    macro #define can affect the semantics of the header
    file (by excluding/including source code), this would
    cause problems that were difficult to fix.
    
    Work around: don't use -fh on special source files that
    may #undef a macro before including the main PCH #include

===========================================================
::::98/08/14 (LMW)

    Fixed compiler GPF while writing information error 
    message for the code given below.
    
    Work around: Correct code. 
    
    Example:
    
    
	class WRampEntry;
	class WObject {
	};
	
	class WRampFile :  public WObject
	{
	    public:
		WRampFile();
		~WRampFile();
		operator WRampEntry *  [] (long int entry); // error is here
	};
	
	WRampFile::WRampFile()
	{
	    
	}
	WRampFile::~WRampFile()
	{
	    
	}
	 WRampEntry * WRampFile::operator [] (long int entry)
	{
	    
	}

===========================================================
::::afs 1998/11/19

    Added warning for #if expressions where an undefined
    identifier defaults to zero for purposes of evaluating
    the expression.  The warning is by default a level 10
    warning that can be enabled with the command line option
    -wce890 or the pragma:
    
    #pragma warning 890 1 
    
    Example:
    
    #if _PRODUCTI0N // should be _PRODUCTION
    // production mode code
    #else
    // debug mode code
    #endif
    
===========================================================
::::afs 1998/13/19

    Added warning for char constants that have a large value
    that cannot be stored in a char type.  Warning will be
    output with -wx.
    
    Example:
    
    char x = '\0x1a'; // '\x00\x78\x30\x61'
    
===========================================================

::::99/12/31 (WHO)

    Sample text goes here.
    
    Work around: text goes here

===========================================================
