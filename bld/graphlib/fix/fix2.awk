# Print set counter variable
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    num = 1;
}

# Process all lines and redirect each module to g#.obj file
{
    printf( "%s%s=%-.8s.obj\n", host_cmd, $1, $1 ) > OUTFILE
    num = num + 1;
}
