/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2009-2018 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Country configuration data
*
****************************************************************************/


#ifndef CNTRYDEF_INCLUDED
#define CNTRYDEF_INCLUDED

#include <cstdio>
#include <string>
#include "nlsrecty.hpp"


class OutFile;

class CountryDef {
public:
    CountryDef() : _size( sizeof( word ) + 2 * sizeof( byte ) + 4 * sizeof( word ) ),
        _type( WIPFC::CONTROL ), _format( 0 ), _value( 256 ), _country( 1 ),
        _codePage( 850 ), _reserved( 0 ) {};

    void nlsConfig( const char *loc );
    dword write( OutFile* out ) const;
    word codePage() const { return( _codePage ); }
    word size() const { return( _size ); }
    const std::string nlsFileName() const { return _nlsFileName; }
    const std::string entityFileName() const { return _entityFileName; }
    const std::string icuConverter() const { return _icuConverter; }

private:
    word                _size;      // 12
    WIPFC::NLSRecType   _type;      // 0 (NLSRecType::CONTROL)
    byte                _format;    // 0
    word                _value;     // 256
    word                _country;   // from config file / 1
    word                _codePage;  // from config file / 850
    word                _reserved;  // 0
    std::string         _nlsFileName;
    std::string         _entityFileName;
    std::string         _icuConverter;
};

#endif

