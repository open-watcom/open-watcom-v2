:CMT don't know how to control the numbering of headings with script so use gml
:CMT layout. Waterloo script docs suggest script didn't know anything about
:CMT numbering in the first place. IBM does.
:CMT It would seem the script .h0 may be calling the same function as
:CMT the internal :H0. This version of script/gml seems highly coupled, all
:CMT rather confusing really.
:LAYOUT.
:DEFAULT
    input_esc='~'
:H0
    number_form=new
    font=12
    number_font=12
:H1
    number_form=prop
    number_font=11
    font=11
:eLAYOUT.

.* These all map to mono font at the mo, could be different if wished
.* should they have start/end tag pairs to move closer to other current
.* sytems? Currently 'CODE' is intended to be for generic code snippets.
.* The more specialized macros should be used where appropriate.

.gt CLASS add @mono
.gt CODE add @mono
.gt FUNC add @mono
.gt PATH add @mono
.gt TOOL add @mono

.dm @mono begin
.* how do you change current font with script?
.* seems this varies a bit between waterloo and ibm, it's rather complicated
.* and I can't figure out how to make it work on this
:SF font=4.&*:eSF.
.dm @mono end

.*----------------------- Headings ----------------------------------
.* like an old .sepsect
.gt H0 add @H0
.dm @H0 begin
.h0 &*
.sr headtext$=" "
.sr headtxt0$=&*
.dm @H0 end
.* .gt H0 print

.* like an old .chap
.gt H1 add @H1
.dm @H1 begin
.h1 &*
.sr headtext$=&*
.sr headtxt1$=&*
.dm @H1 end
.* .gt H1 print

.* like an old .sect
.gt H2 add @H2
.dm @H2 begin
.h2 &*
.sr headtext$=&*
.dm @H2 end
.* .gt H2 print

.* ------------------------------------------------------------------
.gt INCLUDE add @include attribute
.ga * file any
.dm @include begin
.im &*file.
.dm @include end
.* ------------------------------------------------------------------
.gt SET add @set attribute
.ga * symbol req any
.ga * value any
.dm @set begin
.sr &*symbol.="&*value."
.* .ty &*symbol.="&*value."
.dm @set end
.* ------------------------------------------------------------------
.* use the put comand before anything happens so file is created ready for 
.* first pass, otherwise error thrown
.pu 9 .*
.gt AUTHOR add @author att
.ga * date any
.dm @author begin
.* use workfile 9 for author info
.pu 9 &headtxt0$. :~ &*
.pu 9 .br
.pu 9 Documentation last updated:~ &*date.
.pu 9 .sk 1
.* also emit local text
Author: &*
.dm @author end

.* this could be hidden away in the FMATTER tag when that is rewritten...
.gt EMITAUTHOR add @emitauthor
.dm @emitauthor begin
.* workfile 9 will contain author info
Author and update information.
.sk 1
.im 9
.dm @emitauthor end

