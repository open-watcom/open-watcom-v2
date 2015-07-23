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
    num = 1;
}

# Process all lines and redirect each module to g#.obj file
{
    printf( "%s%s=g%7.7d.obj\n", cmd, $1, num ) > OUTFILE
    num = num + 1;
}
