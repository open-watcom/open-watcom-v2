// EXCERR.C -- test compile-time error diagnosis related to exceptions
//
// 92/11/18 -- J.W.Welch        -- defined

extern void goop();

struct B
{ int a[3];
};

struct D : public B
{ int d[5];
};

struct
{   int un[1];
} undef_class_var;

class priv_copy
{
    priv_copy( const priv_copy &);
public:
    priv_copy();
    int a[3];
};

priv_copy pcv;

class priv_dtor
{
    ~priv_dtor();
public:
    priv_dtor( const priv_dtor &);
    priv_dtor();
    int a[3];
};

void foo( int a )
{
    try {
    }                   // no catch blocks

    try {
    } catch( B ) {
    } catch( D ) {      // derived after base
    }

    try {
    } catch( B& ) {
    } catch( D& ) {     // derived after base reference
    }

    try {
    } catch( B ) {
    } catch( D& ) {     // derived after base
    }

    try {
    } catch( B& ) {
    } catch( D ) {      // derived after base reference
    } catch( ... ) {
    } catch( int ) {    // catch follows ellipsis
    }

    if(a) goto try_lab_1;
    if(a) goto catch_lab_1;

    try{
try_lab_1:              // jump into try block
        goop();
    } catch( int ) {
catch_lab_1:            // jump into catch block
        goop();
    }

    if(a) goto try_lab_1;   // jump into try block
    if(a) goto catch_lab_1; // jump into catch block

    struct undefed;
    throw undefed();

    throw undef_class_var;  // warn: throw unnamed class ?

    throw priv_copy();  // private copy CTOR
    throw priv_dtor();  // private DTOR

    try {
    } catch( B* ) {
    } catch( D* ) {     // ptr-to-derived after ptr-to-base
    }

    try {
    } catch( B*& ) {
    } catch( D*& ) {    // ptr-to-derived after ptr-to-base
    }
}
