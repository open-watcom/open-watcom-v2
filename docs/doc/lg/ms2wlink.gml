.*
.*
.*
.section Converting Microsoft Response Files to Directive Files
.*
.np
.ix 'response files' 'conversion'
.ix '&m2wcmdup command'
A utility called &m2wcmdup can be used to convert Microsoft linker
response files to &lnkname directive files.
The response files must correspond to the linker found in version 7
or earlier of Microsoft C.
Later versions of response files such as those used with Microsoft
Visual C++ are not entirely supported.
.np
The same utility can also convert much of the content of IBM OS/2
LINK386 response files since the syntax is similar.
.np
Input to &m2wcmdup is processed in the same way as the Microsoft
linker processes its input.
The difference is that &m2wcmdup writes the corresponding &lnkname
directive file to the standard output device instead of a creating an
executable file.
The resulting output can be redirected to a disk file which can then
be used as input to the &lnkname to produce an executable file.
.np
Suppose you have a Microsoft linker response file called "test.rsp".
You can convert this file to a &lnkname directive file by issuing the
following command.
.exam begin
&sysprompt.&m2wcmd @test.rsp >test.lnk
.exam end
.pc
You can now use the &lnkname to link your program by issuing the
following command.
.exam begin
&sysprompt.&lnkcmd @test
.exam end
.np
An alternative way to link your application with the &lnkname from a
Microsoft response file is to issue the following command.
.exam begin
&sysprompt.&m2wcmd @test.rsp | &lnkcmd
.exam end
.pc
Since the &lnkname gets its input from the standard input device, you
do not have to create a &lnkname directive file to link your
application.
.np
Note that &m2wcmdup can also process module-definition files used for
creating OS/2 applications.
.*
