/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ======================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Implements the functions declared in outbuff.h:
*                   ob_flush()
*                   ob_insert_block()
*                   ob_insert_byte()
*                   ob_insert_ps_text_end()
*                   ob_insert_ps_text_start()
*                   ob_setup()
*                   ob_teardown()
*               and the functions declared in wgml.h:
*                   cop_tr_table()
*                   ob_binclude()
*                   ob_graphic()
*                   ob_oc()
*               as well as these local variables:
*                   binc_buff
*                   buffout
*                   translated
*                   out_file_fp
*                   tr_table
*               and these local functions:
*                   ob_insert_ps_text()
*                   ob_insert_ps_cmd()
*                   ob_insert_ps_cmd_ot()
*                   ob_insert_def()
*                   ob_insert_def_ot()
*                   set_out_file()
*                   set_out_file_attr()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
****************************************************************************/


#include <ctype.h>
#include <errno.h>
#include "wgml.h"
#include "devfuncs.h"
#include "outbuff.h"

#ifndef __WATCOMC__
#include "clibext.h"
#endif


#ifdef __UNIX__
#define TEXT_EOL    "\n"
#else
#define TEXT_EOL    "\r\n"
#endif

/* Local variable declaration. */

static record_buffer    binc_buff;
static record_buffer    buffout;
static record_buffer    translated;
static FILE             *out_file_fp;
static unsigned char    tr_table[0x100]; // .TR-controlled translation table

/* Static function definitions. */

/* Function ob_insert_ps_text().
 * This function inserts text for a PostScript device into the output buffer.
 * PS text elements are allowed to completely fill the output record without
 * regard to word breaks. If the text element is not closed by the end of the
 * record, a '\' is used to terminate the record and the next record starts
 * with the next character. Output translation is always done; if it produces
 * a multibyte translation, then the entire translation must be placed in the
 * same buffer.
 *
 * Parameter:
 *      in_block contains the bytes to be inserted.
 *      count contains the number of bytes in the block.
 *      font contains the number of the font to be used.
 *
 * Notes:
 *      If this is the start of the text element, then the last item was "("
 *          and, if difference = 1, the resulting "(\" is correct for the end
 *          of the output record.
 *      If the text element ends on the last legal position, then inserting "\"
 *          and moving the last character in the text element to the next buffer
 *          is correct.
 *      If the text element ends one position before the last legal position,
 *          then the output record will correctly end with the ")" of ") sd"
 *          or ") shwd", as appropriate.
 *      Adjusting text_count as shown avoids a problem where a word was added
 *          without a final '\', which is not how wgml 4.0 does it and which
 *          produces an extra space when the PS interpreter processes the text.
 *      If a multibyte translation is too large to fit into the buffer, then an
 *          error is reported and the program stops. This should never happen
 *          since, for device PS, the buffer has 79 characters and the longest
 *          output translations have four characters.
 */

static void ob_insert_ps_text( const char *in_block, unsigned count, font_number font )
{
    unsigned            difference;
    translation         **cur_table  = NULL;
    translation         *cur_trans   = NULL;
    unsigned char       trbyte_out;
    unsigned char       trbyte_in;
    unsigned            i;

    /* If the buffer is full, flush it. */

    if( buffout.current == buffout.size )
        ob_flush();

    /* Adjust font if appropriate and initialize cur_trans. */

    if( font >= wgml_font_cnt )
        font = FONT0;
    if( wgml_fonts[font].outtrans != NULL )
        cur_table = wgml_fonts[font].outtrans->table;
    for( i = 0; i < count; i++ ) {
        difference = buffout.size - buffout.current;
        if ( difference > 1 ) {

            /* buffout has room for at least one character. */

            trbyte_in = in_block[i];
            trbyte_out = tr_table[trbyte_in];
            if( trbyte_out == trbyte_in ) {
                if( cur_table == NULL ) {

                    /* No output translation was found. */

                    buffout.text[buffout.current] = trbyte_out;
                    buffout.current++;
                } else {

                    /* An :OUTTRANS block exists. */

                    cur_trans = cur_table[trbyte_out];
                    if( cur_trans == NULL ) {

                        /* No output translation was found. */

                        buffout.text[buffout.current] = trbyte_out;
                        buffout.current++;
                    } else {

                        /* An :OUTTRANS block output translation was found. */

                        if( cur_trans->count == 1 ) {

                            /* This is a single-character output translation. */

                            buffout.text[buffout.current] = cur_trans->data[0];
                            buffout.current++;
                        } else {

                            /* This is a multi-character output translation. */

                            /* If it is too large to fit at all, report overflow. */

                            if( cur_trans->count > buffout.size ) {
                                xx_simple_err_exit_c( ERR_OUT_REC_SIZE, g_dev_name );
                                /* never return */
                            }

                            /* If it won't fit in the current buffer, finalize
                             * and flush the buffer.
                             */

                            if( (buffout.current + cur_trans->count + 1) > buffout.size ) {
                                buffout.text[buffout.current] = '\\';
                                buffout.current++;
                                ob_flush();
                            }

                            /* At this point, it is known that it will fit. */

                            memcpy( &buffout.text[buffout.current], cur_trans->data, cur_trans->count );
                            buffout.current += cur_trans->count;
                        }
                    }
                }
            } else {

                /* A single-byte .tr output translation was found. */

                buffout.text[buffout.current] = trbyte_out;
                buffout.current++;
            }
        } else {

            /* Finalize and flush the buffer. Adjust i so that, when
             * incremented at the top of the loop, it points to the same
             * character and that character is not skipped.
             */

            buffout.text[buffout.current] = '\\';
            buffout.current++;
            ob_flush();
            i--;
        }
    }

    return;
}

