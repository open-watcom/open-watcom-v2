struct S {
    int a;
    static int b;
};

int S::a = 1;		// initializing a non-static member
int S::b = 2;

typedef int T = 1;	// initializing a typedef

extern int E = 2;	// initializing an extern

void F( void ) = 2;	// initializing a function

// too many initializers for aggregate type
int a[3] = { 1, 2, 3, 4 };

// too many initializers for character string
char ac[3] = "abc";

// string literals concatenated for array initialization (twice)
// imagine this: { " ","," ","," " }
char *ad[] = { "abc",
	       "def" "ghi",
	       "jkl", "mno"
	       "pqr" };

// expecting '{' but found expression
int b[3] = 3; // {

// expecting '}' but found '{'
int c[3] = { { 1 }, { 2 }, { 3 }, { 4 } };

// expecting '{' but found '}'
int d[3] = };

// unexpected '}' during initialization
// or syntax error
int e[3] = { 1, 2, 3 } };

// unexpected '{' during initialization
// or syntax error
int f[3] = { 1, 2, 3 } {; };

// cannot generate default constructor for initialization since constructors were declared
struct S2 { S2( int ); };
S2 g[3] = { 1, S2( 2 ) };

// const must be initialized
const int h;
const int ha[3];

// reference must be initialized
int &i;

// don't complain about unreferenced integral consts
const int ok1 = 1;
const short ok2 = 2;
const long ok3 = 3;

// but these can be diagnosed
const int bad1[3] = {2};
const double bad2 = 2;

void bad()
{
    // complain about these
    const int ok1 = 1;
    const short ok2 = 2;
    const long ok3 = 3;
    const int bad1[3] = {2};
    const double bad2 = 2;
}
