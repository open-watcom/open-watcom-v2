.section *refid=vidnlm Debugging a Novel NLM
.*
.np
.ix 'Novell NLM' 'debugging'
.ix 'NLM' 'debugging Novell'
.ix 'debugging' 'Novell NLM'
Novell NLM's may only be debugged remotely. You must use either the
serial, parallel, or Novell SPX link.
There are 6 NLM's distributed in the &company package.
The following table describes their use:
.millust begin
                NetWare 3.11/3.12       NetWare 4.01

Serial                                  serserv4.nlm
Parallel        parserv3.nlm            parserv4.nlm
SPX             novserv3.nlm            novserv4.nlm
.millust end
.np
To start remote debugging, you load one of the above NLM at the NetWare
file server console. The debugger is then invoked as in any remote debugging
session.
See the chapter entitled :HDREF refid='vidrem'. for parameter details.
See the appendix entitled :HDREF refid='vidwire'. for parallel/serial
cable details.
.np
For example, on a NetWare 4.01 server type:
.monoon
load novserv4
.monooff
.np
On a workstation, type
.monoon
&dbgcmdup /tr=nov mynlm
.monooff
