// BADCLAS.C -- TEST ERROR HANDLING FROM CHAPTER 9
//
// 91/11/21 -- Ian McHardy	-- defined


#define INIT			// inititializers
#define OPERATOR		// operators
#define COLON_COLON		// ::
#define FRIEND		// friend functions
#define MEM_FUNC		// member function
#define VIRTUAL

// 9.1

struct S1 { int a; };
struct S2 { int a; };
struct S2 { int a; };		// double defn
struct S2 { double d; };	// different defn
#ifdef INIT
struct S3{			// stuff for ambiguous operator/function
    S3( int );			// 		and constructor/function
#ifdef OPERATOR
    operator S1();
#endif
};

int S3( int a ){ a=a; };

#ifdef OPERATOR
    int S1( S3 s ){ s = s; };
#endif
#endif

struct S3_1{			// stuff for ambiguous class/function
    const volatile unsigned long int cvuli;
};

S3_1 gs3_1;

int S3_1( void );

S3_1 gs3_1_new;			// which S3_1?

// 9.2

struct S4{
    int i;
    float i;			// can't have members with same name
    unsigned long int a;
    a( int );			// ditto
};

#ifdef COLON_COLON		// can't add members out of class decl
S4::new_member( int a ){ a = a; };
#endif // COLON_COLON

#ifdef INIT
struct S5{
    char c = 'a';		// can't initiailize stuff
    static int i = 123;
    const long = 6543210;
    auto unsigned char c2;	// can't specify linkage or registor/auto
    extern signed int i2;
    register r;
    array[];			// arrays must have size and type
    wrong;
    int (*f)() = 0;		// can't init member data
    S5 bs5;			// can't have class being defined as member
    				// data
};
#endif // INIT

struct S6{
    int i;
    enum{ e = sizeof( S6 ) };	// don't know size until after decl
#ifdef MEM_FUNC    
    int f( int i = sizeof( S6 ); );	// ditto
    int g( int i = sizeof( S6 ); ){};
#endif
};

struct S7{
    char ca[ ];			// gotta have all array dimensions
    int ia[ 4 ][ ][ 16 ];
    unsigned ua[ 12 ][ 256 ][ ];
    static float f[ ];		// OK
};

struct S8{
    static volatile unsigned char far *const far *some_ptr[ 23 ];// OK
    enum S8{ zero, one };	// static data member, enumerator, member of
    enum some_enum{ S8, b };	// an anonymous union, or nested type may
    union{			// not have the same name as its class
	int i;
	unsigned long S8;
    };
};

struct S8_1{	 
    class S8_1{ public: int a; };
};

struct S8_2{
    struct S8_2{ unsigned far *const volatile* p; };
};

struct S8_3{
    union S8_3{
	int a;
	signed b;
    };
};
// 9.3
#ifdef MEM_FUNC
struct S9{
    int a;
    int b( int );
    int c( int );
    extern void d( void );
};

short S9::funct( float f )	// not a member of S9
{
    return( f );
}

int S9::c( int a ){ return( a ); }
int S9::c( int a ){ return( a ); }// redeclaration of S9::c
class S10: public S9;		// error: declaration including base class
static S10 s10;
// 9.3.1

struct S11{
    int h() const;		// OK
};

//int S11::h() const{ return a++; };// modifying const ****************

#ifdef INIT
struct S12{
    const S12( void ){};	// constructor and destructor can't be const
    const ~S12( void ){};	// or volatile
};
 
struct S13{
    volatile S13( void ){};	
    volatile ~S13( void ){};
};
    
struct S14{
    const volatile S14( void ){};
    volatile const ~S14( void ){};
};    
#endif // INIT

// 9.3.2
struct S15{
    void g();
//    void g(){ junk };		// can't redeclare function within class ********************
    void h();
};

// 9.4

struct S16{
    int a;
    static void func( int a ){ if( this->a == a )a = a; };// static function
    				// has no this ptr
#ifdef VIRTUAL				
    virtual static func2( void ){};// can't be virtual static
#endif
    volatile unsigned long func3( int *p ); 
    static volatile unsigned long func3( int *p );// re/different declaration
};

#ifdef COLON_COLON
    static float far *S16::b[ 17 ];	// can't add members
