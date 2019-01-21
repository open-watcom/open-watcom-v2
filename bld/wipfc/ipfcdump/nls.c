// Read the NLS data

#include "ipfcdump.h"

static void processCountry( FILE *, FILE *, NlsHeader *) ;
static void processSBCS( FILE *, FILE *, NlsHeader * );
static void processDBCS( FILE *, FILE *, NlsHeader * );
static char * getType( uint8_t );

void readNLS( FILE *in, FILE *out )
{
    fputs( "\nNLS Data\n", out );
    if( Hdr.nlsSize ) {
        NlsHeader nls;
        size_t total;
        fseek( in, Hdr.nlsOffset, SEEK_SET );
        for( total = 0; total < Hdr.nlsSize; total += nls.size ) {
            fread( &nls, sizeof( NlsHeader ), 1, in );
            if( nls.type == CONTROL ) {
                processCountry( in, out, &nls );
            } else if( nls.format == 0 ) {
                processSBCS( in, out, &nls );
            } else {
                processDBCS( in, out, &nls );
            }
        }
    } else {
        fputs("\n  NLS Data is not present\n", out);
    }
}
/*****************************************************************************/
static void processCountry( FILE *in, FILE *out, NlsHeader *nls )
{
    NlsCountryDef c;
    c.reserved = 0;
    fread( &c, nls->size - sizeof( NlsHeader ), 1, in );
    fputs( "  NLS Country Definition\n", out);
    fprintf( out, "    NlsCountryDef.size:     %4.4x (%hu)\n", nls->size, nls->size );
    fprintf( out, "    NlsCountryDef.type:     %s\n", getType( nls->type ) );
    fprintf( out, "    NlsCountryDef.format:   %4.2x (%hu)\n", nls->format, nls->format );
    fprintf( out, "    NlsCountryDef.value:    %4.4x (%hu)\n", c.value, c.value );
    fprintf( out, "    NlsCountryDef.code:     %4.4x (%hu)\n", c.code, c.code );
    fprintf( out, "    NlsCountryDef.page:     %4.4x (%hu)\n", c.page, c.page );
    if( nls->size > 10 ) {
        fprintf( out, "    NlsCountryDef.reserved: %4.4x (%hu)\n", c.reserved, c.reserved );
    }
}
/*****************************************************************************/
static void processSBCS( FILE *in, FILE *out, NlsHeader *nls )
{
    SbcsNlsGrammarDef s;
    size_t count;
    fread( &s, sizeof(SbcsNlsGrammarDef), 1, in );
    fputs( "  NLS SBCS Grammar Definition\n", out );
    fprintf( out, "    SbcsNlsGrammarDef.size:   %4.4x (%hu)\n", nls->size, nls->size );
    fprintf( out, "    SbcsNlsGrammarDef.type:   %s\n", getType( nls->type ) );
    fprintf( out, "    SbcsNlsGrammarDef.format: %4.2x (%hu)\n", nls->format, nls->format );
    fprintf( out, "    SbcsNlsGrammarDef.bits:\n");
    for( count = 0; count < 32; count++ ) {
        fprintf(out, "%s ", bstring( s.bits[ count ] ) );
        if( !( ( count + 1 ) & 7 ) ) {
            fputc( '\n', out );
        }
    }
    if( count & 7 )
        fputc( '\n', out );
    for( count = 0; count < 32; count++ ) {
        fprintf( out, "%2.2x ", s.bits[ count ] );
        if( !( ( count + 1 ) & 7 ) ) {
            fputc( '\n', out );
        }
    }
    if( count & 7 ) {
        fputc('\n', out );
    }
}
/*****************************************************************************/
static void processDBCS( FILE *in, FILE *out, NlsHeader *nls )
{
    size_t   count = 0;
    size_t   items = ( nls->size - sizeof( NlsHeader ) ) / ( 2 * sizeof( uint16_t ) );
    uint16_t hi;
    uint16_t lo;
    fputs( "  NLS DBCS Grammar Definition\n", out );
    fprintf( out, "    DbcsNlsGrammarDef.size:   %4.4x (%hu)\n", nls->size, nls->size );
    fprintf( out, "    DbcsNlsGrammarDef.type:   %s\n", getType( nls->type ) );
    fprintf( out, "    DbcsNlsGrammarDef.format: %4.2x (%hu)\n", nls->format, nls->format );
    fputs( "    DbcsNlsGrammarDef data:\n", out );
    for( count = 0; count < items; count++ ) {
        fread( &lo, sizeof( uint16_t ), 1, in );
        fread( &hi, sizeof( uint16_t ), 1, in );
        fprintf( out, "      %4.4x, %4.4x (%hu, %hu)\n", lo, hi, lo, hi );
    }
}
/*****************************************************************************/
static char * getType( uint8_t type)
{
    static char * types[] = { "control", "text", "graphics" };
    return( types[ type ] );
}
