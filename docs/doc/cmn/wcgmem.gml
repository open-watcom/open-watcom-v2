.ix 'predictable code size'
.ix 'code generation' 'memory requirements'
The
.ev WCGMEMORY
environment variable may be used to request a report of the amount of
memory used by the compiler's code generator for its work area.
.exam begin
&prompt.&setcmd. &setdelim.WCGMEMORY=?&setdelim.
.exam end
.pc
When the memory amount is "?" then the code generator will report how
much memory was used to generate the code.
.np
It may also be used to instruct the compiler's code generator to
allocate a fixed amount of memory for a work area.
.exam begin
&prompt.&setcmd. &setdelim.WCGMEMORY=128&setdelim.
.exam end
.pc
When the memory amount is "nnn" then exactly "nnnK" bytes will be used.
In the above example, 128K bytes is requested.
If less than "nnnK" is available then the compiler will quit with a
fatal error message.
If more than "nnnK" is available then only "nnnK" will be used.
.np
There are two reasons why this second feature may be quite useful.
In general, the more memory available to the code generator, the more
optimal code it will generate.
Thus, for two personal computers with different amounts of memory, the
code generator may produce different (although correct) object code.
.ix 'software quality assurance'
If you have a software quality assurance requirement that the same
results (i.e., code) be produced on two different machines then you
should use this feature.
To generate identical code on two personal computers with different
memory configurations, you must ensure that the
.ev WCGMEMORY
environment variable is set identically on both machines.
.if '&target' ne 'QNX' .do begin
.np
The second reason where this feature is useful is on virtual memory
paging systems (e.g., OS/2) where an unlimited amount of memory can be
used by the code generator.
.ix 'compile time'
If a very large module is being compiled, it may take a very long time
to compile it.
The code generator will continue to allocate more and more memory and
cause an excessive amount of paging.
By restricting the amount of memory that the code generator can use,
you can reduce the amount of time required to compile a routine.
.do end
