#include <windows.h>
#include <stdlib.h>

HPALETTE CreateDIBPalette( BITMAPINFO *info )
{
    unsigned            num_colours, i;
    LOGPALETTE          *palette;
    HPALETTE            palette_handle;
    RGBQUAD             *quads;

    num_colours = info->bmiHeader.biClrUsed;
    if( num_colours == 0 && info->bmiHeader.biBitCount != 24 ) {
        num_colours = 1 << info->bmiHeader.biBitCount;
    }

    palette_handle = (HPALETTE)0;

    if( num_colours ) {
        palette = malloc( sizeof( LOGPALETTE ) +
                num_colours * sizeof( PALETTEENTRY ) );
        if( palette == NULL ) return( (HPALETTE)0 );
        palette->palNumEntries = num_colours;
        palette->palVersion = 0x300;

        quads = &info->bmiColors[0];
        for( i = 0; i < num_colours; i++ ) {
            palette->palPalEntry[i].peRed = quads[i].rgbRed;
            palette->palPalEntry[i].peGreen = quads[i].rgbGreen;
            palette->palPalEntry[i].peBlue = quads[i].rgbBlue;
            palette->palPalEntry[i].peFlags = 0;
        }
        palette_handle = CreatePalette( palette );
        free( palette );
    }
    return( palette_handle );
}
