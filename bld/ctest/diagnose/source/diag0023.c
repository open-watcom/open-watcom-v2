/* diagnose use of 'else' without appropriate 'if' statement */

int main(int x)
{
    int rc;

    else
    rc = x;
    if( rc == 1 ) {
        rc += 2;
    } else if( rc == 2 ) {
        rc += 5;
    } else {
        rc += 3;
        else
    }
    else
    {
        rc += 4; else
        else rc = rc / 2;
        else
    }
    else
    rc = rc / 2 + 4;
    return( rc / 2 );
}
