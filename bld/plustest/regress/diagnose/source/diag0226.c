// BADRETRN.C -- diagnose bad returns
//
// 92/03/26 -- J.W.Welch        -- defined
#pragma enum minimum

typedef enum { e1, e2, e3 } etype;


etype efun( int n )
{
    switch( n ) {
        case 0:
            return e2;
        case 1:
            return 2;
        case 2:
            return n;
    }
    return e1;
}


static char ch = 'a';
static const char cch = 'a';
static int  in = 4;


char *pfun( int n )
{
    switch( n ) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return &ch;
        case 3:
            return &cch;
        case 4:
            return &in;
        case 5:
            return e1;
        case 6:
            return e2;
    }
    return &ch;
}


const char *cpfun( int n )
{
    switch( n ) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return &ch;
        case 3:
            return &cch;
        case 4:
            return &in;
    }
    return &ch;
}



int fun()
{
    return 14;
}
