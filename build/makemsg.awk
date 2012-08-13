# Print set counter variable
BEGIN { 
    num = 0
}

# Process all lines beginning with 'pick'
/^ *pick/ {
    gsub( /,/, "" ) # Remove commas
    printf( "#define %s (%s + %d)\n", $2, base, num )
    num = num + 1   # Increment counter
}
