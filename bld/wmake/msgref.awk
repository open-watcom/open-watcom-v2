BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
}
{
    print "/" $1 "/i\\" > OUTFILE
    print $1 > OUTFILE
}

END {
    print "1,$d" > OUTFILE
}

