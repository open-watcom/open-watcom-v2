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
        NlsHeader hdr;
        size_t total;
        fseek( in, Hdr.nlsOffset, SEEK_SET );
        for( total = 0; total < Hdr.nlsSize; total += hdr.size ) {
            fread( &hdr, sizeof(NlsHeader), 1, in );
            if( hdr.type == CONTROL )
                processCountry(in, out, &hdr );
            else if( hdr.format == 0 )
                processSBCS( in, out, &hdr );
            else
                processDBCS( in, out, &hdr );
            }
        }
    else
        fputs("\n  NLS Data is not present\n", out);
}
/*****************************************************************************/
static void processCountry( FILE *in, FILE *out, NlsHeader *hdr )
{
    NlsCountryDef c;
    c.reserved = 0;
    fread( &c, hdr->size - sizeof( NlsHeader ), 1, in );
    fputs( "  NLS Country Definition\n", out);
    fprintf( out, "    NlsCountryDef.size:     %4.4x (%hu)\n", hdr->size, hdr->size );
    fprintf( out, "    NlsCountryDef.type:     %s\n", getType( hdr->type ) );
    fprintf( out, "    NlsCountryDef.format:   %4.2x (%hu)\n", hdr->format, hdr->format );
    fprintf( out, "    NlsCountryDef.value:    %4.4x (%hu)\n", c.value, c.value );
    fprintf( out, "    NlsCountryDef.code:     %4.4x (%hu)\n", c.code, c.code );
    fprintf( out, "    NlsCountryDef.page:     %4.4x (%hu)\n", c.page, c.page );
    if( hdr->size > 10 )
        fprintf(out, "    NlsCountryDef.reserved: %4.4x (%hu)\n", c.reserved, c.reserved );
}
/*****************************************************************************/
static void processSBCS( FILE *in, FILE *out, NlsHeader *hdr )
{
    SbcsNlsGrammerDef s;
    size_t count;
    fread( &s, sizeof(SbcsNlsGrammerDef), 1, in );
    fputs( "  NLS SBCS Grammer Definition\n", out );
    fprintf( out, "    SbcsNlsGrammerDef.size:   %4.4x (%hu)\n", hdr->size, hdr->size );
    fprintf( out, "    SbcsNlsGrammerDef.type:   %s\n", getType( hdr->type ) );
    fprintf( out, "    SbcsNlsGrammerDef.format: %4.2x (%hu)\n", hdr->format, hdr->format );
    fprintf( out, "    SbcsNlsGrammerDef.bits:\n");
    for( count = 0; count < 32; count++ ) {
        fprintf(out, "%s ", bstring( s.bits[ count ] ) );
        if( !( ( count + 1 ) & 7 ) )
            fputc( '\n', out );
    }
    if( count & 7 )
        fputc( '\n', out );
    for( count = 0; count < 32; count++ ) {
        fprintf( out, "%2.2x ", s.bits[ count ] );
        if( !( ( count + 1 ) & 7 ) )
            fputc( '\n', out );
    }
    if( count & 7 )
        fputc('\n', out );
}
/*****************************************************************************/
static void processDBCS( FILE *in, FILE *out, NlsHeader *hdr )
{
    size_t   count = 0;
    size_t   items = ( hdr->size - sizeof( NlsHeader ) ) / sizeof( size_t );
    uint16_t hi;
    uint16_t lo;
    fputs( "  NLS DBCS Grammer Definition\n", out );
    fprintf( out, "    DbcsNlsGrammerDef.size:   %4.4x (%hu)\n", hdr->size, hdr->size );
    fprintf( out, "    DbcsNlsGrammerDef.type:   %s\n", getType( hdr->type ) );
    fprintf( out, "    DbcsNlsGrammerDef.format: %4.2x (%hu)\n", hdr->format, hdr->format );
    fputs( "    DbcsNlsGrammerDef data:\n", out );
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
