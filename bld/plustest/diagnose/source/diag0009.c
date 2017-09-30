struct S {
    int f;
    S();
};

void test1( void )
{
    {
	S a;
    
	OK:
	    goto OK;
    }
}

void test2( void )
{
    goto BAD;
    {
	S a;

	BAD:;
    }
}

void test3( void )
{
    goto around;
    back:
    {
	S a;

	OK:
	{
	    {
	    	a.f++;
		goto OK;
	    }
	}
    }
    around: goto back;
}

void test4( void )
{
    goto around;
    back:
    {
	S a;

	OK:
	{
	    {
	    	a.f++;
	    }
	}
	goto OK;
    }
    around: goto back;
}

void test5( void )
{
    goto around;
    back:
    {
	S a;

	BAD:
	{
	    {
	    	a.f++;
	    }
	}
    }
    goto BAD;
    around: goto back;
}

void test6( void )
{
    {
	S a;

	goto OK;
	OK:;
    }
}

void test7( void )
{
    {
	goto BAD;
	S a;

	BAD:;
	a.f++;
    }
}
