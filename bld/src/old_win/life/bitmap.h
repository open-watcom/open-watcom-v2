#define BITMAP_TYPE     ( (((WORD)'M')<<8)+'B' )

/* this macros determines the number of bytes of storage needed by a bitmap */
#define BITS_TO_BYTES( x, y )   ( ( ( ( x ) + 31 ) / 32 ) * 4 * ( y ) )

#define BITMAP_INFO_SIZE( bm )  ( sizeof(BITMAPINFO) + \
                                    sizeof(RGBQUAD) * ((1<<(bm)->biBitCount)-1))


/* this function is only for use inside the resource library */

extern HBITMAP ReadBitmapFile( HWND, char *);
extern HBITMAP RequestBitmapFile( void );
