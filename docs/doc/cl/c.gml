.* Watcom C Language Reference
.*
..im lytchg
..if &e'&dohelp eq 0 ..do begin
.  ..im wnohelp
..do end
..im fmtmacro
..im gmlmacs
:CMT. ..im htmlmacs
..im xdefs
..im cmac
..im boxmac
..im csymbol
.*
.* DEFINE MACROS AND FORMAT SETTINGS FOR THIS DOCUMENT
.*
..if &e'&dohelp eq 0 .do begin
.   ..sr wc286="&company. C&S'16."
.   ..sr wc386="&company. C&S'32."
.   ..sr wcboth="&company. C&S'16. and C&S'32."
.   ..sr c286="C&S'16."
.   ..sr c386="C&S'32."
.   ..sr wcall="&company. C&S'16., C&S'32. and Waterloo C"
..do end
..el .do begin
.   ..sr wc286="&company. C/16"
.   ..sr wc386="&company. C/32"
.   ..sr wcboth="&company. C/16 and C/32"
.   ..sr c286="C/16"
.   ..sr c386="C/32"
.   ..sr wcall="&company. C/16, C/32 and Waterloo C"
.   ..sr nobox=1
..do end
..sr wcgeneric="&company. C"
..sr cboth="C"
..sr wlooc="Waterloo C"
..sr target='PC'
.* ..sr target='PC 370'
.* ..sr target='370'
.* ..sr targetQNX=1
..sr targetQNX=0
:SET symbol='userguide' value="User's Guide".
:SET symbol='pragma'    value="User's Guide".
:SET symbol='libref'    value="&company C Library Reference manual".
:SET symbol='linkref'   value="&lnkname User's Guide".
:SET symbol='loadgo'    value="&company Express C".
:SET symbol='portable'  value="Writing Portable Programs".
:SET symbol='basedptr'  value="Based Pointers for &wcboth.".
:SET symbol='ptr86'     value="Special Pointer Types for &wc286.".
:SET symbol='ptr386'    value="Special Pointer Types for &wc386.".
:SET symbol='hugekw'    value="The &kwhuge_sp. Keyword".
:SET symbol='appkw'     value="Compiler Keywords".
:SET symbol='struct'    value="Structures".
:SET symbol='inttoflt'  value="Integer to Floating-Point Conversion".
:SET symbol='numlimit'  value="Macros for Numerical Limits".
:SET symbol='exdecl'    value="Examples of Declarations".
:SET symbol='charset'   value="Character Sets".
:SET symbol='initobj'   value="Initialization of Objects".
.*
..sr tabchar=$
.*
:GDOC.
.*
.   ..if &e'&dohelp eq 0 ..do begin
.   :FRONTM.
.           :INCLUDE file='cover'.
.       :ABSTRACT.
.           :INCLUDE file='copyrite'.
.           :SET symbol='isbn' value=''.
.           :INCLUDE file='disclaim'.
.       :PREFACE.
.           :INCLUDE file='preface'.
.           :INCLUDE file='thanks'.
.           :INCLUDE file='trademrk'.
.           ..if &targetQNX = 0 ..th ..do begin
.           .  :INCLUDE file='newslett'.
.           ..do end
.   .pa odd
.   :TOC.
.   .pa odd
.   ..do end
.   :BODY.
.   .   ..if &e'&dohelp eq 1 ..do begin
.   .   :exhelp
.   .   :INCLUDE file='&book..idx'.
.   .   :INCLUDE file='&book..tbl'.
.   .   :INCLUDE file='&book..kw'.
.   .   ..do end
.       .sepsect Introduction
.           .chap Introduction to C
.               :INCLUDE file='intro'.
.               .section History
.                   :INCLUDE file='history'.
.               .section Uses
.                   :INCLUDE file='uses'.
.               .section Advantages
.                   :INCLUDE file='advantag'.
.               .section How to Use This Book
.                   :INCLUDE file='usebook'.
.       .sepsect Language Reference
.           .chap Notation
.               :INCLUDE file='notation'.
.           .chap Basic Language Elements
.                   :INCLUDE file='basicle'.
.               .section &charset.
.                   :INCLUDE file='charset'.
.               .section Keywords
.                   :INCLUDE file='keywords'.
.               .section Identifiers
.                   :INCLUDE file='ids'.
.               .section Comments
.                   :INCLUDE file='comments'.
.           .chap Basic Types
.                   :INCLUDE file='basicty'.
.               .section Declarations of Objects
.                   :INCLUDE file='declare'.
.               .section Integer Types
.                   :INCLUDE file='integer'.
.               .section Floating-Point Types
.                   :INCLUDE file='float'.
.               .section Enumerated Types
.                   :INCLUDE file='enum'.
.               .section Arrays
.                   :INCLUDE file='array'.
.               .section Strings
.                   :INCLUDE file='string'.
.           .chap Constants
.               :INCLUDE file='constant'.
.           .chap Type Conversion
.               :INCLUDE file='convert'.
.           .chap Advanced Types
.                   :INCLUDE file='advanty'.
.               .section &struct.
.                   :INCLUDE file='struct'.
.               .section Unions
.                   :INCLUDE file='union'.
.               .section Pointers
.                   :INCLUDE file='pointer'.
.                   ..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.                       .beglevel
.                           :INCLUDE file='pcptrs'.
.                           .section &basedptr.
.                               :INCLUDE file='basedptr'.
.                       .endlevel
.                   ..do end
.               .section Void
.                   :INCLUDE file='void'.
.               .section The const and volatile Declarations
.               .beglevel
.                   :INCLUDE file='const'.
.                   :INCLUDE file='volatile'.
.               .endlevel
.           .chap Storage Classes
.                   :INCLUDE file='stgclass'.
.               .section Type Definitions
.                   :INCLUDE file='typedef'.
.               .section Static Storage Duration
.                   :INCLUDE file='statstg'.
.                   .beglevel
.                       .section The static Storage Class
.                           :INCLUDE file='static'.
.                       .section The extern Storage Class
.                           :INCLUDE file='extern'.
.                   .endlevel
.               .section Automatic Storage Duration
.                   :INCLUDE file='autostg'.
.                   .beglevel
.                       .section The auto Storage Class
.                           :INCLUDE file='auto'.
.                       .section The register Storage Class
.                           :INCLUDE file='register'.
.                   .endlevel
.           .chap &initobj.
.               :INCLUDE file='init'.
.           .chap Expressions
.               .se chapname='Expressions'.
.               :INCLUDE file='express'.
.           .chap Statements
.               :INCLUDE file='statemen'.
.           .chap Functions
.               :INCLUDE file='function'.
.           .chap The Preprocessor
.                   :INCLUDE file='preproc'.
.               .section Macro Replacement
.                   :INCLUDE file='macro'.
.           .chap The Order of Translation
.               :INCLUDE file='ordtrans'.
.       .sepsect Programmer's Guide
.           .chap Modularity
.               :INCLUDE file='modular'.
.           .chap &portable.
.               :INCLUDE file='portable'.
.           .chap Avoiding Common Pitfalls
.               :INCLUDE file='pitfalls'.
.           .chap Programming Style
.               :INCLUDE file='style'.
.   .sepsect Appendices
.   :APPENDIX.
.       .chap &appkw.
.           :INCLUDE file='appkword'.
.       .chap Trigraphs
.           :INCLUDE file='apptrigr'.
.       .chap Escape Sequences
.           :INCLUDE file='appescap'.
.       .chap Operator Precedence
.           :INCLUDE file='apporder'.
:CMT.   .chap Preprocessor Directives
:CMT.       :INCLUDE file='appprepr'.
.       .chap Formal C Grammar
.           :INCLUDE file='grammar'.
.       .chap Translation Limits
.           :INCLUDE file='translim'.
.       .chap &numlimit.
.           :INCLUDE file='numlimit'.
.       ..if '&target' eq 'PC' ..th ..do begin
.           .chap Implementation-Defined Behavior
.               :INCLUDE file='impldef1'.
.       ..do end
.       ..el ..if '&target' eq 'PC 370' ..th ..do begin
.           .chap &wcboth. Implementation-Defined Behavior
.               :INCLUDE file='impldef1'.
.           .chap &wlooc. Implementation-Defined Behavior
.               :INCLUDE file='impldef2'.
.       ..do end
.       .chap &exdecl.
.           :INCLUDE file='examples'.
.       .chap A Sample Program
.           :INCLUDE file='program'.
.       .chap Glossary
.           :INCLUDE file='glossary'.
.   ..if &e'&dohelp eq 0 ..do begin
.   :BACKM.
.       ..cd set 2
.       :INDEX.
.   ..do end
.   ..cd set 1
.   .cntents end_of_book
:eGDOC.
.*
.* ======================================================================
.* Other things I haven't figured out what to do with yet:
.*
.* - sequence points
.* - white-space
.* - differing grammar in EXPRESSIONS than in SYNTAX
.* - describe abstract declarators, references in casts, sizeof,
.*   function prototypes
.* - pitfall:  #define x = 10
.* - pitfall: defining an object that holds a character as "char"; the
.*            library stuff (like getchar) returns "int" in order to
.*            allow returning of -1 for EOF.
.* - pitfall: while( expr ); { body of loop }
.*            doesn't do anything in the loop until after the while exits
.* - in examples, describe how to use an object with a complex declar'n
