BEGIN {
    num = 1;
}
{
    if( num == 2 ) {
        printf( "%s\n", $0 );
    }
    if( num > 2 ) {
        num = 0;
    }
    num = num + 1;
}
END {
}