/*
The following table lists the 3-digit country code for the /COUNTRY or -d: nnn
parameter, the numeric identifiers of code pages, and the APS filename of the
IPFC command supported.

+-------------------------------------------------------------------------+
|Country               |Country Code|Code Pages        |APS File          |
|----------------------+------------+------------------+------------------|
|Arabic                |785         |0864              |APSY0864.APS      |
|----------------------+------------+------------------+------------------|
|Australia             |061         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Belgium               |032         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Brazil                |055         |0850, 0437        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Bulgaria              |359         |0915              |APSY0915.APS      |
|----------------------+------------+------------------+------------------|
|Canadian English      |001         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Canadian French       |002         |0863, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Catalan               |034         |0850, 0437        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Chinese (Simplified)  |086         |1381              |APSY1381.APS      |
|----------------------+------------+------------------+------------------|
|Chinese (Simplified)  |086         |1386              |APSY1386.APS      |
|----------------------+------------+------------------+------------------|
|Chinese (Traditional) |088         |0950              |APSY0950.APS      |
|----------------------+------------+------------------+------------------|
|Czech                 |421         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Denmark               |045         |0865, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Finland               |358         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|France                |033         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Germany               |049         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Greece                |030         |0869              |APSY0869.APS      |
|----------------------+------------+------------------+------------------|
|Greece                |030         |0813              |APSY0813.APS      |
|----------------------+------------+------------------+------------------|
|Hebrew                |972         |0862              |APSY0862.APS      |
|----------------------+------------+------------------+------------------|
|Hungary               |036         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Italy                 |039         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Japan                 |081         |0932, 0437, 0850  |APSY0932.APS      |
|----------------------+------------+------------------+------------------|
|Korea                 |082         |0949, 0934        |APSY0949.APS      |
|----------------------+------------+------------------+------------------|
|Latin America         |003         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Lithuania             |370         |0921              |APSY0921.APS      |
|----------------------+------------+------------------+------------------|
|Netherlands           |031         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Norway                |047         |0865, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Poland                |048         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Portugal              |351         |0860, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Russia                |007         |0866              |APSY0866.APS      |
|----------------------+------------+------------------+------------------|
|Slovenia              |386         |0852              |APSY0852.APS      |
|----------------------+------------+------------------+------------------|
|Spain                 |034         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Sweden                |046         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Switzerland           |041         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|Thailand              |066         |0874              |APSY0874.APS      |
|----------------------+------------+------------------+------------------|
|Turkey                |090         |0857              |APSY0857.APS      |
|----------------------+------------+------------------+------------------|
|United Kingdom        |044         |0437, 0850        |APSYMBOL.APS      |
|----------------------+------------+------------------+------------------|
|United States         |001         |0437, 0850        |APSYMBOL.APS      |
+-------------------------------------------------------------------------+
Note:  If there is an APSYxxxx.APS file that matches the code page you are
    using to compile your IPF file (either specified or default), the IPFC
    will use that file. Otherwise, it will use APSYMBOL.APS file that is
    suitable for code page 437 or 850.

The following table lists the 3-letter identifier for the /LANGUAGE and -l: xxx
parameter of the IPFC command:

+------------------------------------------------------------------------+
|ID        |Language                      |NLS File                      |
|----------+------------------------------+------------------------------|
|ARA       |Arabic                        |IPFARA.NLS                    |
|----------+------------------------------+------------------------------|
|BGR       |Bulgarian                     |IPFBGR.NLS                    |
|----------+------------------------------+------------------------------|
|CAT       |Catalan                       |IPFCAT.NLS                    |
|----------+------------------------------+------------------------------|
|CHT       |Chinese (Traditional)         |IPFCHT.NLS                    |
|----------+------------------------------+------------------------------|
|CZE       |Czech                         |IPFCZE.NLS                    |
|----------+------------------------------+------------------------------|
|DAN       |Danish                        |IPFDAN.NLS                    |
|----------+------------------------------+------------------------------|
|DEU       |German                        |IPFDEU.NLS                    |
|----------+------------------------------+------------------------------|
|ELL       |Greek 813                     |IPFELL.NLS                    |
|----------+------------------------------+------------------------------|
|ENG       |English UP                    |IPFENG.NLS                    |
|----------+------------------------------+------------------------------|
|ENU       |English US                    |IPFENU.NLS                    |
|----------+------------------------------+------------------------------|
|ESP       |Spanish                       |IPFESP.NLS                    |
|----------+------------------------------+------------------------------|
|FIN       |Finnish                       |IPFFIN.NLS                    |
|----------+------------------------------+------------------------------|
|FRA       |French                        |IPFFRA.NLS                    |
|----------+------------------------------+------------------------------|
|FRC       |Canadian French               |IPFFRC.NLS                    |
|----------+------------------------------+------------------------------|
|GRK       |Greek 869                     |IPFGRK.NLS                    |
|----------+------------------------------+------------------------------|
|HEB       |Hebrew                        |IPFHEB.NLS                    |
|----------+------------------------------+------------------------------|
|HUN       |Hungarian                     |IPFHUN.NLS                    |
|----------+------------------------------+------------------------------|
|ITA       |Italian                       |IPFITA.NLS                    |
|----------+------------------------------+------------------------------|
|JPN       |Japanese                      |IPFJPN.NLS                    |
|----------+------------------------------+------------------------------|
|KOR       |Korean                        |IPFKOR.NLS                    |
|----------+------------------------------+------------------------------|
|LTU       |Lithuanian                    |IPFLTU.NLS                    |
|----------+------------------------------+------------------------------|
|NLD       |Dutch                         |IPFNLD.NLS                    |
|----------+------------------------------+------------------------------|
|NOR       |Norwegian                     |IPFNOR.NLS                    |
|----------+------------------------------+------------------------------|
|POL       |Polish                        |IPFPOL.NLS                    |
|----------+------------------------------+------------------------------|
|PRC       |Chinese (Simplified) 1381     |IPFPRC.NLS                    |
|----------+------------------------------+------------------------------|
|PRC       |Chinese (Simplified) 1386     |IPFGBK.NLS                    |
|----------+------------------------------+------------------------------|
|PTB       |Brazilian/Portuguese          |IPFPTB.NLS                    |
|----------+------------------------------+------------------------------|
|PTG       |Portuguese                    |IPFPTG.NLS                    |
|----------+------------------------------+------------------------------|
|RUS       |Russian                       |IPFRUS.NLS                    |
|----------+------------------------------+------------------------------|
|SLO       |Slovene                       |IPFSLO.NLS                    |
|----------+------------------------------+------------------------------|
|SVE       |Swedish                       |IPFSVE.NLS                    |
|----------+------------------------------+------------------------------|
|THI       |Thai                          |IPFTHI.NLS                    |
|----------+------------------------------+------------------------------|
|TRK       |Turkish                       |IPFTRK.NLS                    |
|----------+------------------------------+------------------------------|
|UND       |User defined                  |IPFUND.NLS                    |
+------------------------------------------------------------------------+

*/
