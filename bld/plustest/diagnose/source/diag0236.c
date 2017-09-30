// -ml-wx-zu
class WState {

    public:
	unsigned char GetFlags( unsigned char flags )
	    { return( _flags & flags ); };
    private:
	unsigned char		_flags;
};



    WState c;
main(){
    char x;
    unsigned y;
    y = y & x;
    x = x & y;
    y = y | x;
    x = x | y;
    y = y ^ x;
    x = x ^ y;
    return c.GetFlags( 0x80 );
}

    
    


