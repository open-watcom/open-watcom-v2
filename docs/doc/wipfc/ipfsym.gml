.*
.chap *refid='ipfsym' IPF Symbols
.*
.np
Special symbols can be incorporated into OS/2 help file by using entity references in the form of
&symbol-name. Each entity reference begins with the '&' character and ends with the '.' character. 
New symbols can be created by using the nameit command (see :HDREF refid=nameit.), but each 
language encoding has a pre-defined set of symbols associated with it. For the en_US locale (and 
other locales using the IBM 850 code page), the predefined symbols are as follows (symbols that 
cannot appear in this document are left blank):
.*
.millust begin
        Hex     Unicode     Entity
Char    value   code point  name        Description
-----------------------------------------------------------------------------------------
 &x'10      0x10    \u25BA      rarrow      Black right-pointing pointer
 &x'10      0x10    \u25BA      rahead      Black right-pointing pointer
 &x'11      0x11    \u25C4      lahead      Black left-pointing pointer
 &x'11      0x11    \u25C4      larrow      Black left-pointing pointer
 &x'18      0x18    \u25B2      uarrow      Black up-pointing triangle
 &x'19      0x19    \u25BC      darrow      Black down-pointing triangle
        0x20    \u0020      rbl         Required blank
 !      0x21    \u0021      xclm        Exclamation mark
 !      0x21    \u0021      xclam       Exclamation mark
 "      0x22    \u201D      odq         Left double quotation mark
 "      0x22    \u201C      cdq         Right double quotation mark
 #      0x23    \u0023      numsign     Number sign
 $      0x24    \u0024      dollar      Dollar sign
 %      0x25    \u0025      percent     Percent sign
 &      0x26    \u0026      amp         Ampersand
 '      0x27    \u0027      apos        Apostrophe
 `      0x60    \u2018      osq         Left single quotation mark
 '      0x27    \u2019      csq         right single quotation mark
 `      0x60    \u0060      grave       Grave accent
 (      0x28    \u0028      lpar        Left parenthesis
 (      0x28    \u0028      lparen      Left parenthesis
 )      0x29    \u0029      rpar        Right parenthesis
 )      0x29    \u0029      rparen      right parenthesis
 *      0x2A    \u002A      asterisk    Asterisk
 +      0x2B    \u002B      plus        Plus sign
 ,      0x2C    \u002C      comma       Comma
 -      0x2D    \u002D      dash        Hyphen-minus
 -      0x2D    \u002D      emdash      Hyphen-minus
 -      0x2D    \u002D      mdash       Hyphen-minus
 -      0x2D    \u002D      endash      Hyphen-minus
 -      0x2D    \u002D      ndash       Hyphen-minus
 -      0x2D    \u002D      hyphen      Hyphen-minus
 -      0x2D    \u002D      minus       Hyphen-minus
 .      0x2E    \u002E      period      Full stop
 .      0x2E    \u002E      per         Full stop
 /      0x2F    \u002F      slash       Solidus
 /      0x2F    \u002F      slr         Solidus
 :      0x3A    \u003A      colon       Colon
 ;      0x3B    \u003B      semi        Semicolon
 <      0x3C    \u003C      lt          Less-than sign
 <      0x3C    \u003C      ltsym       Less-than sign
 =      0x3D    \u003D      eq          Equals sign
 =      0x3D    \u003D      equals      Equals sign
 =      0x3D    \u003D      eqsym       Equals sign
 >      0x3E    \u003E      gt          Greater-than sign
 >      0x3E    \u003E      gtsym       Greater-than sign
 @      0x40    \u0040      atsign      Commercial at
 [      0x5B    \u005B      lbracket    Left square bracket
 [      0x5B    \u005B      lbrk        Left square bracket
 \      0x5C    \u005C      bslash      Reverse solidus
 \      0x5C    \u005C      bsl         Reverse solidus
 ]      0x5D    \u005D      rbracket    Right square bracket
 ]      0x5D    \u005D      rbrk        Right square bracket
 ^      0x5E    \u005E      caret       Circumflex accent
 ^      0x5E    \u005E      and         Circumflex accent
 _      0x5F    \u005F      us          Low line
 {      0x7B    \u007B      lbrace      Left curly bracket
 {      0x7B    \u007B      lbrc        Left curly bracket
 |      0x7C    \u00A6      splitvbar   Split vertical bar
 |      0x7C    \u007C      vbar        Vertical bar
 |      0x7C    \u007C      lor         Vertical bar
 }      0x7D    \u007D      rbrace      Right curly bracket
 }      0x7D    \u007D      rbrc        Right curly bracket
 ~~      0x7E    \u007E      tilde       Tilde
 ~~      0x7E    \u007E      similar     Tilde
        0x7F    \u2302      house       House
 &x'80      0x80    \u00C7      Cc          Latin capital letter C with cedilla
 &x'81      0x81    \u00FC      ue          Latin small letter U with diaeresis
 &x'82      0x82    \u00E9      ea          Latin small letter E with acute
 &x'83      0x83    \u00E2      ac          Latin small letter A with circumflex
 &x'84      0x84    \u00E4      ae          Latin small letter A with diaeresis
 &x'85      0x85    \u00E0      ag          Latin small letter A with grave
 &x'86      0x86    \u00E5      ao          Latin small letter A with ring above
 &x'87      0x87    \u00E7      cc          Latin small letter C with cedilla
 &x'88      0x88    \u00EA      ec          Latin small letter E with circumflex
 &x'89      0x89    \u00EB      ee          Latin small letter E with diaeresis
 &x'8A      0x8A    \u00E8      eg          Latin small letter E with grave
 &x'8B      0x8B    \u00EF      ie          Latin small letter I with diaeresis
 &x'8C      0x8C    \u00EE      ic          Latin small letter I with circumflex
 &x'8D      0x8D    \u00EC      ig          Latin small letter I with grave
 &x'8E      0x8E    \u00C4      Ae          Latin capital letter A with diaeresis
 &x'8F      0x8F    \u00C5      Ao          Latin capital letter A with ring above
 &x'8F      0x8F    \u00C5      angstrom    Latin capital letter A with ring above
 &x'90      0x90    \u00C9      Ea          Latin capital letter E with acute
 &x'91      0x91    \u00E6      aelig       Latin small letter Ae
 &x'92      0x92    \u00C6      AElig       Latin capital letter Ae
 &x'93      0x93    \u00F4      oc          Latin small letter O with circumflex
 &x'94      0x94    \u00F6      oe          Latin small letter O with diaeresis
 &x'95      0x95    \u00F2      og          Latin small letter O with grave
 &x'96      0x96    \u00FB      uc          Latin small letter U with circumflex
 &x'97      0x97    \u00F9      ug          Latin small letter U with grave
 &x'98      0x98    \u00FF      ye          Latin small letter Y with diaeresis
 &x'99      0x99    \u00D6      Oe          Latin capital letter O with diaeresis
 &x'9A      0x9A    \u00DC      Ue          Latin capital letter U with diaeresis
 &x'9B      0x9B    \u00A2      cent        Cent sign
 &x'9C      0x9C    \u00A3      Lsterling   Pound sign
        0x9D    \u00A5      yen         Yen sign
 &x'9F      0x9F    \u0192      fnof        Latin small letter F with hook
 &x'A0      0xA0    \u00E1      aa          Latin small letter A with acute
 &x'A1      0xA1    \u00ED      ia          Latin small letter I with acute
 &x'A2      0xA2    \u00F3      oa          Latin small letter O with acute
 &x'A3      0xA3    \u00FA      ua          Latin small letter U with acute
 &x'A4      0xA4    \u00F1      nt          Latin small letter N with tilde
 &x'A5      0xA5    \u00D1      Nt          Latin capital letter N with tilde
 &x'A6      0xA6    \u00AA      aus         Feminine ordinal indicator
 &x'A7      0xA7    \u00BA      ous         Masculine ordinal indicator
 &x'A8      0xA8    \u00BF      invq        Inverted question mark
 &x'A9      0xA9    \u2310      lnotrev     Reversed not sign
 &x'AA      0xAA    \u00AC      lnot        Not sign
 &x'AA      0xAA    \u00AC      notsym      Not sign
 &x'AB      0xAB    \u00BD      frac12      Vulgar fraction one half
 &x'AC      0xAC    \u00BC      frac14      Vulgar fraction one quarter
 &x'AD      0xAD    \u00A1      inve        Inverted exclamation mark
 &x'AE      0xAE    \u00AB      odqf        Left-pointing double angle quotation mark
 &x'AF      0xAF    \u00BB      cdqf        Right-pointing double angle quotation mark
        0xB0    \u2591      box14       Light shade
 &x'B1      0xB1    \u2592      box12       Medium shade
 &x'B2      0xB2    \u2593      box34       Dark shade
 &x'B3      0xB3    \u2502      bxv         Box drawings light vertical
 &x'B4      0xB4    \u2524      bxri        Box drawings light vertical and left
 &x'B4      0xB4    \u2524      bxrj        Box drawings light vertical and left
 &x'B5      0xB5    \u2561      bx1012      Box drawings vertical single and left double
 &x'B6      0xB6    \u2562      bx2021      Box drawings vertical double and left single
 &x'B7      0xB7    \u2556      bx0021      Box drawings down double and left single
 &x'B8      0xB8    \u2555      bx0012      Box drawings down single and left double
        0xB9    \u2563      bx2022      Box drawings double vertical and left
 &x'BA      0xBA    \u2551      bx2020      Box drawings double vertical
 &x'BB      0xBB    \u2557      bx0022      Box drawings double down and left
 &x'BC      0xBC    \u255D      bx2002      Box drawings double up and left
 &x'BD      0xBD    \u255C      bx2001      Box drawings up double and left single
 &x'BE      0xBE    \u255B      bx1002      Box drawings up single and left double
 &x'BF      0xBF    \u2510      bxur        Box drawings light down and left
 &x'C0      0xC0    \u2514      bxll        Box drawings light up and right
 &x'C1      0xC1    \u2534      bxas        Box drawings light up and horizontal
 &x'C1      0xC1    \u2534      bxbj        Box drawings light up and horizontal
 &x'C2      0xC2    \u252C      bxde        Box drawings light down and horizontal
 &x'C2      0xC2    \u252C      bxtj        Box drawings light down and horizontal
 &x'C3      0xC3    \u251C      bxle        Box drawings light vertical and right
 &x'C3      0xC3    \u251C      bxlj        Box drawings light vertical and right
 &x'C4      0xC4    \u2500      bxh         Box drawings light horizontal
 &x'C5      0xC5    \u253C      bxcr        Box drawings light vertical and horizontal
 &x'C5      0xC5    \u253C      bxcj        Box drawings light vertical and horizontal
 &x'C6      0xC6    \u255E      bx1210      Box drawings vertical single and right double
 &x'C7      0xC7    \u255F      bx2120      Box drawings vertical double and right single
 &x'C8      0xC8    \u255A      bx2200      Box drawings double up and right
 &x'C9      0xC9    \u2554      bx0220      Box drawings double down and right
 &x'CA      0xCA    \u2569      bx2202      Box drawings double up and horizontal
 &x'CB      0xCB    \u2566      bx0222      Box drawings double down and horizontal
        0xCC    \u2560      bx2220      Box drawings double vertical and right
 &x'CD      0xCD    \u2550      bx0202      Box drawings double horizontal
 &x'CE      0xCE    \u256C      bx2222      Box drawings double vertical and horizontal
 &x'CF      0xCF    \u2567      bx1202      Box drawings up single and horizontal double
 &x'D0      0xD0    \u2568      bx1201      Box drawings up double and horizontal single
 &x'D1      0xD1    \u2564      bx0212      Box drawings down single and horizontal double
 &x'D2      0xD2    \u2565      bx0121      Box drawings down double and horizontal single
        0xD3    \u2559      bx2100      Box drawings up double and right single
 &x'D4      0xD4    \u2558      bx1200      Box drawings up single and right double
 &x'D5      0xD5    \u2552      bx0210      Box drawings down single and right double
 &x'D6      0xD6    \u2553      bx0120      Box drawings down double and right single
 &x'D7      0xD7    \u256B      bx2121      Box drawings vertical double and horizontal single
 &x'D8      0xD8    \u256A      bx1212      Box drawings vertical single and horizontal double
 &x'D9      0xD9    \u2518      bxlr        Box drawings light up and left
 &x'DA      0xDA    \u250C      bxul        Box drawings light down and right
 &x'DB      0xDB    \u2588      BOX         Box drawings Full block
 &x'DC      0xDC    \u2584      BOXBOT      Box drawings Lower half block
 &x'DD      0xDD    \u258C      BOXLEFT     Box drawings Left half block
 &x'DE      0xDE    \u2590      BOXRIGHT    Box drawings Right half block
 &x'DF      0xDF    \u2580      BOXTOP      Box drawings Upper half block
 &x'E0      0xE0    \u03B1      alpha       Greek small letter alpha
 &x'41      0x41    \u0391      Alpha       Greek capital letter alpha
 &x'E1      0xE1    \u00DF      Beta        Latin small letter sharp s
 &x'E2      0xE2    \u0393      Gamma       Greek capital letter gamma
 &x'E3      0xE3    \u03C0      pi          Greek small letter pi
 &x'E4      0xE4    \u03A3      Sigma       Greek capital letter sigma
 &x'E5      0xE5    \u03C3      sigma       Greek small letter sigma
 &x'E6      0xE6    \u00B4      mu          Micro sign
 &x'E7      0xE7    \u03C4      tau         Greek small letter tau
        0xE8    \u03A6      Phi         Greek capital letter phi
 &x'E9      0xE9    \u0398      Theta       Greek capital letter theta
        0xEA    \u03A9      Omega       Greek capital letter omega
 &x'EB      0xEB    \u03B4      delta       Greek small letter delta
        0xEC    \u221E      infinity    Infinity
 &x'ED      0xED    \u03C6      phi         Greek small letter phi
 &x'EE      0xEE    \u03B5      epsilon     Greek small letter epsilon
        0xEF    \u2229      intersect   Intersection
        0xF0    \u2261      identical   Identical to
 &x'F1      0xF1    \u00B1      plusmin     Plus-minus sign
 &x'F1      0xF1    \u00B1      pm          Plus-minus sign
 &x'F2      0xF2    \u2265      ge          Greater-than or equal to
 &x'F2      0xF2    \u2265      gesym       Greater-than or equal to
 &x'F3      0xF3    \u2264      le          Less-than or equal to
 &x'F3      0xF3    \u2264      lesym       Less-than or equal to
 &x'F4      0xF4    \u2320      inttop      Top half integral
 &x'F5      0xF5    \u2321      intbot      Bottom half integral
 &x'F6      0xF6    \u00F7      divide      Division sign
 &x'F7      0xF7    \u2248      nearly      Almost equal to
 &x'F8      0xF8    \u00B0      degree      Degree sign
 &x'F8      0xF8    \u00B0      deg         Degree sign
 &x'F9      0xF9    \u2219      bullet      Bullet operator
 &x'FA      0xFA    \u00B7      dot         Middle dot
 &x'FB      0xFB    \u221A      sqrt        Square root
 &x'FC      0xFC    \u207F      supn        Superscript Latin small letter n
 &x'FD      0xFD    \u00B2      sup2        Superscript 2
 &x'FE      0xFE    \u25A0      sqbul       Black square
.millust end
