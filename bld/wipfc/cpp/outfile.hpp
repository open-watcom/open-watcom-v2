#include <string>


class Nls;

struct UNIFILE {
    std::FILE   *_fp;
};

class OutFile {
    typedef STD1::uint8_t   byte;
    typedef STD1::uint16_t  word;
    typedef STD1::uint32_t  dword;

public:
    OutFile( const std::string& sfname, Nls *nls );
    ~OutFile();

    dword tell() { return std::ftell( _ufp->_fp ); };
    bool write( const void *buffer, std::size_t size, std::size_t cnt ) { return( std::fwrite( buffer, size, cnt, _ufp->_fp ) != cnt ); };
    bool putc( char data ) { return( std::fputc( data, _ufp->_fp ) == EOF ); };
    bool put( byte data ) { return( std::fwrite( &data, sizeof( data ), 1, _ufp->_fp ) != 1 ); };
    bool put( word data ) { return( std::fwrite( &data, sizeof( data ), 1, _ufp->_fp ) != 1 ); };
    bool put( dword data ) { return( std::fwrite( &data, sizeof( data ), 1, _ufp->_fp ) != 1 ); };
    bool codePage( word codePage );
    int seek( dword offset, int where ) { return std::fseek( _ufp->_fp, offset, where ); };
    // UNICODE->MBCS conversion
    std::string wtomb_string( const std::wstring& input );

private:
    UNIFILE*            _ufp;
    word                _codePage;
};
