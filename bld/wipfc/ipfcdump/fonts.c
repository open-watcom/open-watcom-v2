// Dumps the font data

#include "ipfcdump.h"

void readFonts(FILE *in, FILE *out)
{
    fputs( "\nFont Data\n", out );
    if( eHdr.fontCount ) {
        FontEntry fnt;
        size_t count;
        fseek( in, eHdr.fontOffset, SEEK_SET );
        for ( count = 0; count < eHdr.fontCount; count++ ) {
            fread( &fnt, sizeof(FontEntry), 1, in );
            fprintf( out, "  Font Entry #%u\n", count );
            fprintf( out, "    FontEntry.faceName: %s\n", fnt.faceName );
            fprintf( out, "    FontEntry.width:    %4.4x (%hu)\n", fnt.width, fnt.width );
            fprintf( out, "    FontEntry.height:   %4.4x (%hu)\n", fnt.height, fnt.height );
            fprintf( out, "    FontEntry.codePage: %4.4x (%hu)\n", fnt.codePage, fnt.codePage );
        }
    }
    else
        fputs("  No font data is present\n", out);
}
