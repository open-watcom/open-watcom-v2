/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
*
*  ========================================================================
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
* Description: implement .ga script control word (define GML Tag attributes)
*              not all options are used  / implemented
*
*  comments are from script-tso.txt
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1       /* use safer C library             */

#include "wgml.h"
#include "gvars.h"


/***************************************************************************/
/* GML ATTRIBUTE defines an attribute of a GML tag.  Multiple uses of the  */
/* control word are needed to define the  list of values that this attri-  */
/* bute accepts.                                                           */
/*                                                                         */
/*      旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴커       */
/*      |       |                                                  |       */
/*      |  .GA  |    tagname|*  attname|*  <opA>  <opV>            |       */
/*      |       |                                                  |       */
/*      읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴켸       */
/*                                                                         */
/* This control word defines an attribute of  a GML tagname that has been  */
/* previously  defined by  the GML  Tag  (GT)  control  word.   (See  the  */
/* description of GT for information on the internal data structures that  */
/* SCRIPT creates from the definitions of a GML tagset.)                   */
/*                                                                         */
/* tagname|*:  The name of  the tag for which this is  an attribute.   If  */
/*    the tagname does not exist in  the internal GML data structure,  an  */
/*    error message is produced and no  further processing occurs.   If *  */
/*    is  specified instead  of  a tagname,   the  most recently  defined  */
/*    tagname will be used.   There  is processing-performance benefit to  */
/*    defining a tagname (GT)  and then following it immediately with all  */
/*    of the attribute definitions (GA)  using  the * notation instead of  */
/*    the tagname,  since  it avoids repeatedly causing  SCRIPT to search  */
/*    for the tagname in its internal data structures.                     */
/* attname|*:  Specifies the name of the attribute being defined for this  */
/*    tagname.  The attname may contain a maximum of 9 alphanumeric char-  */
/*    acters.   Once  an attname has  been defined,  the  subsequent .GAs  */
/*    immediately following may  use the * notation  to define additional  */
/*    values for that same attribute.   (See above comment on performance  */
/*    benefits for the * notation.)                                        */
/* opA:  This is a list of one or more "attribute operands" (see below).   */
/* opV:  This is an "attribute value" specification (see below).           */
/*                                                                         */
/* Attribute Operands (opA)                                                */
/*                                                                         */
/* In the definition  of most GML tagsets,  each  tagname definition (GT)  */
/* will be followed immediately by all of the attribute/value definitions  */
/* (GA)  for that tagname.   The "opA" operands specify information about  */
/* the attribute itself.   Any number of  these operands may be specified  */
/* on the same control word.                                               */
/*                                                                         */
/* OFF|ON:   OFF  causes  user  specification of  this  attribute  to  be  */
/*    ignored.   The value the user has  specified for the attribute will  */
/*    be verified, but will not be presented to the tag's APF macro.   ON  */
/*    terminates a previous  OFF condition;  no error results  if the OFF  */
/*    condition was not in effect.                                         */
/* UPpercase:   The  attribute  value  specified  by  the  user  will  be  */
/*    converted to uppercase  before being verified,  and  the uppercased  */
/*    result will be presented to the APF macro.                           */
/* REQuired:  The  attribute is  always required when  this tag  is used.  */
/*    Failure to specify the attribute will result in an error message.    */
/*                                                                         */
/* Attribute Value Operands (opV)                                          */
/*                                                                         */
/* While any  number of attribute operands  (opA)  may be specified  on a  */
/* single .GA,  only one of the following attribute-value operands may be  */
/* specified per .GA control word.                                         */
/*                                                                         */
/* AUTOmatic  'value   string':   This  attname  will   automatically  be  */
/*    presented to  the APF macro,  with  the "value string" text  as its  */
/*    value.   Any  attempt by  the user to  specify this  attribute will  */
/*    result in an error message.                                          */
/* LENgth maxlength:  This attribute accepts  any value whose length does  */
/*    not exceed "maxlength" characters.                                   */
/* RANge minvalue maxvalue <default1 <default2>>:  This attribute accepts  */
/*    a range of  numeric values.   If a "default1" value  is also speci-  */
/*    fied,  that default value will be presented to the APF macro if the  */
/*    user does not specify the attribute,   or if the user specifies the  */
/*    attribute with no value.   If a "default2" value is also specified,  */
/*    the "default1" value will be presented to the APF macro if the user  */
/*    does not specify  the attribute,  and the "default2"  value will be  */
/*    presented if the user specifies the attribute with no value.         */
/* VALue valname  <DEFault>:  This  defines "valname" as  one value  in a  */
/*    list of values that the attribute accepts.  The valname may contain  */
/*    a maximum  of ten  characters,  and  may be  specified as  a quoted  */
/*    string if it contains embedded blanks.    If DEFault is also speci-  */
/*    fied,  then this valname will be used as the default value and will  */
/*    be presented  to the  APF macro if  the user  does not  specify the  */
/*    attribute.                                                           */
/* VALue valname USE 'value string' <DEFault>:  As above, with the excep-  */
/*    tion that the "value string" will be  presented to the APF macro as  */
/*    the value  of the  attribute,  instead  of "valname".    The "value  */
/*    string" may contain a maximum of 50 characters.                      */
/* ANY <'value string'>:  This attribute accepts any value.   If a "value  */
/*    string"  is  also  specified,   then the  "value  string"  will  be  */
/*    presented to the APF macro instead of the user-specified value.      */
/* RESET valname|'value string'|numeric:   This form of the  control word  */
/*    resets an  attribute's currently-defined AUTOmatic  'value string',  */
/*    RANge default,  VALue valname DEFault,  or ANY 'value string'.   If  */
/*    the attribute has a RANge of  numeric values,  two numeric operands  */
/*    may be specified,  corresponding to  "default1" and "default2";  if  */
/*    only one  is specified,  then  it resets  both defaults (it  is not  */
/*    necessary that previously-defined defaults  exist).   If the attri-  */
/*    bute has a  list of VALue valnames,   the RESET operand must  be an  */
/*    already-defined valname;   it is not  necessary that  a previously-  */
/*    defined default exist.    If the attribute was defined  in a manner  */
/*    such that it  does not have something  that can be RESET,   then an  */
/*    error message is produced.                                           */
/*                                                                         */
/* EXAMPLES                                                                */
/* (1) The following is a partial definition  of the FIG tag,  taken from  */
/*     the tagset definition for the  "standard layouts" described in the  */
/*     Waterloo SCRIPT/GML User's Guide document.                          */
/*       .gt FIG add @FIGTAB attributes texterror                          */
/*       .ga * DEPTH range 0 32000 0                                       */
/*       .ga * FONT off upper                                              */
/*       .ga * * value MONO default                                        */
/*       .ga * * value TEXT                                                */
/*       .ga * FRAME value BOX default                                     */
/*       .ga * * value NONE                                                */
/*       .ga * * value RULE                                                */
/*       .ga * ID length 6                                                 */
/*       .ga * NAME automatic 'Figure'                                     */
/*       .ga * PLACE upper value FLOAT default                             */
/*       .ga * * value ANY                                                 */
/*     FIG is defined  as a tag that  has attributes and does  not accept  */
/*     tagtext;   a  macro  named  @FIGTAB  is  to  be  invoked  for  APF  */
/*     processing.   If the first occurrence of  the FIG tag is specified  */
/*     by the  user with  attributes FONT=text  and PLACE=any,   then the  */
/*     following local-symbol  values will  be presented  to the  @FIGTAB  */
/*     macro:                                                              */
/*       *_TAG = 'FIG'                                                     */
/*       *_N = 1                                                           */
/*       *DEPTH = 0                                                        */
/*       *FRAME = 'BOX'                                                    */
/*       *NAME = 'Figure'                                                  */
/*       *PLACE = 'ANY'                                                    */
/* (2) If  the following  occurred  after the  GT/GA  definitions in  the  */
/*     example above,                                                      */
/*       .ga FIG FONT on                                                   */
/*       .ga * FRAME reset RULE                                            */
/*     before the  first user occurrence of  the same FIG tag,   then the  */
/*     following local-symbol  values will  be presented  to the  @FIGTAB  */
/*     macro:                                                              */
/*       *_TAG = 'FIG'                                                     */
/*       *_N = 1                                                           */
/*       *DEPTH = 0                                                        */
/*       *FRAME = 'RULE'                                                   */
/*       *FONT = 'TEXT'                                                    */
/*       *NAME = 'Figure'                                                  */
/*       *PLACE = 'ANY'                                                    */
/*                                                                         */
/*                                                                         */
/* ! for VALue the USE 'string' sub option is not implemented              */
/* ! RESET is not implemented                                              */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/***************************************************************************/


