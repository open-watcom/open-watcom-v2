union {
    int x : 4;
    int y : 2;
} p;

void main( void )
{
    p.x = 0;
    p.y = -1;
    if( p.x == 0 ) {
        puts( "FAIL" );
    }
}
