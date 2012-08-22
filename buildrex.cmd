/* REXX  Version of build.cmd                                              */
/*                                                                         */
/*  buildrex.cmd - build Open Watcom using selected compiler               */
/*                                                                         */
/*  will build (or clean) the builder, watcom and installer                */
/*                                                                         */
/*  combined OS/2 and Windows version                                      */
/*                                                                         */
/*  CALL:                                                                  */
/*    OS/2:         buildrex     [self] [clean] [doc_build]                */
/*      tested with classic REXX and OREXX                                 */
/*    Winxx: regina buildrex.cmd [self] [clean] [doc_build]                */
/*      XP: tested with Regina REXX                                        */
/*                                                                         */
/*  If self is not specified, the tools from the existing OW installation  */
/*  are used, this requires the WATCOM environment variable already set.   */
/*                                                                         */
/*  If self is specified, the tools from the new rel2 tree are used.       */
/*                                                                         */
/*  clean deletes the generated files in the build tree                    */
/*                                                                         */
/*  doc_build builds the documenatation, too. (may require changes in      */
/*   setvars.cmd/bat)                                                      */
/*                                                                         */
/*  owroot, watcom and doc_build are set prior to doing the build.         */
/*  The other evironment variables are taken from setvars.cmd/bat.         */
/*                                                                         */
/***************************************************************************/

tver="2006-09-23 15.10"
Signal On NOVALUE                           /* catch uninitilized variables*/
Signal On SYNTAX                                     /* catch syntax errors*/
Signal On HALT                                           /* catch ctl-break*/

Call Initos                                                /* os/2 or winxx*/

Trace 'O'

Parse Source os invoke sourcefn .                       /* what is our name*/
fn=Filespec("Name",sourcefn)
Parse Var fn name "." .

savecurdir = Directory()                               /* current directory*/
srcdir = Filespec("Drive", sourcefn)Filespec("Path", sourcefn)
srcdir = Left(srcdir, Length(srcdir)-1)            /* get rid of trailing \*/



/***************************************************************************/
/*  Check parms        self clean doc_build                                */
/***************************************************************************/

Parse Upper Arg p.1 p.2 p.3 p.4 .
makeclean=""
self = ""
doc_build = 0
target = ""
Do k = 1 To 4
  If p.k = "CLEAN" Then target = "clean"
  If p.k = "SELF" Then self = "SELF"
  If p.k = "DOC_BUILD" Then doc_build = 1
End

If target = "" Then target = "rel2"
If target = "clean" Then makeclean = target

/***************************************************************************/
/*    get / set owroot + watcom environment variables                      */
/***************************************************************************/
watcom = Value( "WATCOM", , "ENVIRONMENT" )
owroot = Value( "OWROOT", , "ENVIRONMENT" )

If owroot = "" Then Do
  owroot = Value( "OWROOT", srcdir, "ENVIRONMENT" )
  owroot = Value( "OWROOT", , "ENVIRONMENT" )
End

If self = "SELF" Then Do                            /* build with rel2 exes*/
  If watcom >< owroot"\rel2" Then Do
    watcom = Value( "WATCOM", owroot"\rel2", "ENVIRONMENT" )
    watcom = Value( "WATCOM", , "ENVIRONMENT" )
  End
End
Else Do                                          /* build with release exes*/
  If watcom = "" Then Do
    Say "WATCOM environment variable not set, cannot continue."
    Return 99
  End
End

Call Procsetvar  /* process setvar commandlist */


/***************************************************************************/
/*  The real work                                                          */
/***************************************************************************/

/* builder is first */
Call Directory owroot"\bld\builder\"builderdir
"wmake" makeclean

/* now we can start the builder with 9 generations of logfiles */
Call Directory owroot"\bld"
"builder" target "-b 9"

/* build the installer */
Call Directory owroot"\bld\setupgui"
"builder" target "-b 9"

Return


/***************************************************************************/
/*  process setvars.cmd / bat                                              */
/*     does hopefully the same processing as cmd.exe                       */
/***************************************************************************/
PROCSETVAR:
devvar= owroot"\setvars."cmdext