/* Function ob_insert_ps_cmd().
 * This function inserts PostScript language statements. No output
 * translation occurs.
 *
 * Parameter:
 *      in_block contains the bytes to be inserted.
 *      count contains the number of bytes in the block.
 *
 * Notes:
 *      PostScript language statements must not be split in mid-token.
 *      When deciding whether or not a token will fit into the current output
 *          record, the space following that token must be included in the
 *          length of the token.
 *      The first space following the last token in an output record does not
 *          appear in any output record.
 */

static void ob_insert_ps_cmd( const char *in_block, unsigned count )
{
    unsigned    current     = 0;
    unsigned    spc_end     = 0;
    unsigned    tkn_end     = 0;
    unsigned    spc_start   = 0;
    unsigned    tkn_start   = 0;
    unsigned    text_count  = count;

    /* If the buffer is full, flush it. */

    if( buffout.current == buffout.size ) {
        ob_flush();
    }

    /* Copy each token in turn. */

    while( (buffout.current + text_count) > buffout.size ) {

        /* If the entire text_block will now fit, exit the loop. */

        if( text_count <= buffout.size - buffout.current ) {
            break;
        }

        for( current = spc_start; current < count; current++ ) {
            if( in_block[current] != ' ' ) {
                break;                  // skip initial spaces
            }
        }

        tkn_start = current;
        for( ; current < count; current++ ) {
            if( in_block[current] == ' ' ) {
                break;                  // find end of token
            }
        }

        tkn_end = current;
        for( ; current < count; current++ ) {
            if( in_block[current] != ' ' ) {
                break;                  // skip following spaces
            }
        }
        spc_end = current;

        if( ( buffout.current + spc_end - spc_start ) <= buffout.size ) {

            /* Copy up the token and any initial space characters. */

            memcpy( &buffout.text[buffout.current], &in_block[spc_start], tkn_end - spc_start );
            buffout.current += tkn_end - spc_start;
            text_count -= tkn_end - spc_start;
            current = tkn_end;
            spc_start = current;
            spc_end = current;
            tkn_start = current;
        } else {
            ob_flush();
            current = tkn_start;
            text_count -= tkn_start - spc_start;
        }
    }

    /* Special for " rmoveto" from pre_text_output() for "sup" */

    text_count = strlen( &in_block[current] );

    /* Insert any remaining text. */

    if( text_count > 0 ) {
        memcpy( &buffout.text[buffout.current], &in_block[current], text_count );
        buffout.current += text_count;
    }

    /* If the buffer is full, flush it. */

    if( buffout.current == buffout.size )
        ob_flush();

    return;
}

/* Function ob_insert_ps_cmd_ot().
 * This function inserts PostScript language statements, with output
 * translation.
 *
 * Parameter:
 *      in_block contains the bytes to be inserted.
 *      count contains the number of bytes in the block.
 *      font contains the number of the font to be used.
 *
 * Notes:
 *      PostScript language tokens are translated before it is determined
 *          whether or not they will fit in the current output buffer.
 *      PostScript language statements must not be split in mid-token.
 *      When deciding whether or not a token will fit into the current output
 *          record, the space following that token must be included in the
 *          length of the token.
 *      The first space following the last token in an output record does not
 *          appear in any output record.
 */

