.section The OS/2 TZ Environment Variable
.*
.np
.ix 'time zone'
.np
On the OS/2 platform, an alternate format of the
.kw TZ
environment variable is supported, in addition to the standard format
described in the preceding section.
The value of the OS/2
.kw TZ
environment variable should be set as follows (spaces are for clarity
only):
.begnote
.note std offset dst, rule
.endnote
This format will be used if after scanning the standard format there are
additional fields or the format has not been identified as standard.
.np
The standard format is identified if an offset follows dst;
characters J, M, /, or : are found in rule; or some fields are empty.
.np
The alternate expanded format is as follows (fields may not be empty):
.begnote
.note stdoffsetdst,sm,sw,sd,st,em,ew,ed,et,shift
.endnote
.begnote
.note std, dst
three or more letters that are the designation for the standard
.us (std)
and summer
.us (dst)
time zone.
Upper- and lowercase letters are allowed.
Any characters except for a leading colon (:), digits, comma (,),
minus (&minus), plus (+), and ASCII NUL (\0) are allowed.
.note offset
.ix 'Coordinated Universal Time'
.ix 'UTC'
indicates the value one must add to the local time to arrive at
Coordinated Universal Time (UTC).
The
.us offset
has the form:
.begnote
.note hh[:mm[:ss]]
.endnote
.pc
The minutes
.us (mm)
and seconds
.us (ss)
are optional.
The hour
.us (hh)
is required and may be a single digit.
The value is always interpreted as a decimal number.
The hour may be between 0 and 24, and the minutes (and seconds)
- if present - between 0 and 59.
If preceded by a "&minus", the time zone will be east of the
.ix 'Prime Meridian'
.us Prime Meridian
.'ct ; otherwise it will be west (which may be indicated by an optional
preceding "+").
.note rule
indicates when to change to and back from summer time and the time shift
for summer time.
The
.us rule
has the form:
.begnote
.note sm,sw,sd,st,em,ew,ed,et,shift
.endnote
.pc
where
.us sm,sw,sd,st
describe when the change from standard to summer time occurs and
.us em,ew,ed,et
describe when the change back happens.
.np
.us sm
and
.us em
specify the starting and ending month (1 - 12) of the summer time.
.np
.us sw
and
.us ew
specify the starting and ending week of the summer time.
You can specify the last week of the month ( -1 ), or week 1 to 4.
Week 0 has a special meaning for the day field (
.us sd
or
.us ed
).
.np
.us sd/ed
Starting/ending day of dst,
.np
0 - 6 ( weekday Sun to Sat ) if
.us sw/ew
is not zero,
.np
1 - 31 ( day of the month ) if
.us sw/ew
is zero
.np
.us st/et
Starting/ending time (in seconds after midnight) of the summer time.
.np
.us shift
Amount of time change (in seconds).
.np
.endnote
.np
An example of the default setting is:
.begnote $break
.note TZ=EST5EDT,4,1,0,7200,10,-1,0,7200,3600
This is the full specification for the default when the
.kw TZ
variable is not set.
Eastern Standard Time is 5 hours earlier than Coordinated Universal
Time (UTC).
Standard time and daylight saving time both apply to this locale.
Eastern Daylight Time (EDT) is one hour ahead of standard time.
Daylight saving time starts on the first (1) Sunday (0) of April (4)
at 2:00 A.M. and ends on the last (-1) Sunday (0) of October (10) at
2:00 A.M.
.endnote
.*
