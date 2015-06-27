# Print set counter variable
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    if( host == "unix" ) {
        cmd = ":";
    } else {
        cmd = "*";
    }
}

# Process all lines and redirect module to .obj file
{
    printf( "%s%s=.obj\n", cmd, $1 ) > OUTFILE
}
