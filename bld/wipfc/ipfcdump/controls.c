// Dump the controls data

#include "ipfcdump.h"
#include <stdlib.h>

static size_t readCtrlString(FILE *, wchar_t *);

void readControls( FILE *in, FILE *out )
{
    fputs( "\nPanel Controls\n", out );
    if( eHdr.ctrlOffset ) {
        Controls ctrls;
        fseek( in, eHdr.ctrlOffset, SEEK_SET );
        fread( &ctrls, sizeof(Controls), 1, in );
        fprintf( out, "  Controls.controlCount: %4.4x (%hu)\n", ctrls.controlCount, ctrls.controlCount );
        fprintf( out, "  Controls.groupCount:   %4.4x (%hu)\n", ctrls.groupCount, ctrls.groupCount );
        fprintf( out, "  Controls.groupIndex:   %4.4x (%hu)\n", ctrls.groupIndex, ctrls.groupIndex );
        fprintf( out, "  Controls.reserved:     %4.4x (%hu)\n", ctrls.reserved, ctrls.reserved );
        if( ctrls.controlCount) {
            ControlDef cd;
            size_t  count;
            wchar_t name[ WSTRING_MAX_LEN ];
            for( count = 0; count < ctrls.controlCount; count++ ) {
                fread( &cd, sizeof( ControlDef ), 1, in );
                readCtrlString( in, name );
                fprintf( out, "  Control #%u\n", count );
                fprintf( out, "    ControlDef.type:  %4.4x (%hu)\n", cd.type, cd.type );
                fprintf( out, "    ControlDef.resid: %4.4x (%hu)\n", cd.resid, cd.resid );
                fprintf( out, "    ControlDef.text:  %ls\n", name );
            }
        }
        if( ctrls.groupCount ) {
            GroupDef gd;
            size_t count1;
            size_t count2;
            uint16_t index;
            for( count1 = 0; count1 < ctrls.groupCount; count1++ ) {
                fread( &gd, sizeof(GroupDef), 1, in );
                fprintf( out, "  Group #%u\n", count1 );
                fprintf( out, "    GroupDef.count: %4.4x (%hu)\n", gd.count, gd.count );
                fputs( "    GroupDef.data: ", out );
                for (count2 = 0; count2 < gd.count; count2++ ) {
                    fread( &index, sizeof(uint16_t), 1, in );
                    fprintf( out, "%4.4x (%hu) ", index, index );
                }
                fputc( '\n', out );
            }
        }
    }
    else
        fputs("  No panel controls found\n", out);
}
/*****************************************************************************/
size_t readCtrlString( FILE *in, wchar_t *buffer )
{
    char    temp[ STRING_MAX_LEN ];
    size_t  length = fgetc( in );
    fread( temp, sizeof( char ), length, in );
    temp[ length ] = '\0';
    length = mbstowcs( buffer, temp, WSTRING_MAX_LEN );
    return( length + 1 );
}