static  char        stringval[VAL_LENGTH + 1];
static  char    *   valptr;
static  long        ranges[4];



/***************************************************************************/
/*  process .ga xxx xxx optionsA optionsB                                  */
/*                      ^^^^^^^^                                           */
/*  Allowed options are shown in the strnicmp calls                        */
/*  Minimum Abbreviation is the UPpercase part of each option              */
/*                                                                         */
/*  returns cc = neg if unrecognized options (perhaps valid optionsB )     */
/*          cc = omit if no more parms                                     */
/***************************************************************************/

static  condcode    scan_att_optionsA( gaflags * att_flags )
{
    char        *   p;
    condcode        cc = pos;

    while( cc == pos ) {

        cc = getarg();
        if( cc == omit ) {              // nothing more
            break;
        }
        p = tok_start;
        switch( tolower( *p ) ) {
        case   'u' :
            if( (arg_flen > 1) && (arg_flen < 11)
                && !strnicmp( "UPpercase", p, arg_flen ) ) {

                *att_flags |= att_upper;
            } else {
                cc = neg;               // perhaps option B
            }
            break;
        case   'r' :
            if( (arg_flen > 2) && (arg_flen < 9)
                && !strnicmp( "REQuired", p, arg_flen ) ) {

                *att_flags |= att_req;
            } else {
                cc = neg;               // perhaps option B
            }
            break;
        case   'o' :
            if( !strnicmp( "OFF", p, arg_flen ) ) {

                *att_flags |= att_off;
            } else {
                if( !strnicmp( "ON", p, arg_flen ) ) {

                    *att_flags &= ~att_off;
                } else {
                    cc = neg;           // perhaps option B
                }
            }
            break;
        default:
            cc = neg;                   // invalid option
            break;                      // perhaps one of optionsB
        }
    }
    return( cc );
}

