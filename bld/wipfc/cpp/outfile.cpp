#include "wipfc.hpp"
#include <cstdlib>
#include <climits>
#include <cstring>
#include "fnt.hpp"
#include "errors.hpp"
#include "outfile.hpp"
#include "util.hpp"
#include "nls.hpp"


OutFile::OutFile( const std::string& sfname, Nls *nls ) : _ufp( NULL ), _codePage( DEFAULT_CODEPAGE )
{
    std::FILE *fp;

    _codePage = nls->codePage();

    fp = std::fopen( sfname.c_str(), "wb" );
    if( fp != NULL ) {
        _ufp = new UNIFILE;
        _ufp->_fp = fp;
//        _conv = ucnv_safeClone( conv, 0, &bufferSize, &err );
    }
}

OutFile::~OutFile()
{
    if( _ufp != NULL ) {
        std::fclose( _ufp->_fp );
//        uconv_close( _conv );
        delete _ufp;
    }
}

bool OutFile::codePage( word codePage )
{
    if( codePage == DEFAULT_CODEPAGE )
        codePage = _codePage;
    return( write( &codePage, sizeof( codePage ), 1 ) );
}

static int wtomb_char( char *mbc, wchar_t wc )
/********************************************/
{
    // TODO! must be converted by selected UNICODE->MBCS conversion table
    // which is independent from the host user locale
    return( std::wctomb( mbc, wc ) );
}

std::string OutFile::wtomb_string( const std::wstring& input )
/************************************************************/
{
    std::string output;
    for( std::size_t index = 0; index < input.size(); ++index ) {
        char ch[ MB_LEN_MAX + 1 ];
        int  bytes( wtomb_char( &ch[ 0 ], input[ index ] ) );
        if( bytes == -1 )
            throw FatalError( ERR_T_CONV );
        ch[ bytes ] = '\0';
        output += ch;
    }
    return( output );
}
