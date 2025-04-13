
README
======

This folder contains the LaTeX source of the Open Watcom documentation set.
This is an experimental project to migrate the entire documentation set from
WGML format to LaTeX. These files are not being built as part of the normal
Open Watcom build process.

Quick Start
-----------

To build the documents here, you need to have a LaTeX distribution installed.
We are using [TeX Live](https://www.tug.org/texlive/), but any modern LaTeX
distribution that includes LuaLaTeX should work (in theory). One advantage of
TeX Live is that it supports all three of Windows, Linux, and macOS (via
[MacTeX](https://www.tug.org/mactex/)) in a uniform way.

Currently, the documents use Libertinus fonts. You may need to install
these fonts separately. Under TeX Live, you can do this with:

    tlmgr install libertinus-fonts

This makes the fonts available only to the TeX Live system. Alternatively, you
can download the fonts from the [Libertinus releases
page](https://github.com/alerque/libertinus/releases) and install them
according to the procedures for your operating system. This approach makes the
fonts available to your entire system, including other applications such as
Microsoft Word, LibreOffice, etc.

Other packages that might need to be installed are:

    tlmgr install marginnote

The documentation set consists of several independent books. Each book is in
its own folder. To build a book, change to the folder and run:

    lualatex book.tex

Where *book.tex* is the main LaTeX source file for the book, named after the
folder. You should run the command three times to ensure the table-of-contents
and all cross-references are correct. The result will be a PDF file named
*book.pdf* (again, named after the folder).

Development Environments
------------------------

### Visual Studio Code

We recommend [Visual Studio Code](https://code.visualstudio.com/) for LaTeX
development. Install the following extensions:

* LaTeX Workshop (by James Yu)
* LaTeX Utilities (by tecosaur
* Rewrap (by stkb)

There are also several Git extensions available. They are not strictly
required for editing the documentation, but they may make interacting with the
Git repository easier. For example:

* GitLens (by GitKraken)

Some other utility extensions may also be useful:

* Todo Tree (by Gruntfuggly)
* Code Spell Checker (by Street Side Software)

The Code Spell Checker extension duplicates, somewhat, the spell checking
feature in the LTeX+ tool that is installed with the LaTeX Workshop. Note that
LTeX+ is a grammar checker for LaTeX (and other markup languages).

A suitable settings file for Visual Studio Code is provided in the `.vscode`
folder in this repository. Settings for certain tools are shared, such as the
wrap width for the Rewrap extension (use Alt+Q to rewrap a paragraph). Simply
open the `latex` folder in Visual Studio Code, and it will automatically pick
up the settings file.


### Other Environments

Since LaTeX is a text-based format, any text editor can be used to edit LaTeX
documents. However, certain documents in the Open Watcom documentation set may
contain non-ASCII (Unicode) characters that older editors might have
difficulty processing.

Notes
-----

Currently, only PDF documents are built. It is our hope to eventually also
build HTML documents. Support for other output formats would be harder
(although PostScript and DVI would be straight forward). In particular,
support for the DOS and OS/2 document readers might never be added.

Building the documentation set on DOS or OS/2 would require a modern LaTeX
distribution that runs on those platforms. The
[emTeX](https://ecsoft2.org/emtex) distribution might be usable, but it is
obsolete and, to our knowledge, no longer supported. It most likely lacks
LuaLaTeX and OpenType support, which means it would require special handling.