/***************************************************************************/
/*  process .ga xxx xxx optionsA  optionsB                                 */
/*                                ^^^^^^^^                                 */
/*  Allowed options are shown in the strnicmp calls                        */
/*  Minimum Abbreviation is the uppercase part of each option              */
/*                                                                         */
/*  returns cc = omit  if all options processed                            */
/*               other is error                                            */
/***************************************************************************/

static  condcode    scan_att_optionsB( gavalflags * val_flags, condcode cca,
                                       gaflags * att_flags )
{
    condcode        cc;
    getnum_block    gn;
    int             k;

    if( cca == omit ) {
        return( cca );                  // no more parms
    }

    stringval[0] = '\0';
    cc = pos;

    switch( tolower( *tok_start ) ) {
    case   'a' :
        if( !strnicmp( "ANY", tok_start, arg_flen ) ) {

            *val_flags |= val_any;
            *att_flags |= att_any;
        } else {
            if( (arg_flen > 3) && (arg_flen < 10)
                && !strnicmp( "AUTOmatic", tok_start, arg_flen ) ) {

                *val_flags |= val_auto;
                *att_flags |= att_auto;
            } else {
                xx_err( err_att_val_inv );
                cc = neg;
            }
        }

        if( cc != neg ) {               // any or auto found now scan string
            cc = getarg();
            if( cc == quotes || cc == pos || cc == quotes0) {
                if( arg_flen < sizeof( stringval ) ) {
                    strncpy_s( stringval, sizeof( stringval), tok_start,
                               arg_flen );
                    *val_flags |= val_value;
                } else {
                    valptr = mem_alloc( arg_flen + 1 );
                    strncpy_s( valptr, arg_flen + 1, tok_start, arg_flen );
                    *val_flags |= val_valptr;
                }
                if( *att_flags & att_any ) { // default for any specified
                    *val_flags |= val_def;
                    *att_flags |= att_def;
                }
            }
        }
        break;
    case   'r' :
        if( (arg_flen > 2) && (arg_flen < 6)
            && !strnicmp( "RANge", tok_start, arg_flen ) ) {

            *val_flags |= val_range;
            *att_flags |= att_range;

            gn.argstart = scan_start;
            gn.argstop  = scan_stop + 1;
            gn.ignore_blanks = false;
            ranges[2] = LONG_MIN;
            ranges[3] = LONG_MIN;
            for( k = 0; k < 4; k++ ) {  // scan max 4 numbers
                cc = getnum( &gn );
                if(  cc == omit ) {
                    break;
                }
                if( cc == notnum ) {
                    xx_err( err_att_val_inv );
                    cc = neg;
                    return( cc );
                }
                ranges[k] = gn.result;
            }
            scan_start = gn.argstart;
            if( (k < 2) || (ranges[0] > ranges[1]) ) {// need 2 or more values
                xx_err( err_att_range_inv );// ... second <= first
                cc = neg;
                return( cc );
            }
            if( k == 3 ) {
                ranges[3] = ranges[2];  // only 1 default specified
            }
            if( k > 2 ) {               // default specified
                if( (ranges[0] > ranges[2]) // default less min
                    || (ranges[1] < ranges[2])  // default gt max
                    || (ranges[0] > ranges[3])  // default2 less min
                    || (ranges[1] < ranges[3]) ) {  // default2 gt max
                    xx_err( err_att_default );
                    cc = neg;
                    return( cc );
                }
            }
            if( ranges[2] > LONG_MIN ) {
                *val_flags |= val_def;  // we have default
                *att_flags |= att_def;  // we have default
            }
        } else {
            if( (arg_flen == 5)
                && !strnicmp( "RESET", tok_start, arg_flen ) ) {

                *val_flags |= val_reset;
                /* no further processing */
            } else {
                cc = neg;
            }
        }
        break;
    case   'l' :
        if( (arg_flen > 2) && (arg_flen < 7)
            && !strnicmp( "LENgth", tok_start, arg_flen ) ) {

            *val_flags |= val_length;
            gn.argstart = scan_start;
            gn.argstop  = scan_stop + 1;
            gn.ignore_blanks = false;
            cc = getnum( &gn );
            if( cc == notnum || cc == omit ) {
                xx_err( err_att_val_inv );
                cc = neg;
                return( cc );
            } else {
                scan_start = gn.argstart;
                ranges [0] = gn.result;
            }
        } else {
            cc = neg;
        }
        break;
    case   'v' :
        if( (arg_flen > 2) && (arg_flen < 6)
            && !strnicmp( "VALue", tok_start, arg_flen ) ) {

            cc = getarg();
            if( (cc == pos) || (cc == quotes) || (cc == quotes0) ) {
                 if( arg_flen <= VAL_LENGTH ) {
                    *val_flags |= val_value;
                    strncpy_s( stringval, VAL_LENGTH + 1, tok_start,
                               arg_flen );
                    if( *att_flags & att_upper ) {
                        strupr( stringval );
                    }
                 } else {
#if 1
                    xx_err( err_att_val_inv );  // only short string allowed
                    cc = neg;           // this is a restriction from wgml 4.0
                    break;              // can be removed if neccessary
#else
                    *val_flags |= val_valptr;
                    valptr = mem_alloc( arg_flen + 1 );
                    strncpy_s( valptr, arg_flen + 1, tok_start, arg_flen );
                    if( *att_flags & att_upper ) {
                        strupr( valptr );
                    }
#endif
                 }
            } else {
                xx_err( err_att_val_inv );
                cc = neg;
                break;
            }
            cc = getarg();
            if( cc == omit ) {          // nothing more
                break;
            }
            if( cc == pos && (arg_flen > 2) && (arg_flen < 8)
                && !strnicmp( "DEFault", tok_start, arg_flen ) ) {

                *val_flags |= val_def;
                *att_flags |= att_def;
            } else {
                xx_err( err_att_val_inv );
                cc = neg;
                break;
            }
        } else {
            cc = neg;
        }
        break;
    default:
        cc = neg;                   // invalid option
        break;
    }
    if( cc == omit || cc == neg ) {
       ; /* empty pass thru cc */
    } else {
        cc = getarg();                  // set new cc omit is expected
    }
    return( cc );
}


