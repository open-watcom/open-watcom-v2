# generate defines for msgs
BEGIN {
    printf( "#define MSG_LANG_SPACING    1000\n\n" )
    printf( "#define MSG_BASE %d\n\n", base )
    num = 1
}

# Process all lines beginning with 'pick'
/^ *pick/ {
    gsub( /,/, "" ) # Remove commas
    printf( "#define %s (MSG_BASE + %d)\n", $2, num )
    num = num + 1   # Increment counter
}
