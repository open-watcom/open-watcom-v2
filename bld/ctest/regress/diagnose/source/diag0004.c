/* Test comparison warnings. Tests for 'comparison is always 0/1'
 * and 'comparison equivalent to unsigned == 0' warnings.
 */

int main( int argc, char **argv )
{
    int                     ret;
    unsigned char           uc;
    unsigned short          us;
    unsigned int            ui;
    unsigned long           ul;
    unsigned long long      ull;
    struct {
        int                     ret : 2;
        unsigned char           uc  : 2;
        unsigned short          us  : 2;
        unsigned int            ui  : 2;
        unsigned long           ul  : 2;
        unsigned long long      ull : 2;
    } st;

    ret = 0;
    uc = us = ui = ul = ull = argc;
    if( uc < 0 )    // always 0
        ret = 1;
    if( uc <= 0 )   // equivalent to uc == 0
        ret = 1;
    if( uc >= 0 )   // always 1
        ret = 1;
    if( uc > 0 )    // no problem here
        ret = 1;
    if( uc == 0 )   // no problem here
        ret = 1;
    if( uc != 0 )   // no problem here
        ret = 1;

    if( us < 0 )    // always 0
        ret = 1;
    if( us <= 0 )   // equivalent to uc == 0
        ret = 1;
    if( us >= 0 )   // always 1
        ret = 1;
    if( us > 0 )    // no problem here
        ret = 1;
    if( us == 0 )   // no problem here
        ret = 1;
    if( us != 0 )   // no problem here
        ret = 1;

    if( ui < 0 )    // always 0
        ret = 1;
    if( ui <= 0 )   // equivalent to uc == 0
        ret = 1;
    if( ui >= 0 )   // always 1
        ret = 1;
    if( ui > 0 )    // no problem here
        ret = 1;
    if( ui == 0 )   // no problem here
        ret = 1;
    if( ui != 0 )   // no problem here
        ret = 1;

    if( ul < 0 )    // always 0
        ret = 1;
    if( ul <= 0 )   // equivalent to uc == 0
        ret = 1;
    if( ul >= 0 )   // always 1
        ret = 1;
    if( ul > 0 )    // no problem here
        ret = 1;
    if( ul == 0 )   // no problem here
        ret = 1;
    if( ul != 0 )   // no problem here
        ret = 1;

    if( ull < 0 )   // always 0
        ret = 1;
    if( ull <= 0 )  // equivalent to uc == 0
        ret = 1;
    if( ull >= 0 )  // always 1
        ret = 1;
    if( ull > 0 )   // no problem here
        ret = 1;
    if( ull == 0 )  // no problem here
        ret = 1;
    if( ull != 0 )  // no problem here
        ret = 1;

    st.ret = 0;
    st.uc = st.us = st.ui = st.ul = st.ull = argc;
    if( st.ret < -2 )  // always 0
        st.ret = 1;
    if( st.ret > 1 )   // always 0
        st.ret = 1;
    if( st.ret > -3 )  // always 1
        st.ret = 1;
    if( st.ret < 2 )   // always 1
        st.ret = 1;

    if( st.uc < 0 )    // always 0
        st.ret = 1;
    if( st.uc <= 0 )   // equivalent to uc == 0
        st.ret = 1;
    if( st.uc >= 0 )   // always 1
        st.ret = 1;
    if( st.uc > 3 )    // always 0
        st.ret = 1;
    if( st.uc > 0 )    // no problem here
        st.ret = 1;
    if( st.uc == 0 )   // no problem here
        st.ret = 1;
    if( st.uc != 0 )   // no problem here
        st.ret = 1;

    if( st.us < 0 )    // always 0
        st.ret = 1;
    if( st.us <= 0 )   // equivalent to uc == 0
        st.ret = 1;
    if( st.us >= 0 )   // always 1
        st.ret = 1;
    if( st.us > 3 )    // always 0
        st.ret = 1;
    if( st.us > 0 )    // no problem here
        st.ret = 1;
    if( st.us == 0 )   // no problem here
        st.ret = 1;
    if( st.us != 0 )   // no problem here
        st.ret = 1;

    if( st.ui < 0 )    // always 0
        st.ret = 1;
    if( st.ui <= 0 )   // equivalent to uc == 0
        st.ret = 1;
    if( st.ui >= 0 )   // always 1
        st.ret = 1;
    if( st.ui > 3 )    // always 0
        st.ret = 1;
    if( st.ui > 0 )    // no problem here
        st.ret = 1;
    if( st.ui == 0 )   // no problem here
        st.ret = 1;
    if( st.ui != 0 )   // no problem here
        st.ret = 1;

    if( st.ul < 0 )    // always 0
        st.ret = 1;
    if( st.ul <= 0 )   // equivalent to uc == 0
        st.ret = 1;
    if( st.ul >= 0 )   // always 1
        st.ret = 1;
    if( st.ul > 3 )    // always 0
        st.ret = 1;
    if( st.ul > 0 )    // no problem here
        st.ret = 1;
    if( st.ul == 0 )   // no problem here
        st.ret = 1;
    if( st.ul != 0 )   // no problem here
        st.ret = 1;

    if( st.ull < 0 )   // always 0
        st.ret = 1;
    if( st.ull <= 0 )  // equivalent to uc == 0
        st.ret = 1;
    if( st.ull >= 0 )  // always 1
        st.ret = 1;
    if( st.ull > 3 )   // always 0
        st.ret = 1;
    if( st.ull > 0 )   // no problem here
        st.ret = 1;
    if( st.ull == 0 )  // no problem here
        st.ret = 1;
    if( st.ull != 0 )  // no problem here
        st.ret = 1;

    return( ui );
}
