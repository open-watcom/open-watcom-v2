:H2.Indexing
:P.
GML will create an index for you using information
:I1.indexing
gathered during the processing of the document.
The index information is supplied by index tags.
This section will illustrate the use of the indexing tags
to create a simple index.
:P.
With a large document, working drafts can be produced faster
without the index.
To process the index information, the INDEX option
(see :HDREF refid='runindx'.)
must be specified on the &WGML. command line.
:P.
When an index tag is specified, the text line following the tag and
the current page number are saved.
If the :hp2.&gml.index:ehp2.
:I2 refid='ttindex'.ttindex
tag is specified in the back material,
and the INDEX option is specified,
the saved information is processed and output as an index.
The following example illustrates the use of the index
tags&gml.
:CMT.NOTE THAT RFGINDX IS RUN AND HAND CRAFTED TO PRODUCE THE OUTPUT.
:fig id=rfgindx place=inline frame=box.
 :INCLUDE file='rfgindx'.
 :figcap.Illustration of the Indexing Tags
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box

   +---+
   | P |
   +---+

   primary index  1

      another subentry  1
         subentry of an I2 entry  1
      primary index subentry  1

 :figcap.
 :figdesc.Output of :figref refid=rfgindx page=no.
:efig
:PC.
The :hp2.&gml.i1:ehp2.
:I2 refid='tti1'.tti1
tag causes the creation of a primary index entry.
The :hp2.&gml.i2:ehp2.
:I2 refid='tti2'.tti2
tags cause the creation of
index subentries for the last primary index entry.
The :hp2.&gml.i3:ehp2.
:I2 refid='tti3'.tti3
tag causes the creation of
index subentries for the last level two index entry.
Note that &WGML. automatically sorted the index subentries.
All index entries are sorted when the index is created.
:P.
If an index entry is specified more than once, the entries are merged.
The following example illustrates index entry merging&gml.
:CMT.NOTE THAT RFGINDX2 IS RUN AND HAND CRAFTED TO PRODUCE THE OUTPUT.
:fig id=rfgidx2 place=inline frame=box.
 :INCLUDE file='rfgindx2'.
 :figcap.A More Complex Index
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box

   +---+
   | P |
   +---+

   primary index  1-2

      primary index subentry  1


   +---+
   | S |
   +---+

   second primary  2

      subentry  2
         subentry of an I2  2

 :figcap.
 :figdesc.Output of :figref refid=rfgidx2 page=no.
:efig
:PC.
The two index subentries created by the :HP2.&gml.I2:eHP2. tags with
the text "subentry" were merged together.
Note that since they were both for the same output page, the
page number was only displayed once.
The level one index entries with the text "primary index" were
also merged together.
However, since they were for different output pages, the page
number for each entry was displayed.
:P.
A different method to create an index entry is with the
index heading tag.
The index heading tags generate results similar to the index
tags described above.
The main difference is that the page number is not saved.
The following example illustrates the index heading tags&gml.
:CMT.NOTE THAT RFGINDX3 IS RUN AND HAND CRAFTED TO PRODUCE THE OUTPUT.
:fig id=rfgidx3 place=inline frame=box.
 :INCLUDE file='rfgindx3'.
 :figcap.Illustration of Index Headings
:efig.
:INCLUDE file='rffhead'.
:fig place=inline frame=box

   +---+
   | P |
   +---+

   primary index

      another subentry  1
      primary index subentry  1


   +---+
   | S |
   +---+

   second primary  1

      subentry  1

 :figcap.
 :figdesc.Output of :figref refid=rfgidx3 page=no.
:efig
:PC.
Note that the level one index with the text "primary index"
does not have any page numbers displayed.
It can also be seen that the subentry for the second :hp2.&gml.ih1:ehp2.
:I2 refid='ttih1'.ttih1
tag is merged with the subentry for the first :hp2.&gml.ih1:ehp2. tag.
