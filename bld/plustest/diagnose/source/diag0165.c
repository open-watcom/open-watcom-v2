// check that referencing works for functions
// check that referencing works for mem-initialzer optimized away
// check that mem-initializers are analyzed when optimized away
 
typedef void (*FunctionType)(int CallReason); 
 
int Reason; 
 
static void Y(int CallReason) 
{ 
    Reason = CallReason; 
} 
static void Z(int CallReason) 
{ 
    Reason = CallReason; 
} 
 
FunctionType X(int CallReason) 
{ 
    return (CallReason ? Z : Y); // W014: Z/Y unrefered ? 
} 
 
struct A {}; 
struct B : public A { B(const A&); }; 
B::B(const A& a) : A(a) {}  


struct S {};

struct T : S
{
    T();
    T( const S& );
    T( S& );
};

T::T( const S& s ) : S(s)
{
}

T::T( S& s ) : S( 987 )
{
}

T::T() : S( 8765 )
{
}
