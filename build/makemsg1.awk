# Print set counter variable and output format
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    num = 0
    if( lang ) {
        fmt = "pick((%s+%d), \"\", %s)\n"
    } else {
        fmt = "pick((%s+%d), %s, \"\")\n"
    }
}

# Process all lines
{
    printf( fmt, base, num, $0 ) > OUTFILE
    num = num + 1   # Increment counter
}
