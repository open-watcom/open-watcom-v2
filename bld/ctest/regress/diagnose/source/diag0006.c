/* Test function type mismatch warnings */

int __cdecl (*cdv)( int );
typedef int __cdecl (*cdt)( int );

int __watcall (*wcv)( int );
typedef int __watcall (*wct)( int );

int (*dcv)( int );
typedef int (*dct)( int );

int __cdecl cdf( int a ) { return( a ); }
int __watcall wcf( int a ) { return( a ); }
int dcf( int a ) { return( a ); }

int main( int argc, char **argv )
{
    cdt     cdlv;
    dct     dclv;
    wct     wclv;

    // Correct assignments
    cdv  = cdf;
    cdlv = cdf;
    dcv  = dcf;
    dclv = dcv;
    wcv  = wcf;
    wclv = wcf;

    // Incorrect assignments
    cdv  = dcf;
    cdv  = wcf;
    cdlv = dcf;
    cdlv = wcf;

    dcv  = cdf;
    dclv = cdf;

    wcv  = cdf;
    wclv = cdf;

    // Note: Assignments between __watcall and undecorated functions aren't
    // tested. At the moment it isn't clear whether those assignments should
    // be diagnosed or not (when compiling with -3r or equivalent).
    return( 0 );
}