static void ob_insert_ps_cmd_ot( const char *in_block, unsigned count, font_number font )
{
    unsigned            test_length;
    unsigned            text_count;
    translation         **cur_table  = NULL;
    translation         *cur_trans   = NULL;
    unsigned char       trbyte_out;
    unsigned char       trbyte_in;
    unsigned            i;
    unsigned            j;
    unsigned            k;

    /* Adjust font if necessary and initialize cur_table and text_count. */

    if( font >= wgml_font_cnt )
        font = FONT0;
    if( wgml_fonts[font].outtrans != NULL )
        cur_table = wgml_fonts[font].outtrans->table;
    text_count = count;
    for( i = 0; i < count; i++ ) {

        /* If the buffer is full, flush it. */

        if( buffout.current == buffout.size )
            ob_flush();

        /* First process any initial space. */

        if( in_block[i] == ' ' ) {

            /* Now check for an output translation. */

            trbyte_in = in_block[i];
            trbyte_out = tr_table[trbyte_in];
            if( trbyte_out == trbyte_in ) {
                if( cur_table == NULL ) {

                    /* No output translation was found. */

                    buffout.text[buffout.current] = trbyte_out;
                    buffout.current++;
                } else {

                    /* An :OUTTRANS block exists. */

                    cur_trans = cur_table[trbyte_out];
                    if( cur_trans == NULL ) {

                        /* No output translation was found. */

                        buffout.text[buffout.current] = trbyte_out;
                        buffout.current++;
                    } else {

                        /* An :OUTTRANS block output translation was found. */

                        if( cur_trans->count == 1 ) {

                            /* This is a single-character output translation. */

                            buffout.text[buffout.current] = cur_trans->data[0];
                            buffout.current++;
                        } else {

                            /* This is a multi-character output translation. */

                            /* If it is too large to fit at all, report overflow. */

                            if( cur_trans->count > buffout.size ) {
                                xx_simple_err_exit_c( ERR_OUT_REC_SIZE, g_dev_name );
                                /* never return */
                            }

                            /* If it won't fit, flush the buffer. */

                            if( (buffout.current + cur_trans->count) > buffout.size ) {
                                ob_flush();
                            }

                            /* At this point, it is known that it will fit. */

                            memcpy( &buffout.text[buffout.current], cur_trans->data, cur_trans->count );
                            buffout.current += cur_trans->count;
                        }
                    }
                }
            } else {

                /* A single-byte .tr output translation was found. */

                buffout.text[buffout.current] = trbyte_out;
                buffout.current++;
            }
            text_count--;

            /* Process next character, might be another space. */

            continue;
        }

        /* Get the next token and translate it. */

        k = 0;
        for( j = i; j < count; j++ ) {

            /* in_block[i] points to a non-space character. */

            if( in_block[j] == ' ' )
                break;

            /* At least one character will be added to translated. */

            resize_record_buffer( &translated, k );

            /* Add the non-space character to translated. */

            trbyte_in = in_block[j];
            trbyte_out = tr_table[trbyte_in];
            if( trbyte_out == trbyte_in ) {
                if( wgml_fonts[font].outtrans == NULL ) {

                    /* No translation exists: copy the character. */

                    translated.text[k] = trbyte_out;
                    k++;
                } else {

                    /* An :OUTTRANS block exists. */

                    cur_trans = cur_table[trbyte_out];
                    if( cur_trans == NULL ) {

                        /* No output translation was found. */

                        translated.text[k] = trbyte_out;
                        k++;
                    } else {

                        /* An :OUTTRANS block output translation was found. */

                        if( cur_trans->count == 1 ) {

                            /* Single-char translation found. */

                            translated.text[k] = cur_trans->data[0];
                            k++;
                        } else {

                            /* Multi-char translation found. */

                            /* If it is too large to fit at all, report overflow. */

                            if( cur_trans->count > buffout.size ) {
                                xx_simple_err_exit_c( ERR_OUT_REC_SIZE, g_dev_name );
                                /* never return */
                            }

                            resize_record_buffer( &translated, k + cur_trans->count );
                            memcpy( &translated.text[k], cur_trans->data, cur_trans->count );
                            k += cur_trans->count;
                        }
                    }
                }
            } else {

                /* A single-byte .tr translation found. */

                translated.text[k] = trbyte_out;
                k++;
            }
        }
        translated.current = k;
        i = j;

        /* If a space followed the current token, test_length must include it. */

        test_length = buffout.current + translated.current;
        if( in_block[i] == ' ' )
            test_length++;

        /* If the token won't fit, flush the buffer. The increment at the top
         * of the loop will skip any following space character; since this is
         * not correct except when the buffer has been flushed, decrement i.
         * text_count is decremented if the flush occurs because the space
         * will be skipped, but not otherwise since it will be processed.
         */

        if( test_length >= buffout.size ) {
            ob_flush();
            text_count--;
        } else {
            i--;
        }

        /* Now insert the translated token into the buffer. */

        memcpy( &buffout.text[buffout.current], translated.text, translated.current );
        buffout.current += translated.current;
        text_count -= translated.current;
    }
    return;
}

/* Function ob_insert_def().
 * This function inserts a block of bytes into the output buffer. This is done
 * byte-by-byte because the block may include nulls. If the entire block won't
 * fit, as much of it as possible is copied into the buffer, which is then
 * flushed and the rest of the block processed. No output translation occurs.
 *
 * Parameter:
 *      in_block contains the bytes to be inserted.
 *      count contains the number of bytes in the block.
 */