/***************************************************************************/
/*  scr_ga    implement .ga control word                                   */
/***************************************************************************/

void    scr_ga( void )
{
    char        *   p;
    char        *   pn;
    char            savetag;
    char            saveatt;
    int             k;
    int             len;
    condcode        cc;
    gaflags         att_flags;
    gavalflags      val_flags;
    gavalentry  *   gaval;
    gaentry     *   gawk;


    savetag = ' ';
    saveatt = ' ';
    att_flags = 0;
    val_flags = 0;
    garginit();                         // find end of CW

    cc = getarg();                      // Tagname or *

    if( cc == omit || (*tok_start == '*' && tag_entry == NULL) ) {
        // no operands or tagname * and no previous definition
        tag_name_missing_err();
    }
    if( tag_entry == NULL ) {           // error during previous .gt

        scan_restart = scan_stop + 1;   // ignore .ga
        return;
    }


    /***********************************************************************/
    /*  isolate tagname  use previous if tagname *                         */
    /***********************************************************************/

    p = tok_start;

    if( *p == '*' ) {                   // single * as tagname
        if( arg_flen > 1 ) {
            xx_err( err_tag_name_inv );
            return;
        }
        savetag = '*';                  // remember for possible quick access
        if( GlobalFlags.firstpass && input_cbs->fmflags & II_research ) {
            out_msg("  using tagname %s\n", tagname );
        }
    } else {
        savetag = ' ';                  // no quick access

        init_tag_att();            // forget previous values for quick access

        pn      = tagname;
        len     = 0;

        while( *p && is_macro_char( *p ) ) {
            if( len < TAG_NAME_LENGTH ) {
                *pn++ = tolower( *p++ );// copy lowercase tagname
                *pn   = '\0';
            } else {
                break;
            }
            len++;
        }
        for( k = len; k < TAG_NAME_LENGTH; k++ ) {
            tagname[k] = '\0';
        }
        tagname[TAG_NAME_LENGTH] = '\0';

        if( len < arg_flen ) {
            xx_err( err_tag_name_inv );// name contains invalid or too many chars
            return;
        }
        tag_entry = find_tag( &tag_dict, tagname );
        if( tag_entry == NULL ) {
            nottag_err();               // tagname not defined
            return;
        }
    }

    /***********************************************************************/
    /* isolate attname  use previous if attname *                          */
    /***********************************************************************/

    cc = getarg();                      // Attribute  name or *

    if( cc == omit || (*tok_start == '*' && att_entry == NULL) ) {
        // no operands or attname * and no previous definition
        xx_err( err_att_name_inv );
        return;
    }

    p = tok_start;

    if( *p == '*' ) {                   // single * as attname
        if( arg_flen > 1 ) {
            xx_err( err_att_name_inv );
            return;
        }
        saveatt = '*';                  // remember for possible quick access
        if( GlobalFlags.firstpass && input_cbs->fmflags & II_research ) {
            out_msg("  using attname %s\n", attname );
        }
        att_flags = att_entry->attflags;
    } else {
        saveatt = ' ';                  // no quick access
        att_entry = NULL;
        pn      = attname;
        len     = 0;

        while( *p && is_macro_char( *p ) ) {
            if( len < ATT_NAME_LENGTH ) {
                *pn++ = tolower( *p++ );// copy lowercase tagname
                *pn   = '\0';
            } else {
                break;
            }
            len++;
        }
        for( k = len; k < ATT_NAME_LENGTH; k++ ) {
            attname[k] = '\0';
        }
        attname[ATT_NAME_LENGTH] = '\0';

        if( len < arg_flen ) {
            xx_err( err_att_name_inv );// attname with invalid or too many chars
            cc = neg;
            return;
        }
    }


    /***********************************************************************/
    /*   process options A and options B                                   */
    /***********************************************************************/

    if( cc != omit ) {
        if( saveatt != '*' ) {          // no quickaccess for attribute
            gawk = NULL;
            for( gawk = tag_entry->attribs; gawk != NULL;
                 gawk = gawk->next ) {

                if( !stricmp( attname, gawk->name ) ) {
                    att_flags = gawk->attflags; // get possible uppercase option
                    break;
                }
            }
        } else {
            att_flags = att_entry->attflags;
        }
        cc = scan_att_optionsA( &att_flags );   // process options A

        if( cc != omit ) {

            cc = scan_att_optionsB( &val_flags, cc, &att_flags );// process option B
            if( cc != omit ) {
                xx_err( err_tag_toomany );  // excess parameters
                return;
            }
        }
    }

    /***********************************************************************/
    /*  scanning complete     add/modify attribute in dictionary           */
    /***********************************************************************/
    if( saveatt != '*' ) {              // no quickaccess for attribute
        for( att_entry = tag_entry->attribs; att_entry != NULL;
             att_entry = att_entry->next ) {

            if( !stricmp( attname, att_entry->name ) ) {
                break;
            }
        }
    }
    if( att_entry == NULL ) {           // new attribute
        att_entry = mem_alloc( sizeof( gaentry ) );

        att_entry->next = tag_entry->attribs;
        tag_entry->attribs = att_entry;

        att_entry->vals = NULL;
        att_entry->attflags = att_flags;
        strcpy( att_entry->name, attname );
    } else {
        att_entry->attflags = att_flags;// update flags
    }

    gaval = mem_alloc( sizeof (gavalentry ) );

    if( att_entry->vals == NULL ) {
        att_entry->vals = gaval;
    } else {
        gavalentry  *   valwk;

        for( valwk = att_entry->vals;  valwk != NULL;
                                       valwk = valwk->next ) {
            if( valwk->next == NULL ) {
                break;                      // last entry found
            }
        }
        valwk->next = gaval;
    }
    gaval->next = NULL;

    gaval->valflags = val_flags;
    if( val_flags & val_length ) {
        gaval->a.length = ranges[0];
    } else if( val_flags & val_range ) {
        for( k = 0; k < 4; k++ ) {
            gaval->a.range[k] = ranges[k];
        }
    } else if( val_flags & val_value ) {
        strcpy_s( gaval->a.value, sizeof( gaval->a.value ), stringval );
    } else if( val_flags & val_valptr ) {
        gaval->a.valptr = valptr;
    }
    scan_restart = scan_stop + 1;
    return;
}
