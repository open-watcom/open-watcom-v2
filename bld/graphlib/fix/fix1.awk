# Print set counter variable
BEGIN {
    if( host == "unix" ) {
        cmd = ":";
    } else {
        cmd = "*";
    }
}

# Process all lines and redirect module to .obj file
{
    printf( "%s%s=.obj\n", cmd, $1 )
}
