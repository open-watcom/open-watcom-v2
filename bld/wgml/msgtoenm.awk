# Generate enum values from pick
# pick( msg,
# Print preamble and set enum start
BEGIN {
    printf( "    msg_base = %d,\n", base )
}

# Process all lines beginning with 'pick'
/^ *pick/ {
   gsub( /,/, "" ) # Remove commas
   print "    " tolower( $2 ) ","
}

