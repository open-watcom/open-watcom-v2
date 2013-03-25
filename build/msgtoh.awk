# Print preamble and set counter variable
BEGIN {
    if( length( basename ) == 0 ) {
        basename = "MSG_RC_BASE"
    }
    printf( "#define %s %d\n", basename, rcbase )
    num = 1
}

# Process all lines beginning with 'pick'
/^ *pick/ {
   gsub( /,/, "" ) # Remove commas
   print "#define " $2 " (" basename " + " num ")"  
   num = num + 1   # Increment counter
}