static void ob_insert_def( const char *in_block, unsigned count )
{
    unsigned    current;
    unsigned    difference;
    unsigned    text_count;

    /* If the buffer is full, flush it. */

    if( buffout.current == buffout.size )
        ob_flush();

    /* Start at the beginning of text_block. */

    current = 0;
    text_count = count;
    while( (buffout.current + text_count) > buffout.size ) {

        difference = buffout.size - buffout.current;

        /* If the entire text_block will now fit, exit the loop. */

        if( text_count <= difference )
            break;

        memcpy( &buffout.text[buffout.current], &in_block[current], difference );
        buffout.current += difference;
        current += difference;
        text_count -= difference;
        ob_flush();
    }

    /* Insert any remaining text. */

    if( text_count > 0 ) {
        memcpy( &buffout.text[buffout.current], &in_block[current], text_count );
        buffout.current += text_count;
    }

    /* If the buffer is full, flush it. */

    if( buffout.current == buffout.size )
        ob_flush();

    return;
}

/* Function ob_insert_def_ot().
 * This function inserts a block of bytes into the output buffer with output
 * translation. This is done byte-by-byte because the block may include nulls.
 * If the entire block won't fit, as much of it as possible is copied into the
 * buffer, which is then flushed and the rest of the block processed.
 *
 * Parameter:
 *      in_block contains the bytes to be inserted.
 *      count contains the number of bytes in the block.
 *      font contains the number of the font to be used.
 * Notes:
 *      Multi-character output translations cannot be split between output
 *          records, so a given output record may have fewer than the maximum
 *          allowed number of characters in it.
 */

static void ob_insert_def_ot( const char *in_block, unsigned count, font_number font )
{
    unsigned            text_count;
    translation         **cur_table  = NULL;
    translation         *cur_trans   = NULL;
    unsigned char       trbyte_out;
    unsigned char       trbyte_in;
    unsigned            i;

    /* Adjust font if necessary and initialize cur_table and text_count. */

    if( font >= wgml_font_cnt )
        font = FONT0;
    if( wgml_fonts[font].outtrans != NULL )
        cur_table = wgml_fonts[font].outtrans->table;
    text_count = count;

    for( i = 0; i < count; i++ ) {

        /* If the buffer is full, flush it. */

        if( buffout.current == buffout.size )
            ob_flush();

        /* Now check for an output translation. */

        trbyte_in = in_block[i];
        trbyte_out = tr_table[trbyte_in];
        if( trbyte_out == trbyte_in ) {
            if( cur_table == NULL ) {

                /* No output translation was found. */

                buffout.text[buffout.current] = trbyte_out;
                buffout.current++;
            } else {

                /* An :OUTTRANS block exists. */

                cur_trans = cur_table[trbyte_out];
                if( cur_trans == NULL ) {

                    /* No output translation was found. */

                    buffout.text[buffout.current] = trbyte_out;
                    buffout.current++;
                } else {

                    /* An :OUTTRANS block output translation was found. */

                    if( cur_trans->count == 1 ) {

                        /* This is a single-character output translation. */

                        buffout.text[buffout.current] = cur_trans->data[0];
                        buffout.current++;
                    } else {

                        /* This is a multi-character output translation. */

                        /* If it is too large to fit at all, report overflow. */

                        if( cur_trans->count > buffout.size ) {
                            xx_simple_err_exit_c( ERR_OUT_REC_SIZE, g_dev_name );
                            /* never return */
                        }

                        /* If it won't fit in the current buffer, flush the buffer. */

                        if( (buffout.current + cur_trans->count) > buffout.size ) {
                            ob_flush();
                        }

                        /* At this point, it is known that it will fit. */

                        memcpy( &buffout.text[buffout.current], cur_trans->data, cur_trans->count );
                        buffout.current += cur_trans->count;
                    }
                }
            }
        } else {

            /* A single-byte .tr output translation was found. */

            buffout.text[buffout.current] = trbyte_out;
            buffout.current++;
        }
        text_count--;
    }

    /* If the buffer is full, flush it. */

    if( buffout.current == buffout.size )
        ob_flush();

    return;
}

/* Function set_out_file().
 * Sets the global out_file to the correct value. This can be anything from a
 * filename entered on the command line to one constructed using bits from the
 * command line and the output_filename field in the :DEVICE block.
 */

