char *thunk( char *p )
{
    // generates:
    //          sub ax,4
    //          add ax,2
    // cmd: wccp -ox -d1
    p -= 4;
    p += 2;
    return( p );
}
