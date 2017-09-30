typedef int A;
int A;			// error: name 'A' previously used for a typedef

typedef int B;
int B( int );		// error: name 'B' previously used for a typedef

enum C { };
int C;

enum D { };
int D( int );

struct E { };
int E;

struct F { };
int F( int );

int G;
typedef int G;		// error: name 'G' previously used for a variable

int H( int );
typedef int H;		// error: name 'H' previously used for a function

int I;
enum I { };

int J( int );
enum J { };

int K;
struct K { };

int L( int );
struct L { };

#include "diag0103.h"

int ST1;		// error: name 'ST1' previously used for a typedef
int ST2( int );		// error: name 'ST2' previously used for a typedef
int CT1;		// warning: 'CT1' was a typedef
int CT2( int );		// warning: 'CT2' was a typedef
int UT1;		// warning: 'UT1' was a typedef
int UT2( int );		// warning: 'UT2' was a typedef
enum VN1 {};		// warning: 'VN1' was a var/fn
struct VN2 {};		// warning: 'VN2' was a var/fn
enum FN1 {};		// warning: 'FN1' was a var/fn
struct FN2 {};		// warning: 'FN2' was a var/fn