static void set_out_file( void )
{
    char        cmd_outfile[_MAX_PATH2];
    char    *   cmd_drive;
    char    *   cmd_dir;
    char    *   cmd_ext;
    char    *   cmd_fname;
    char        dev_outfile[_MAX_PATH2];
    char    *   dev_drive;
    char    *   dev_dir;
    char    *   dev_ext;
    char    *   dev_fname;
    char        doc_spec[_MAX_PATH2];
    char    *   doc_drive;
    char    *   doc_dir;
    char    *   doc_ext;
    char    *   doc_fname;
    char        temp_outfile[_MAX_PATH];

    /* Split the possible source names into their component parts. */

    if( master_fname == NULL ) {
        doc_spec[0] = '\0';
        doc_drive = &doc_spec[0];
        doc_spec[1] = '\0';
        doc_dir = &doc_spec[1];
        doc_spec[2] = '\0';
        doc_fname = &doc_spec[2];
        doc_spec[3] = '\0';
        doc_ext = &doc_spec[3];
    } else {
        _splitpath2( master_fname, doc_spec, &doc_drive, &doc_dir, &doc_fname,
                     &doc_ext );
    }

    if( out_file == NULL ) {
        cmd_outfile[0] = '\0';
        cmd_drive = &cmd_outfile[0];
        cmd_outfile[1] = '\0';
        cmd_dir = &cmd_outfile[1];
        cmd_outfile[2] = '\0';
        cmd_fname = &cmd_outfile[2];
        cmd_outfile[3] = '\0';
        cmd_ext = &cmd_outfile[3];
    } else {
        _splitpath2( out_file, cmd_outfile, &cmd_drive, &cmd_dir, &cmd_fname,
                     &cmd_ext );
    }

    if( bin_device->output_name == NULL ) {
        dev_outfile[0] = '\0';
        dev_drive = &dev_outfile[0];
        dev_outfile[1] = '\0';
        dev_dir = &dev_outfile[1];
        dev_outfile[2] = '\0';
        dev_fname = &dev_outfile[2];
        dev_outfile[3] = '\0';
        dev_ext = &dev_outfile[3];
    } else {
        _splitpath2( bin_device->output_name, dev_outfile, &dev_drive, &dev_dir,
                     &dev_fname, &dev_ext );
    }

    /* Ensure it is possible to tell if a file name was constructed. */

    temp_outfile[0] = '\0';

    /* Construct the file name, if necessary. If the command-line option OUTput
     * was used and both a filename and and extension were given, then cmd_fname
     * will be used as-is and temp_outfile will not be touched.
     */

    if( *cmd_fname != '\0' ) {
        if( *cmd_fname != '*' ) {
            if( *cmd_ext != '\0' ) {

            /* If both name and extension were given on the command line, use
             * out_file as-is.
             */

            } else {

            /* If the name was given on the command line, use out_file with the
             * extension given in the :DEVICE block.
             */

                _makepath( temp_outfile, cmd_drive, cmd_dir, cmd_fname,
                           bin_device->output_extension );
            }
        } else {
            if( *cmd_ext != '\0' ) {

            /* If the name was not given but an extension was given on the command
             * line, use the document specification name with the extension given.
             */

                _makepath( temp_outfile, cmd_drive, cmd_dir, doc_fname, cmd_ext );
            } else {

            /* If neither a specific name nor an extension was given on the
             * command line, use use the document specification name with the
             * extension given in the :DEVICE block.
             */

                _makepath( temp_outfile, cmd_drive, cmd_dir, doc_fname,
                           bin_device->output_extension );
            }
        }
    } else {
        if( (*cmd_drive != '\0') || (*cmd_dir != '\0') ) {

            /* Command line OPTION was used with something like "c:" or "..\" but
             * with no filename or extension.
             */

                _makepath( temp_outfile, cmd_drive, cmd_dir, doc_fname,
                           bin_device->output_extension );
        } else {

            /* The situation here is that command-line option OUTPUT was not
             * used with only a drive letter and/or a path but with no file name,
             * not even "*", and no extension. In other words, it was not used
             * at all.
             */

            if( (*dev_fname != '*') && (*dev_fname != '\0') ) {

                /* If the :DEVICE block specified a file name then use the file
                 * name and any extension provided.
                 */

                _makepath( temp_outfile, "", "", bin_device->output_name,
                           bin_device->output_extension );
            } else {

                /* If the :DEVICE block did not specify a file name then use the
                 * document specification name with any extension provided.
                 */

                _makepath( temp_outfile, dev_drive, dev_dir, doc_fname,
                           bin_device->output_extension );
            }
        }
    }

    /* If a file name was constructed, update out_file with it. */

    if( temp_outfile[0] != '\0' ) {
        if( out_file != NULL )
            mem_free( out_file );
        out_file = mem_alloc( strlen( temp_outfile ) + 1 );
        strcpy( out_file, temp_outfile );
    }

    return;
}

/* Function set_out_file_attr().
 * Sets the global out_file_attr to the correct value. This will be either the
 * record type entered on the command line, the spec_rec field in the :DRIVER
 * block, or the default record type "t:132".
 */

