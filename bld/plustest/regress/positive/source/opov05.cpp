#include <string.hpp>
#include "fail.h"

String a[10];

int main() {
    String	s;
    String	s2;
    String & sr = s2;
    const String cs = "W";
    String *sp = a;
    char *str = "01234567890";
    const char *cstr = "sdf";

    s = "W" + s;
    s = s + "W";
    s += "W";
    sr = sr + "W";
    sr += "W";

    s = 'W' + s;
    s = s + 'W';
    s += 'W';
    sr = sr + 'W';
    sr += 'W';

    s = str + s;
    s = s + str;
    s += str;
    sr = sr + str;
    sr += str;

    s = cstr + s;
    s = s + cstr;
    s += cstr;
    sr = sr + cstr;
    sr += cstr;

    sp = sp + 1;
    sp += 1;

    if( s == s2);
    if( s == cs);
    if( s == "W");
    if( s == 'W');
    if( cs == s);
    if( cs == "W");
    if( cs == 'W');

    if( s != s2);
    if( s != cs);
    if( s != "W");
    if( s != 'W');
    if( cs != s);
    if( cs != "W");
    if( cs != 'W');

    if( s <= s2);
    if( s <= cs);
    if( s <= "W");
    if( s <= 'W');
    if( cs <= s);
    if( cs <= "W");
    if( cs <= 'W');

    if( s < s2);
    if( s < cs);
    if( s < "W");
    if( s < 'W');
    if( cs < s);
    if( cs < "W");
    if( cs < 'W');

    if( s >= s2);
    if( s >= cs);
    if( s >= "W");
    if( s >= 'W');
    if( cs >= s);
    if( cs >= "W");
    if( cs >= 'W');

    if( s > s2);
    if( s > cs);
    if( s > "W");
    if( s > 'W');
    if( cs > s);
    if( cs > "W");
    if( cs > 'W');

    if( s[0] == 'W' );
    if( s[0] != 'W' );
    _PASS;
}
