# Generate enum values from pick
# pick( msg,
# Print preamble and set enum start
BEGIN {
    # Basic sanity check on input
    if( OUTFILE == "" ) {
        printf( "OUTFILE variable must be set!\n" ) > "/dev/stderr"
        exit 1
    }
    printf( "    msg_base = %d,\n", base ) > OUTFILE
}

# Process all lines beginning with 'pick'
/^ *pick/ {
   gsub( /,/, "" ) # Remove commas
   print "    " tolower( $2 ) "," > OUTFILE
}