#endif // COLON_COLON

#endif // MEM_FUNC
#ifdef INIT
struct S17{
    static int x1 = 1;		// no initializers
    static const int x2 = 2;
    static int x3 = sqrt( 2 );
    static const int x4 = sqrt( 2 );
};
#endif // INIT

// 9.5

union U2{
    int a;
    float b;
};

class S18:public U2{		// unions can't be bases
    const volatile long double far *const volatile near ***const ***far ****c;
};

struct SOK{
    int a;
};

union U3:public SOK{		// unions can't inherit
    float c;
    unsigned short int d;
};

#ifdef MEM_FUNC

union U1{
    virtual int f( int );	// unions can't be virtual
};

union U4{
    static int a;		// union can't have static members
    static int func( float near *a );
};

#ifdef INIT

struct S19{
    union{
	int a;
	long int b;
    };
    S19( void );
};

union U5{
    S19 s19;			// union can't have a class with constructor
};

struct S20{
    struct S{
	int a;
	int b;
    }s;
    ~S20( void );
};

union U6{
    S20 s20;			// can't have class with destructor
};

#endif // INIT

#endif // MEM_FUNC

int some_var;

static union{
    int some_var;		// name must be unique in scope
    float float_var;
};

union{				// global union must be static
    float some_float;
};

extern union{			// global union must be static
    float some_float2;
};

static union{
    public:			// OK (I think)
	long int volatile near * far * near * far *some_var2;
    private:		// no private or protected in anonymous union
	int far *some_var3;
    protected:		// ditto
	struct{ int a; float b; SOK s; }some_var4;
    public:
#ifdef MEM_FUNC	
    int func( int a );
#endif // MEM_FUNC
};

static union U7{
    int pub1;
    int pub2;

#ifdef MEM_FUNC
    int pubf( int a ){ return( a ); };
#endif // MEM_FUNC
protected:			// can't have protected
    int pro1;
    float pro2;
#ifdef MEM_FUNC
    int prof( int a ){ return( a ); };
#endif // MEM_FUNC
private:			// OK
    long pri1;
    SOK s;
#ifdef MEM_FUNC
    int prif( int a ){ return( a ); };
#endif // MEM_FUNC
};

// 9.6
struct S21{
    float b1:17;		// integral type
    double b2:18;
    long double b3:5;
    int b4:13.3;
    long :0;
    unsigned long b5:33;	// too long?
    long :0;
    short b6:17;		// ditto
    long :0;
    char b7:9;			// ditto
};

struct S22{			// OK
    int a:8;
};
    
// 9.7

class enclose{
    static int private_si;
  public:
    int b;
#ifdef MEM_FUNC
    int func( float );
#endif // MEM_FUNC 
    class inner{
#ifdef FRIEND	
	friend int inner_friend( int );
#endif // FRIEND
	static int private_inner_si;
      public:
        static int a;
#ifdef MEM_FUNC
	void f( int a );
	int f2( int );
#endif // MEM_FUNC 
    };
#ifdef MEM_FUNC
    g( inner* I );
#endif // MEM_FUNC 
};

#ifdef COLON_COLON
int enclose::inner::a = 1;	// OK

#ifdef MEM_FUNC
void enclose::inner::f2( int a ){
   b = a;			// can't access b or func
   return func( a );
   private_si = a;		// it's private
};

enclose::g( inner* I ){ I->private_inner_si = 5; };

void enclose::inner::f( int a ){
   b = a;		// can't access b or func
   return func( a );
   private_si = a;	// it's private
};
#endif // MEM_FUNC

enclose::inner b_inner;		// out of scope

#ifdef FRIEND
void inner_friend( int a ){
   enclose::b = a;		// can't access b or func
   return enclose::func( a );
   enclose::private_si = a;	// it's private
}
#endif // FRIEND
#endif // COLON_COLON

#ifdef FRIEND
#ifdef MEM_FUNC

class enclose2{
public:
   int x;
   static y;
   class inner2{
       int a;
       static b;

       friend void f( inner2* p, int i )
       {
	   p->a = i;
	   a = 2;		// inner2::a not static
	   x = 4;		// enclose2::x not static
       };
       friend void g( inner2 *, int );
   };
};

