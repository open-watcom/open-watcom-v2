The &fn. creates a
.if '&str_sub_pm' = 'constant' .do begin
const
.do end
reference to the character at offset
:ARG.pos
within the &obj.:PERIOD.
This reference may
.if '&str_sub_pm' = 'constant' .do begin
not
.do end
be used to modify that character.
The first character of a &obj. is at position zero.
:P.
If
:ARG.pos
is greater than or equal to the length of the &obj.,
and the resulting reference is used, the behavior is undefined.
.if '&str_sub_pm' = 'dynamic' .do begin
:P.
If the reference is used to modify other characters within the
&obj., the behavior is undefined.
.do end
:P.
The reference is associated with the &obj., and therefore has meaning only
as long as the &obj. is not modified (or destroyed).
If the &obj. has been modified and an old reference is used, the
behavior is undefined.
:RSLTS.
The &fn. returns a
.if '&str_sub_pm' = 'constant' .do begin
const
.do end
reference to a character.