static void set_out_file_attr( void )
{
    unsigned    len;

    /* Construct the output file record type if necessary. If the command-line
     * option OUTput was used and a record type was given, then out_file_attr
     * will be used as-is. Otherwise, the rec_spec will be used if it is
     * properly formatted. If all else fails, the default will be used.
     */

    if( out_file_attr == NULL ) {
        if( bin_driver->rec_spec != NULL ) {
            len = strlen( bin_driver->rec_spec );
            if( (bin_driver->rec_spec[0] != '(') ||
                (bin_driver->rec_spec[len - 1] != ')')) {

                /* Use default if rec_spec is badly-formed. */

                out_file_attr = mem_strdup( "t:132" );

            } else {

                /* Copy the record type itself, without parentheses, into
                 * out_file_attr.
                 */

                len -= 1;
                out_file_attr = mem_tokdup( &bin_driver->rec_spec[1], len - 1 );
            }
        } else {

            /* Use default if bin_driver->rec_spec is missing. */

            out_file_attr = mem_strdup( "t:132" );
        }
    }

    return;
}

/* Global function definitions. */

/***************************************************************************/
/* update tr_table as specified by the data                                */
/***************************************************************************/

void cop_tr_table( const char *p )
{

    bool            first_found;
    bool            no_data;
    const char      *pa;
    int             i;
    unsigned char   token_char;
    unsigned char   first_char;
    unsigned        len;

    char            cwcurr[4];
    cwcurr[0] = SCR_char;
    cwcurr[1] = 't';
    cwcurr[2] = 'r';
    cwcurr[3] = '\0';

    // if there is no data, then the table will be reset
    first_found = false;
    no_data = true;

    while( *p != '\0' ) {
        SkipSpaces( p );        // next char start
        pa = p;
        SkipNonSpaces( p );     // next char start
        len = p - pa;
        if( len == 0 )
            break;              // exit loop if no next char

        token_char = parse_char( pa, len );
        no_data = false;

        if( first_found ) {     // we now have two chars
            tr_table[first_char] = token_char;
            first_found = false;
        } else {                // we found a first or only char
            first_char = token_char;
            first_found = true;
        }
    }

    if( first_found ) {     // we now have two chars
        tr_table[first_char] = first_char;
    }

    if( no_data ) {         // reset the table if no_data is still true
        for( i = 0; i < 0x100; i++ ) {
            tr_table[i] = i;
        }
    }

    return;
}

static void emit_FONT0( void )
{
#define TO_FONT0    "@fs0 "

    ob_flush();
    strcpy( buffout.text, TO_FONT0 );
    buffout.current = sizeof( TO_FONT0 ) - 1;

#undef TO_FONT0
}

/* Function ob_binclude().
 * This function implements the action of BINCLUDE.
 * NOTE: BINCLUDE produces a doc_element only if the file has been found.
 */

void ob_binclude( binclude_element * in_el )
{
    unsigned    count;

    fb_binclude_support( in_el );
    if( fwrite( buffout.text, sizeof( uint8_t ), buffout.current, out_file_fp ) < buffout.current ) {
        xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
        /* never return */
    }
    buffout.current = 0;

    if( in_el->has_rec_type ) {
        count = fread( buffout.text, sizeof( uint8_t ), buffout.size, in_el->fp );
        while( count == buffout.size ) {
            buffout.current = count;
            if( fwrite( buffout.text, sizeof( uint8_t ), buffout.current, out_file_fp ) < buffout.current ) {
                xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
                /* never return */
            }
            count = fread( buffout.text, sizeof( uint8_t ), buffout.size, in_el->fp );
        }
        buffout.current = count;
        if( fwrite( buffout.text, sizeof( uint8_t ), buffout.current, out_file_fp ) < buffout.current ) {
            xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
            /* never return */
        }
        if( ferror( in_el->fp ) ) {
            xx_simple_err_exit_cc( ERR_IN_FILE, "BINCLUDE", in_el->file );
            /* never return */
        }
        buffout.current = 0;
    } else {
        count = fread( binc_buff.text, sizeof( uint8_t ), binc_buff.size, in_el->fp );
        while( count == binc_buff.size ) {
            binc_buff.current = count;
            if( fwrite( binc_buff.text, sizeof( uint8_t ), binc_buff.current, out_file_fp ) < binc_buff.current ) {
                xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
                /* never return */
            }
            ob_flush();
            count = fread( binc_buff.text, sizeof( uint8_t ), binc_buff.size, in_el->fp );
        }
        binc_buff.current = count;
        if( fwrite( binc_buff.text, sizeof( uint8_t ), binc_buff.current, out_file_fp ) < binc_buff.current ) {
            xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
            /* never return */
        }
        if( ferror( in_el->fp ) ) {
            xx_simple_err_exit_cc( ERR_IN_FILE, "BINCLUDE", in_el->file );
            /* never return */
        }
    }

    if( in_el->force_FONT0 ) {          // force fs0 under some conditions; not PS? tbd
        emit_FONT0();
    }
//    fclose( in_el->fp );
//    in_el->fp = NULL;
//    mem_free( in_el->file );
}


/* Function ob_oc().
 * This function implements the action of control word OC.
 */

