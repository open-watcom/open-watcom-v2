# Print set counter variable
BEGIN { 
    if( host == "unix" ) {
        cmd = ":";
    } else {
        cmd = "*";
    }
    num = 1;
}

# Process all lines and redirect each module to g#.obj file
{
    printf( "%s%s=g%7.7d.obj\n", cmd, $1, num );
    num = num + 1;
}
