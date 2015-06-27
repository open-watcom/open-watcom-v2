# Print set counter variable
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    num = 0
    FS = "pick"
}

# Process all lines beginning with 'pick'
/^[\t ]*pick/ {
    for( i = 2; i <= NF; ++i ) {
        split( $i, f, "[\t ,]+" )
        printf( "#define %s%s (%s + %d)\n", prefix, f[2], base, num ) >> OUTFILE
        num = num + 1   # Increment counter
    }
}