void ob_oc( const char *text )
{
    if( ProcFlags.has_aa_block ) {
        set_oc_pos();
    }
    ob_flush();
    ob_insert_block( text, strlen( text ), false, false, g_curr_font );
    if( ProcFlags.has_aa_block ) {
        if( (text[strlen( text ) - 1]) != ' ' ) {
            ob_insert_byte( ' ' );
        }
        set_oc_pos();
    }
    ob_flush();
    return;
}


/* Function ob_flush().
 * This function actually flushes the output buffer to the output device/file.
 *
 * Notes:
 *      The output file is, and must be, opened in binary mode. This requires
 *          the explicit emission of end-of-line. For *NIX platforms "\n" is emitted,
 *          for others "\r\n" is emitted.
 */

void ob_flush( void )
{
    if( out_file_fp == NULL )
        return;     /* Bail in case of fatal errors further up. */

    if( fwrite( buffout.text, sizeof( uint8_t ), buffout.current, out_file_fp ) < buffout.current ) {
        xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
        /* never return */
    }
    buffout.current = 0;
    /*
     * emit correct end-of-line bytes
     */
    if( fprintf( out_file_fp, TEXT_EOL ) != sizeof( TEXT_EOL ) - 1 ) {
        xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
        /* never return */
    }
}

/* Function ob_graphic().
 * This function implements the action of GRAPHIC for the PS device.
 * NOTE: GRAPHIC produces a doc_element only if the file has been found.
 */

void ob_graphic( graphic_element * in_el )
{
    char        begindoc[] = "%%BeginDocument: ";
    char        enddoc[] = "%%EndDocument";
    char        graphobj[] = "/graphobj save def /showpage { } def";
    char        restore[] = "graphobj restore";
    unsigned    ps_size;
    unsigned    count;

    fb_graphic_support( in_el );
    ob_flush();

    ps_size = strlen( graphobj );
    strcpy( buffout.text, graphobj );
    buffout.current = ps_size;
    ob_flush();

    memset( buffout.text, '\0', buffout.size );
    ps_size = sprintf( buffout.text, "%d %d %d %d %d %d %d graphhead",
                         in_el->cur_left, in_el->y_address, in_el->width, in_el->depth,
                         in_el->xoff, -1 * (in_el->depth + in_el->yoff), in_el->scale );
    buffout.current = strlen( buffout.text );
    ob_flush();

    ps_size = strlen( begindoc );
    strcpy( buffout.text, begindoc );
    buffout.current = ps_size;
    strcpy( buffout.text + ps_size, in_el->short_name );
    buffout.current = strlen( buffout.text );
    ob_flush();

    count = fread( buffout.text, sizeof( uint8_t ), buffout.size, in_el->fp );
    while( count == buffout.size ) {
        buffout.current = count;
        if( fwrite( buffout.text, sizeof( uint8_t ), buffout.current, out_file_fp )
                < buffout.current ) {
            xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
            /* never return */
        }
        count = fread( buffout.text, sizeof( uint8_t ), buffout.size, in_el->fp );
    }
    buffout.current = count;
    if( fwrite( buffout.text, sizeof( uint8_t ), buffout.current, out_file_fp )
            < buffout.current ) {
        xx_simple_err_exit_c( ERR_WRITE_OUT_FILE, out_file );
        /* never return */
    }
    buffout.current = 0;

    ps_size = strlen( enddoc );
    strcpy( buffout.text, enddoc );
    buffout.current = ps_size;
    ob_flush();

    ps_size = strlen( restore );
    strcpy( buffout.text, restore );
    buffout.current = ps_size;
    ob_flush();

    /* Only if font in use after the GRAPHIC was not font 0 */

    if( in_el->next_font > FONT0 ) {
        emit_FONT0();
        if( ferror( in_el->fp ) ) {
            xx_simple_err_exit_cc( ERR_IN_FILE, "GRAPHIC", in_el->file );
            /* never return */
        }
    }
//    fclose( in_el->fp );
//    in_el->fp = NULL;
//    mem_free( in_el->file );
//    mem_free( in_el->short_name );
}

/* Function ob_insert_block().
 * This function inserts a block of bytes into the output buffer by invoking
 * the appropriate static function based on flags and parameters. The block
 * of bytes may contains nulls.
 *
 * Parameter:
 *      in_block contains the bytes to be inserted.
 *      count contains the number of bytes in the block.
 *      out_trans is true if the bytes are to be translated before insertion.
 *      out_text is true is the bytes are to appear in the document itself.
 */

void ob_insert_block( const char *in_block, unsigned count, bool out_trans,
                      bool out_text, font_number font )
{
    /* Select and invoke the proper static function. */

    if( ProcFlags.ps_device )  {
        if( out_text ) {
            ob_insert_ps_text( in_block, count, font );
        } else {
            if( out_trans ) {
                ob_insert_ps_cmd_ot( in_block, count, font );
            } else {
                ob_insert_ps_cmd( in_block, count );
            }
        }
    } else {
        if( out_trans ) {
            ob_insert_def_ot( in_block, count, font );
        } else {
            ob_insert_def( in_block, count );
        }
    }
    return;
}

