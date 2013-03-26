# Print set counter variable and output format
BEGIN { 
    num = 0
    if( lang ) {
        fmt = "pick((%s+%d), \"\", %s)\n"
    } else {
        fmt = "pick((%s+%d), %s, \"\")\n"
    }
}

# Process all lines
{
    printf( fmt, base, num, $0 )
    num = num + 1   # Increment counter
}
