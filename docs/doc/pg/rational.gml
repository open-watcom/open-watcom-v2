:set symbol='dos4g'     value=';.sf1 DOS/4GW;.esf '.
:set symbol='dos4gct'   value=';.ct .sf1 DOS/4GW;.esf '.
:set symbol='dos4gprd'  value='DOS/4GW'.
:set symbol='dos4gexe'  value='DOS4GW.EXE'.
:set symbol='dos4gnam'  value='DOS4GW'.
.*
.chap The Tenberry Software &dos4gprd DOS Extender
.*
.np
.ix '&dos4gprd DOS extender'
The chapters in this section describe the 32-bit Tenberry Software
&dos4g DOS Extender which is provided with the &cmpname package.
&dos4g is a subset of Tenberry Software's DOS/4G product.
&dos4g is customized for use with the &cmpname package.
Key differences are:
.begbull
.bull
&dos4g will only execute programs built with a &company 32-bit compiler
such as &cmpname and linked with its run-time libraries.
.bull
.ix '&dos4gprd' 'VMM restriction'
The &dos4g virtual memory manager (VMM), included in the package, is
restricted to 32MB of memory.
.bull
.ix '&dos4gprd' 'TSR not supported'
&dos4g does not provide extra functionality such as TSR capability and
VMM performance tuning enhancements.
.endbull
.np
If your application has requirements beyond those provided by &dos4g,
you may wish to acquire &dos4gprd Professional or DOS/4G from:
.millust begin
Tenberry Software, Inc.
PO Box 20050
Fountain Hills, Arizona
U.S.A  85269-0050

WWW:    http://www.tenberry.com/dos4g/
Email:  info@tenberry.com
Phone:  1.480.767.8868
Fax:    1.480.767.8709
.millust end
.np
Programs developed to use the restricted version of &dos4g which is
included in the &cmpname package can be distributed on a royalty-free
basis, subject to the licensing terms of the product.
:INCLUDE file='LINEXE'
:INCLUDE file='CONFIG'
:INCLUDE file='VMM'
:INCLUDE file='NT2CLS'
:INCLUDE file='INT31'
:INCLUDE file='UTILTS'
:INCLUDE file='ERRORS'
:INCLUDE file='DOS4GWQA'
