.section *refid='acviewport' acviewport
.ix 'acviewport'
.ix 'Block' 'acviewport'
.ix 'Application controlled window'
.tag begin
.tdesc
This tag creates a child window within the current window (page). The contents of this child window is
controlled by an application via a dynamic link library.
.tattrbs
.tattr dll='text'
The name of the dynamic link library containing the code that controls the current viewer window.
.tattr objectname='text'
The entry point (function name) in the dll to be called. Case sensitive.
.tattr objectinfo='text'
Parameters to be passed to the dll function.
.tattr objectid='text'
An identifier to associate the window with the object.
.tattr vpx=measurement
The x origin of the child window. May be absolute, relative, or dynamic. See :HDREF refid='units'..
.tattr vpy=measurement
The y origin of the child window. May be absolute, relative, or dynamic. See :HDREF refid='units'..
.tattr vpcx=measurement
The width of the child window. May be absolute, relative, or dynamic. See :HDREF refid='units'..
.tattr vpcy=measurement
The height of the child window.  May be absolute, relative, or dynamic. See :HDREF refid='units'..
.tclass Block
.tcont None
.tseealso
:HDREF refid='ddf'.
.tag end

