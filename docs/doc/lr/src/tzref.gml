.np
.if '&machsys' eq 'PP' .do begin
The time set on the computer with the
.mono Time
and the
.mono Date
entries of the
.mono Preferences
menu reflects the local time.
The environment variable
.mono TZ
is used to establish the time zone to which this local time applies.
.do end
.el .if '&machsys' eq 'QNX' .do begin
The time set on the computer with the
.qnxcmd date
reflects Coordinated Universal Time (UTC).
The environment variable
.mono TZ
is used to establish the local time zone.
.do end
.el .do begin
The time set on the computer with the
.doscmd time
and the
.doscmd date
reflects the local time.
The environment variable
.mono TZ
is used to establish the time zone to which this local time applies.
.do end
.ix 'time zone'
.ix 'TZ'
See the section
.us The TZ Environment Variable
for a discussion of how to set the time zone.
