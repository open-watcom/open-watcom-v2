struct X;

void takes_X( X & );
void takes_cX( const X & );
void takes_vX( volatile X & );

void has_X( X &x )
{
    takes_X( x );
    takes_cX( x );
    takes_vX( x );
}

void has_cX( const X &x )
{
    takes_X( x );		// error: cannot strip away const
    takes_cX( x );
    takes_vX( x );		// error: cannot strip away const
}

void has_vX( volatile X &x )
{
    takes_X( x );		// error: cannot strip away volatile
    takes_cX( x );		// error: cannot strip away volatile
    takes_vX( x );
}
