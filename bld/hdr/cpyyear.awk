BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    if( CPYYEAR == "" ) {
        printf( "CPYYEAR variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    str = sprintf("%d The Open Watcom Contributors", CPYYEAR)
}
{
    gsub( /xxxx The Open Watcom Contributors/, str )
    print $0 > OUTFILE
}
