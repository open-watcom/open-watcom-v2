# generate defines for msgs
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    printf( "#define MSG_LANG_SPACING    1000\n\n" ) > OUTFILE
    printf( "#define MSG_BASE %d\n\n", base ) > OUTFILE
    num = 1
}

# Process all lines beginning with 'pick'
/^ *pick/ {
    gsub( /,/, "" ) # Remove commas
    printf( "#define %s (MSG_BASE + %d)\n", $2, num ) > OUTFILE
    num = num + 1   # Increment counter
}
