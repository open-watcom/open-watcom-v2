This form of the &fn. creates an empty &obj.:PERIOD.
Dynamic allocation is used.
The inherited stream member functions can be used to access the &obj.:PERIOD.
.if '&cl_name' = 'strstream' .do begin
Note that the &getptr. and &putptr. are not necessarily pointing at the same
location, so moving one pointer (e.g. by doing a write) does not affect the
location of the other pointer.
.do end
:RSLTS.
This form of the &fn. creates an initialized, empty &obj.:PERIOD.
