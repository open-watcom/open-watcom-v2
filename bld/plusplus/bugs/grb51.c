// C compiler generates two copies of "asdf" one in FAR data and the other in _CONST
// we currently have no info to let us generate the far data one
char far *p = "asdf";

void main()
{
    char buff[10];

    strcpy( buff, "asdf" );
}