If Stream( devvar, "C", "Query Exists" ) = "" Then Do
  Say devvar "file does not exist, cannot continue."
  Return 99
End

Do While Lines( devvar ) > 0
  orgline = Strip( Linein( devvar ) )
  If orgline = "" Then Iterate
  upline = Translate( orgline )
  If Left( upline, 1 ) = "@" Then Do
    upline = Substr( upline, 2 )
    orgline = Substr( orgline, 2 )
  End
  Parse Var upline uword1 uword2 "=" uword3
  Select
    When uword1 = "REM" Then Iterate
    When uword1 = "SET" Then Do
      If uword2 = "OWROOT" Then Iterate
      If uword2 = "WATCOM" Then Iterate
      Parse Var orgline word1 word2 "=" word3
      If uword2 = "OWDOCBUILD" Then word3 = doc_build
      Else word3 = repvars( word3 )
      Call Value uword2, word3, "ENVIRONMENT"
      Call Value uword2, word3
    End
    When uword1 = "CD" Then Do
      Parse Var orgline word1 word2 .
      word2 = repvars( word2 )
      Call Directory word2
    End
    When uword1 = "ECHO" Then Say orgline
    When uword1 = "IF" Then Do
      If upline = 'IF "%OWDEFPATH%" == "" SET OWDEFPATH=%PATH%' Then Do
        path = Value( "PATH", , "ENVIRONMENT" )
        Call Value "OWDEFPATH", path, "ENVIRONMENT"
        Call Value "OWDEFPATH", path
      End
    End
    Otherwise
      Say "??? not processed:" orgline
  End
End
Return

/***************************************************************************/
/*  replace environment variables in string                                */
/***************************************************************************/
REPVARS:
Parse Arg raw
www = raw
auswert = ""
lastchar = Right( raw, 1 )
If lastchar <> "%" Then lastchar=""

Do While www <>""
  Parse Var www vor"%"svar"%"www
  auswert = auswert||vor
  If www <> "" Then Do
    evar = Value( svar, , "ENVIRONMENT" )
    If evar = "" Then evar = svar
    auswert = auswert||evar
  End
  Else Do
    If svar <> "" Then Do
      evar = Value( svar, , "ENVIRONMENT" )
      If evar = "" Then evar = svar
      auswert = auswert||evar
      lastchar = ""
    End
    auswert = auswert||lastchar
  End
End
Return auswert



/***************************************************************************/
/*  determine NT / OS/2  + REXX version                                    */
/***************************************************************************/
INITOS:
Parse Source os invoke sourcefn srest
Parse version langproc langlvl rday rmon ryear

/* try without rexxutils */
/* rc=rxfuncadd( "sysloadfuncs","rexxutil","sysloadfuncs")
   Call Sysloadfuncs
---------------------------------*/
rexcall = "Call";
If os = "OS/2" Then Do
  os2 = 1
  winxx = 0
  builderdir = "os2386"
  cmdext = "cmd"
End
Else Do
  os2 = 0
  winxx = 1
  rexcall     = "Regina"
  builderdir = "nt386"
  cmdext = "bat"
End
Return



/* Error / signal handling */
SYNTAX: Trace'o'
Signal Off NOVALUE
zigl = sigl
Parse Source opsys . execfn .
If Symbol('RC') <> 'LIT'
  Then
    error_rc = rc
  Else
    error_rc = 0
Say '*'
Say '>>>' execfn 'Syntax error in line' zigl':'
Say 'Rexx returncode =' error_rc  Errortext(error_rc)
Say '*'
Call Common

HALT: Trace'o'
Signal Off NOVALUE
zigl = sigl
Parse Source opsys . execfn .
Say '>>>' execfn 'CTL-break in line' zigl'.'
Say '*'
Call Common

NOVALUE: Trace'o'
Signal Off NOVALUE
zigl = sigl
Parse Source opsys . execfn .
Say '>>>' execfn 'NoValue error in line' zigl':'
Parse version impl lvl .
If lvl > 3.45 Then
  Say '>>> Variable is:' Condition('D')

COMMON:
Say '>>>' Sourceline(zigl)
Nop
Exit errk
