char *foo( void )
{
    return( ::new char[10] ( 'a', 'b', 'c', '\0' ) );
}