#ifdef COLON_COLON
void g( enclose2::inner2* p, int i )
{
    p->a = i;
    a = 2;			// OK
    b = 1;			// error: inner::b not in scope
    x = 3;			// error: enclose::x is not in scope
    y = 4;			// error: enclose::y is not in scope

    enclose::x = 3;		// x,y not static
    enclose::y = 4;
}
#endif //COLON_COLON
#endif //MEM_FUNC
#endif //FRIEND

// 9.9
struct S22_1{
    typedef int I;
    class S23{ };
    I a;
};

I b;				// out of scope
S23 c;				// ditto

typedef int type_int;		// OK
enum{ enum_val = 1 };

struct S24{
//    char v[ enum_val ];		// OK ******************
    type_int i;
    char type_int;		// redefined after use
    enum{ enum_val = 2 };	// redefined after use
};

struct S25{
    char type_int;		// OK
    enum{ enum_val = 2 };
};

struct S26{
    type_int i;			
    typedef long type_int;	// redefined after use
};

struct S27{
    type_int i;			
    typedef int type_int;	// OK
};


void main( void )//*******************main
{
    S1 bs1;
    S2 bs2;
    int i;

// 9.1
//    bs1 = bs2;			// different types ***********
//    bs1 = i;			// different types

#ifdef INIT
    S3( i );			// ambiguous
#ifdef OPERATOR
    S3 bs3;
    S1( bs3 );			// ambiguous
#endif // OPERATOR
#endif // INIT

// 9.3
#ifdef MEM_FUNC
    (S9 *)0->c( 17 );		// -> done before cast, error
    ((S9 *)0)->c( 17 );		// allowed, but undefined behaviour
    const S9 cs9;
    volatile S9 vs9;

    cs9.b( 5 );			// b not const or volatile
    vs9.b( 7 );
    s10.a = 17;			// S10 only inherits S9-illegal
#endif // MEM_FUNC

// 9.5
    int name_that_is_duplicated;
    union{
	int some_var;		// OK, some_var in file scope
	int name_that_is_duplicated;// duplicate
    };
    union{
	public:			// OK (I think)
	    long int volatile near * far * near * far *some_var2;
	private:		// no private or protected in anonymous union
	    int far *some_var3;
	protected:		// ditto
	    struct{ int a; float b; SOK s; }some_var4;
#if 0	    
	public:
#ifdef MEM_FUNC	
    	    int func( int a ){ a = a };
#endif // MEM_FUNC
#endif
    };
    {
    	U7 bu7;

	bu7.pub1 = 1;		// OK
	bu7.pub2 = 2;		// OK
	bu7.pro1 = 3;		// protected error
	bu7.pro2 = 4;		// ditto
	bu7.pri1 = 1;		// private error
	bu7.s.a = 17;		// ditto
#ifdef MEM_FUNC
        bu7.pubf( 1 );		// OK
	bu7.prof( 2 );		// protected error
	bu7.prif( 3 );		// private error
#endif // MEM_FUNC
    }
    union{ int aa; char *p; }obj;
    aa = 7;			// ERROR: not anonymous union
    obj.p = 0;
    union{ int aa2; char *p2; }*ptr;
    aa2 = 7;			// ERROR: not anonymous union
    ptr = new char[sizeof *ptr];
    ptr->p2 = 0;

// 9.6
    S22 bs22;
typedef int bitfield:8;    
    bitfield *bfp;		// can't have bitfield pointers
    bfp = &bs22.a;
    bitfield &bfr;		// can't have bitfield references

// 9.7
#ifdef COLON_COLON
    enclose::inner::a = 1;	// can't access nested function
#endif // COLON_COLON    

// 9.8
    struct LOCAL{
    private:
	int local_int;
#ifdef MEM_FUNC
	int priv_func( int a ){ return( a ); };
#endif // MEM_FUNC
    public:	
	static int a;		// no static data
#ifdef MEM_FUNC
	int func( int );	// local functions must have definition
	int func2( int a ){ local_int = a; return( a ); };//	OK
#endif // MEM_FUNC
    };
    LOCAL b_local;
    b_local.local_int = 17;	// local_int is private
#ifdef MEM_FUNC
    b_local.priv_func( 17 );	// private
#endif // MEM_FUNC
}
