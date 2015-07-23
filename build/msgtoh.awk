# Print preamble and set counter variable
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    if( length( basename ) == 0 ) {
        basename = "MSG_RC_BASE"
    }
    printf( "#define %s %d\n", basename, rcbase ) > OUTFILE
    num = 1
}

# Process all lines beginning with 'pick'
/^[\t ]*pick/ {
   gsub( /,/, "" ) # Remove commas
   print "#define " $2 " (" basename " + " num ")" > OUTFILE
   num = num + 1   # Increment counter
}
