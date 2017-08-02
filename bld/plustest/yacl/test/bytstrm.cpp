

#include "base/base.h"
#include <iostream.h>

main ()
{
    CL_String s1 ("Just some string here.");
    CL_String y  ("Yet another class library");

    CL_ByteString b (50);
    CL_ByteStream strm (b);
    long l = 509;
    strm << s1 << l << y;
    s1 = ""; y = "Junk"; l = 0;
    strm.SeekTo (0);
    strm >> s1 >> l >> y;
    cout << s1 << endl << l << endl << y << endl;
    return 0;
}
