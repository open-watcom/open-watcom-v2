:SET symbol="owstlname" value="&company. STL"
:SET symbol="startyear" value="2005"

.* These all map to mono font at the mo, could be different if wished
.* should they have start/end tag pairs to move closer to other current
.* sytems? Currently 'CODE' is intended to be for generic code snippets.
.* The more specialized macros should be used where appropriate.

.gt CLASS add mono
.gt CODE add mono
.gt FUNC add mono
.gt PATH add mono
.gt TOOL add mono

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
