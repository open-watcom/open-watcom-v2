BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    num = 1;
}
{
    if( num == 2 ) {
        printf( "%s\n", $0 ) > OUTFILE
    }
    if( num > 2 ) {
        num = 0;
    }
    num = num + 1;
}
END {
}