/* Function ob_insert_byte().
 * This function inserts a single byte into the output buffer.
 *
 * Parameter:
 *      in_char contains the byte to be inserted.
 */

void ob_insert_byte( unsigned char in_char )
{
    /* Flush the buffer if it is full. */

    if( buffout.current == buffout.size )
        ob_flush();

    /* Insert the character and increment the current position pointer. */

    buffout.text[buffout.current] = in_char;
    buffout.current++;

    return;
}

/* Function ob_insert_ps_text_end().
 * This function inserts the text end character and output macro for the PS
 * device.
 *
 * Parameter:
 *  htab_done indicates whether "sd" or "shwd" is to be used
 *
 * Note:
 *      This is only called for the PS device, to insert the ")" and the
 *      appropriate macro after text.
 */

void ob_insert_ps_text_end( bool htab_done, font_number font )
{
    char        shwd_suffix[]   = " shwd ";
    char        sd_suffix[]     = " sd ";
    unsigned    ps_size;

    ob_insert_block( ")", 1, false, false, font );

    if( htab_done ) {
        ps_size = strlen( sd_suffix );
        ob_insert_block( sd_suffix, ps_size, false, false, font );
    } else {
        ps_size = strlen( shwd_suffix );
        ob_insert_block( shwd_suffix, ps_size, false, false, font );
    }

    return;
}


/* Function ob_insert_ps_text_start().
 * This function inserts the text start character for the PS device.
 *
 * Note:
 *      This is only called for the PS device, to insert the "(" before text.
 *      It turns out that ending the record with "(" results in the newline
 *      produced by ob_flush() appearing as an extra space in the document,
 *      and that this is not how it is done by wgml 4.0.
 */

void ob_insert_ps_text_start( void )
{
    /* Flush the buffer if it is full or has only one character position left. */

    if( buffout.current >= (buffout.size - 1) )
        ob_flush();

    /* Insert '(' and increment the current position pointer. */

    buffout.text[buffout.current] = '(';
    buffout.current++;

    return;
}

/* Function ob_setup().
 * Determines the output file/device name and format. Opens the output
 * file/device (if appropriate) and initializes the output buffer itself.
 */

void ob_setup( void )
{
    unsigned        i;
    unsigned        count;
    unsigned long   num;

    /* Finalize out_file and out_file_attr. */

    set_out_file();
    set_out_file_attr();

    /* The record type must be "t"; it must have only one character and be */
    /* followed by ":".                                                    */

    if( ( my_tolower( out_file_attr[0] ) != 't' ) || ( out_file_attr[1] != ':' ) ) {
        xx_simple_err_exit_c( ERR_REC_ATT_NOT_SUP, out_file_attr );
        /* never return */
    }

    /* The rest of the record type must be numeric. */

    count = 0;

    for( i = 2; i < strlen( out_file_attr ); i++ ) {
        if( !my_isdigit( out_file_attr[i] ) ) {
            xx_simple_err_exit_c( ERR_REC_ATT_BAD_FMT, out_file_attr );
            /* never return */
        }
        count++;
    }

    num = strtoul( &out_file_attr[2], NULL, 0 );
#if !defined( __WATCOMC__ ) && defined( __UNIX__ )
    if( errno == ERANGE || num > UINT_MAX - 1 ) {
#else
    if( errno == ERANGE ) {
#endif
        xx_simple_err_exit_i( ERR_OUT_REC_SIZE2, UINT_MAX );
        /* never return */
    }

    /* Initialize the local variables. */

    init_record_buffer( &binc_buff, 80 );
    init_record_buffer( &buffout, num );
    init_record_buffer( &translated, 80 );

    /* Create (truncate) the output file. */

    out_file_fp = fopen( out_file, "wb" );
    if( out_file_fp == NULL ) {
        xx_simple_err_exit_c( ERR_OPEN_OUT_FILE, out_file );
        /* never return */
    }

    /* Initialize tr_table. */

    for( i = 0; i < 0x100; i++) {
        tr_table[i] = i;
    }
    return;
}

/* Function ob_teardown().
 * Determines the output file/device name and format. Opens the output
 * file/device (if appropriate) and initializes the output buffer itself.
 */

void ob_teardown( void )
{
    if( binc_buff.text != NULL ) {
        mem_free( binc_buff.text );
        binc_buff.text = NULL;
    }

    if( buffout.text != NULL ) {
        mem_free( buffout.text );
        buffout.text = NULL;
    }

    if(translated.text != NULL ) {
        mem_free( translated.text );
        translated.text = NULL;
    }

    if( out_file_fp != NULL ) {
        if( fclose( out_file_fp ) ) {
            xx_simple_err_exit_c( ERR_CLOSE_OUT_FILE, out_file );
            /* never return */
        }
        out_file_fp = NULL;
    }

    return;
}

