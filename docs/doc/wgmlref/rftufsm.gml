:H3.Selecting Symbol Information
:P.
Symbols may be used in the GML source document to ensure a text fragment
is specified the same way in different places.
For example,
:XMP.
&gml.SET symbol='product' value='&WGML.'.
:eXMP.
:PC.
will result in the text &mono.&WGML.&emono. being placed in the
document wherever &mono.&amp.product.&emono. is specified (the period
is necessary when specifying a symbol name in the document).
The symbol screen is used to define symbol values before processing
the document.
These values may then be changed without editing the GML source.
:P.
From the options screen, select the :HP1.Symbol Definitions:eHP1. hotspot.
The symbol screen is organized in a similar manner as the device screen
discussed previously.
Select the :HP1.Insert Symbol:eHP1. hotspot.
Enter the text &mono.product&emono. and press the &key.Tab&ekey. key.
Enter the text &mono.&WGML.&emono.\.
The screen should appear as follows:
.screen 'frsymed' 'Define a symbol value'
:P.
The symbol name :HP1.product:eHP1. will now have the value
&mono.&WGML.&emono. if used in the source document.
:P.
&WGML. has a form letter capability.
When a values file containing a list of names and addresses is specified,
&WGML. will produce one document for each record in the file
(although this capability is usually used for producing form letters, it
may be used with regular documents as well).
The individual values in each record are assigned to pre-defined
symbol names (&amp.value1., through to &amp.valuen.).
(See :HDREF refid='mailmrg'. for more information.)
:P.
Press the &key.Tab&ekey. twice and
enter the text &mono.customer&emono.\.
The screen should appear as follows:
.screen 'frsymva' 'Define a mail merge file'
