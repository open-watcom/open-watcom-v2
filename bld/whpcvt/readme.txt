
            WATCOM Help Converter Program
            =============================


What WHLPCVT Is:
================
    WHLPCVT is a program which can be used to convert GML documentation
    source into OS-dependent online help. This program is just one
    step in the process, as described below:

        - Modify your GML documentation source to have whatever
          degree of online help specific information you want (see
          the notes below about using GML). With minimal
          modifications to your GML source, you can generate
          a basic online book. With more advanced changes, you can
          generate more useful online help with specific help
          features (pictures, specialized hyperlinks, popup definition
          boxes, index lists, etc.).

        - Using the WHLP layouts and macros, format your GML
          book. The result will be an output file representing
          online help in a system independent manner (extension
          'WHP' for WATCOM help).

        - This system independent format can then be converted
          into OS dependent help files. Windows RTF, OS/2 IPF,
          and InfoBench IB help are all supported.


The WHP File Format:
====================
    The WHP file format is intended to be OS independent, and
    capable of generating Windows RTF and OS/2 IPF help file
    formats. DOS InfoBench-style help is also supported.

    The WHP is designed to be very easy to parse. It is not intended
    to be easy to read! Since GML must generate this stuff, an
    important goal was to make sure GML could generate it easily.

    This section is intended to be used by people having trouble
    with WGML and WHP layouts. Normally, nobody would look
    at the WHP files.

    Codes:
    ------
    The WHP file contains key code characters. These are ASCII codes
    greater than 127 which are unlikely to appear as normal GML text.
    The format descriptions are BNF: '[]' means option stuff, '<>'
    means a necessary component described later, '|' is one of,
    anything else means actual text.


    '¥' (180) Include On.
        - Column 0, no following text. Default at start.
        - Text from the next line and on is to be included in processing.

    '√' (195) Include Off.
        - Column 0, no following text.
        - Text from the next line and on is to be excluded completely
          until the next '>' character is encountered. Include On/Off
          are not 'stackable'; the next include on always turns
          on processing.

    '˜' (247) Topic Definition.
        - Column 0.
        - format: ˜<level>˜[¯|ß]<ctx_name>˜<title>[˜<browse_list>[˜<keywords>]]
        - <level> is the heading level of this topic. This is
          used for OS/2 IPF help, as well as in RTF and InfoBench "up"
          buttons. Heading levels can be skipped.
          Valid heading levels are from 0 to 6.
        - '¯' (248) Popup definition topic. Indicates that the
          <title> should not appear in a non-scrolling region at the
          top of the topic. The title will scroll with the topic text.
        - 'ß' (167) Normal definition topic (default). Indicates that
          the <title> will appear in a non-scrolling region at
          the top of the topic. The title will not scroll
          away as the topic is scrolled.
        - <ctx_name> is the help topic context name for the topic. Blanks
          (and other non-variable-like characters) are not allowed
          in this name.
        - <title> is the Title displayed at the top of the Topic. It
          may be any text.
        - <keywords> is a list of search keywords (actually, key
          word groupings), separated by commas and semi-colons. The <title>
          is usually put in here as well.
        - <browse_list> is the name of the browse list that this
          topic belongs to.
        - Topic text may follow immediately, or a topic section.

    '' (127) Hyperlink.
        - in text lines, not split across lines.
        - format: <ctx_name><text>
        - <ctx_name> is the help topic context name to which this hyperlink
          should go.
        - <text> is any arbitrary text which represents the hyperlink.

    'Í' (234) Definition popup.
        - in text lines, not split across lines.
        - format: Í<ctx_name>Í<text>Í
        - <ctx_name> is the help topic context name to which this popup
          definition should go.
        - <text> is any arbitrary text which represents the definition link.

    'Ó' (238) Topic Section.
        - column 0.
        - format: Ó<ctx_name>[Ó<order_nun>]
        - defines a new topic section for <ctx_name>.
        - if no <order_num> is provided, then this section is placed
          as the next unordered topic.
        - if <order_num> is provided, then this section is ordered
          by its order number (lowest first). Ordered sections come
          after unordered sections. <order_num> can be 0 or greater.

    '“' (210) Unordered List Start.
         - column 0. No following text.
         - format: “[c]
         - starts an unordered list (with bullets).
         - 'c' is optional, and indicates a compact list (no spaces
           between list items). Otherwise, spacing is as indicated
           in the definition.

    '»' (200) Order List Start.
        - column 0. No following text.
         - format: »[c]
        - starts an ordered and numbered list.
         - 'c' is optional, and indicates a compact list (no spaces
           between list items). Otherwise, spacing is as indicated
           in the definition.

    '’' (213) Simple List Start.
        - column 0. No following text.
         - format: ’[c]
        - starts a simple list (no bullets).
         - 'c' is optional, and indicates a compact list (no spaces
           between list items). Otherwise, spacing is as indicated
           in the definition.

    ' ' (202) Definition List Start.
        - column 0. No following text.
        - starts a definition list.

    'Ã' (204) Definition List Term.
        - column 0.
        - format: Ã<term>
        - The text lines after the term are displayed below the term
          as paragraphs.
        - There is no closer for defn terms since font styling and
          other effects can be inserted.

    'π' (185) Definition Description.
        - column 0. Followed by text for the definition description.
        - Starts a definition description.

    '”' (211) List item.
        - column 0. Followed by text for the list item.
        - Item for a unordered, ordered or simple list.
        - ALL list types can be nested arbitrarily.

    '‘' (212) Unordered List End.
        - column 0. No following text.
        - ends an unordered list.

    '…' (201) Order List End.
        - column 0. No following text.
        - ends an ordered list.

    'À' (203) Definition List End.
        - column 0. No Following text.
        - ends a definition list.

    '÷' (214) Simple list End.
        - column 0. No Following text.
        - ends a simple list.

    'Ï' (236) Keyword.
        - in text lines, or on their own lines.
        - format: Ï<keyword>Ï
        - <keyword> is added to the list of keywords for the current topic.
        - If a line is defined containing only keywords, it does not
          generate a blank line.

    'û' (158) Paragraph Reset.
        - in text lines.
        - causes the next paragraph to be reset properly. Useful
          for RTF, when boxing and other wierd stuff require a
          paragraph reset to gets things back to normal.

    'ü' (159) Include On and Eat Blank Line.
        - Column 0.
        - same as Include On, but also eats the next blank line, if any.

    '∞' (176) Bitmap Picture.
        - in text lines.
        - format: ∞i|l|r|c∞<file>∞
        - 'i' is for inline, 'l' for left, 'r' for right, and 'c' for centered.
        - <file> is the name of the bitmap file.

    'Ô' (239) Font Style.
        - in text lines.
        - format: Ô<font_styles>Ô {with ending '' later}
        - <font_styles> is any combination of the following characters:
          b (bold), i (italic), u (underline), s (underscore).
        - The ending '' restores the font to plain with no styles. Hence,
          font stylings do NOT nest (this is due the the stupidity of
          OS/2 IPF).

    '' (240) Font Style End.
        - in text lines.
        - see 'Font Style'.

    'ù' (157) Font Type.
        - in text lines.
        - format: ù<facename>ù<point_size>ù
        - valid facenames are recognized by the following subwords:
          Courier, Helv, Roman, Tmns, Symbol.
        - common point sizes are: 8, 10, 12, 14, 18, 24.
        - the Windows default is Helv 10.

    '€' (219) Box On.
        - column 0. No Following text.
        - turns box mode on

    '˛' (254) Box Off.
        - column 0. No Following text.
        - turns box mode off

    '™' (170) Tab example.
        - format 1: ™<char> [<stops>]*
            - setups up a tabbed example.
        - format 2: ™
            - ends a tabbed example.

    'Ë' (232) File hyperlink.
        - in text lines, not split across lines.
        - format: Ë<file_name>Ë<topic_title>Ë<text>Ë
        - <file_name> is the name of help file to jump to.
        - <topic_title> is the help topic name to which this hyperlink
          should go.
        - <text> is any arbitrary text which represents the hyperlink.


Windows Online Help Structure:
==============================
    This section describes the various features of Windows online help.
    Items with '*' mean that the current conversion process does not
    support this feature.

    - Topics:
        - help files are organized into topics. Topics contain information
          to help the user.
        - topics have titles.

    - Keywords:
        - A help file will have keywords. Keywords are phrases which
          map to topics. A search button contains a sorted list of
          all keywords.

    - Browse Lists:
        - Topics can be organized into browse lists. A topic can belong
          to at most one browse list.
        - When in any topic, forward and backward buttons appear
          on the help window which can be used to move through the
          browse list this topic is in.

    - Hyperlinks:
        - hyperlinks appear within topic text. When clicked on, a jump
          occurs to the associated topic.

    - Popup Definitions:
        - Popup definitions appear within topic text. When clicked on,
          a window appears containing the associated topic.

    - Graphics:
        - bitmaps can be included in Windows help.
        * metafile files can also be included.

    * Graphic Hotspots:
        * graphic hotspots (which are bitmaps) can be defined. These work
          like hyperlinks.
        * Segmented graphic hotspots can be created. This allows a complex
          picture to go to many different topics (depending on where the
          user clicked).

    - Up Topics:
        - a heirarchy of topics can be created. An 'Up' button is provided
          on the Help window which canbe used to move up the heirarchy.

    - Prev/Next:
        - move forwards/backwards through history of help topics.

    - Boxing:
        - Paragraphs can be outlined with boxes.

    * Borders:
        * borders can be drawn to the left, right, above or below
          a paragraph.
        * Control over line styling is available.

    - Font Control:
        - full font control is allowed for all text features.
        - size, type and style.

    * Color Control:
        - color of text is controlable.

    - Paragraph Formating:
        - full control over paragraph formating is allowed.
        - low level support, with line wrapping, is available.
        - access to indentation, paragraph alignment, word wrapping,
          spacing, etc. is possible.

    * Table:
        * table support is available.

    * Multi-file On-Line Help:
        * can be done, but must be explicitly specified in the online help.

    * Accelerator Keys:
        * can be set.

    * Appendable Menus:
        * The online help menus can be modified.

    - Appendable Buttons:
        - buttons can be added to the button bar.

    - Book Marks:
        - users can mark help topics and return to them.

    - Macros:
        - runtime macro support, with conditional processing.


OS/2 IPF Online Help Structure:
===============================
    This section describes OS/2 online help (IPF).

    - Topic Hierachy:
        - IPF has a formal GML heading hierachy. This is exposed as the
          primary view to the user.
        - H1 to H6 are supported.

    - Indexing:
        - two level indexing is available.

    - Control Words:
        - .im: imbedded files.
        - .*: comments.
        - .br: break line.

    - Font Control:
        - 9 hilighting styles (:hp1 to :hp9).
        - direct control over font: size, color, face.
        ? style?

    - List Control:
        - simple lists.
        - unordered lists.
        - ordered lists.
        - definition lists.
        - parameter lists.

    - Tables:
        - supported, with lining.

    - Figures:
        - supported.

    - examples:
        - supported.

    - bitmaps:
        - supported. Similar to Windows.

    - Hyperlinks:
        - supported, like Windows.

    - Graphic Hyperlinks:
        - supported, including segmented graphics.

    - Popup Definitions:
        - supported through footnotes.

    - Multiple Windows:
        - can be done.
        - powerful features. Not supported in Windows.

    - Split Windows:
        - can be done.
        - Not supported in Windows.


Using The GML Help Layouts:
===========================
    A layout, tag set, and device driver have been written for GML
    to generate WHP help format files. The WPROJ project you need
    to generate WHP files is in docs\doc\whelp. In there,
    you will find:
        - whelp.opt
        - whelp.gml
        - whelp.pcd
        - whelpdrv.pcd
        - whelplay.gml
        - wnohelp.gml

    To process your online help, use a GML command line like:

        wgml <book>.gml (pass 2 output (t:2000)<book>.whp file whelp.opt

    You will probably want to define variables which indicate that you
    are processing for online help (since certain GML features should
    not be included). This is what the main file for your book should
    end up like:
        :GDOC.
        :FRONTM.
        :BODY.
        :exhelp

    Don't process any GML which generates the title page, table of contents,
    figlist or index. These features don't make sense in online help
    (both are generated by WHPCVT).

    When not processing for online help, include 'wnohelp.gml'. This
    file defines the HELP macros to do what should be done for non-help
    processing (which in most cases is nothing).


    How it Works:
    -------------
    'whelp.gml' redefines all the important base tags to generate 'WHP'
    information. The driver (whelpdrv.pcd) provides the necessary
    font support.

    One consequence of redefining the base tags is that a problem
    occurs with higher level macros which reference base tags.
    In a macro (i.e. a tag), a usage of a base tag
    (i.e. :li) will refer directly to the base definition, rather
    than the redefinition. Outside of macros, the base tags will
    refer to the redefinition.

    To get around this, all the overridden base tags have equivalent
    tags names which are prefixed with 'z' (i.e. :zli for :li). In
    any macros you have, you will need to change references to
    base tags to be prefixed with 'z'. Both 'whelp.gml' and 'wnohelp.gml'
    have these defined, so you can switch to the 'z' versions
    for help and non-help processing.

    Behaviour for GML Tags:
    -----------------------
    Most base GML tags will process without alterations. Most tags
    do not observe the various options which are possible; base GML
    document elements are supported in only one help format, generally
    (this is a limitation of the help environments, in some cases).
    Listed below are all of the base GML tags and their various features
    and limitations.

    :DL
        - format is fixed (attributes do nothing).
        - terms are always broken across lines.
        - a blank line follows the description.
        - DTHD and DDHD are not supported (if you need these, come
          talk to Dan about it).

    :OL
        - format is fixed (attributes do nothing).
        - a blank line follows each item.
        - for OS consistency, the numbering scheme and format may change
          slightly for each help platform.

    :UL
        - format is fixed (attributes do nothing).
        - format is 'compact' (no blank lines).

    :SL
        - format is fixed (attributes do nothing).
        - format is 'compact' (no blank lines).

    :H0-H4
        - forms a new topic.
        - heading levels are used for OS/2 IPF structure, and to generate
          the table of contents.

    :H5-H6
        - forms a subtitle within a topic.

    :I1
        - adds a keyword for a topic, used in 'search'.

    :FIGREF
        - generates a figure reference with no page indicator.

    :HDREF
        - generates a hyperlink to the corresponding topic.
        - do not 'HDREF' to H5 level headings.

    :GRAPHIC
        - not implemented.
        - the usual approach is to cover the ':fig' usage with a
          tag which generates the appropriate tags for printing,
          and alternative references for online help (i.e. use
          ':HBMP').

    New GML Help Tags:
    ------------------
    A strict online book will not use any tags other than the base GML
    tags. In some cases, you may want to add specialized features
    for online help. For example, headings can be defined such that they
    are prefixed with extra text for online help (useful since topic
    titles must be unique). Use the tags listed below with caution!

    :QREF str='heading' [pfx='prefix'].
        - generates a hyperlink to the topic 'heading'.
        - The displayed text for the hyperlink is the topic name.
        - if 'pfx' is supplied, it is prefixed before 'heading'
          to determine the actual topic name. This is useful
          only if you are generating different heading names in
          your help file from the printed book.
        - the nonhelp version of this tag simply outputs 'str'.

    :POPREF str='heading' [pfx='prefix'].
        - similar to ':QREF', does creates a popup definition
          hyperlink instead.

    :SEEALSO ['heading']*
        - generates a 'See Also' list of hyperlinks to the topic
          'heading's (i.e. :seealso 'File Menu' 'Options bar').
        - up to nine topic headings can be supplied.
        - the nonhelp version of this tag does nothing.

    :CONTTPC 'heading' ['order_num']
        - continues a topic, as if the following paragraphs followed
          directly after the original topic.
        - if 'order_num' is provided, then this section is ordered
          by its order number (lowest first). Ordered sections come
          after unordered sections. 'order_num' can be 0 or greater.

    :POPHELP [pfx='prefix'] [cnt=yes|no].'heading'
        - defines a topic for popup help. Though any topic can be
          a popup definition, using this tag will format the topic
          more appropriately (no line under the title).
        - 'cnt=yes' indicates that this topic should appear in
          the table of contents. 'cnt=no' will prevent the topic
          from appearing in the table of contents.
        - 'pfx'is the same as with 'QREF'.
        - the nonhelp version generates an :h4 heading. 'pfx' is ignored.

    :EXHELP.
        - causes subsequent output not to appear until the next topic
          definition (heading).

    :HLINK [ctx='ctx_name' | hd='heading'].
        - starts the definition of a hyperlink to a topic. Either
          'ctx' or 'hd' should be supplied (but not both).
        - If 'hd' is supplied, 'heading' is the name of the topic to
          link to.
        - If 'ctx' is supplied, 'ctx_name' defines the context name
          for the topic to link to. Context names are formed
          from heading titles by replacing non-variable characters
          with underscores (i.e. spaces).
        - the text following and up to the next ':EHLINK' is the
          displayed as the hyperlink text.
        - normally, this tag is not used. Use ':HDREF' or ':QREF'
          instead, unless you want to hyperlink arbitrary text.

    :EHLINK.
        - ends hyperlink definition.

    :HDFN [ctx='ctx_name' | hd='heading'].
        - similar to ':HLINK', does creates a popup definition
          hyperlink instead.
        - normally, this tag is not used. Use ':POPREF' instead,
          unless you want a popup definition for arbitrary text.

    :EHDFN.
        - used with ':HDFN'.

    :HBMP 'file' i|r|l|c
        - inserts a graphic bitmap picture into the helpfile.
        - 'file' is the file name of the bitmap (with .bmp extension).
          A path may be included, but is not normally necessary.

    :CTXKW ['keyword']*
        - adds keywords to the current topic, which are used in the
          search function of the help environment.
        - use this if you want to add search keywords which are not in
          the printed index (':i1' uses this tag for online help).

    :HELPLVL [level='hlvl'].
        - sets the heading level at which headings form separate topics.
          Headings from level 0 to 'hlvl' are created as separate topics.
          'hlvl' is 4 by default, and cannot be set higher (H5 and H6
          always form inline headings).

    :HELPPFX [pfx='prefix'].
        - sets the heading prefix. For all following headings (H0-H6),
          this prefix is added in front. Use ':HELPPFX' without
          a 'pfx' attribute to cancel a prefix for further headings.

    :BOX.
        - starts box framing online-help. All following text until :EBOX
          will be framed in a solid outlined box. This tag does nothing
          for non-help processing: this is because boxing in Script
          is layout dependent.

    :EBOX.
        - ends box framing for online-help.

    :TABXMP <char> [<stop>]*.
        - begins a tabbed example. Similar to an example, except that
          the font is not changed and tab spacing is allowed. Tab
          stops at each 'stop' character position are set. Each
          occurence of <char> is converted into a tab and spacing
          will occur accordingly. The tabbed example continues
          until the next ':ETABXMP'.
        - works for online help and postscript. Average character spacing
          is used in RTF. A switch to a mono font is done for IPF.
        - reasonably high tabs stops should be used to guarantee similar
          text output for all platforms.
        - only plain text should be contained in a :TABXMP, as is
          done with :XMP. The exception is hyperlinks (which are allowed).
          Font changing will work, in some cases (i.e. you can get
          underlined headings in a table, or change the font
          for the whole table, but you shouldn't mix and match in general).

    :ETABXMP.
        - ends a tabbed example.

    :FLINK file='file_name' hd='topic_title'.
        - starts the definition of a hyperlink to a topic in another
          help file.
        - 'file_name' is the name of the help file which contains
          the topic you want to jump to. Do not include the extension.
        - 'topic_title' is the topic title (not the context name)
          you want to jump to. You can only jump to help files created
          with the WHP process.
        - the text following and up to the next ':EFLINK' is the
          displayed as the hyperlink text.

    :EFLINK.
        - ends file hyperlink definition.


    'Z' Tags:
    ---------
    Each base GML tag has an equivalent tag defined with a slightly
    different name; each is prefixed by a 'Z'. You can use the 'Z'
    versions of base tags for both help and non-help processing.

    The reason these tags exist is to get around a difficult GML
    problem; in macros, a reference to a base tag is ALWAYS to
    the base version, even if it has been redefined. WHELP.GML
    redefines the base tags, so any macros you have which use base
    tags would not work during help processing. To get around
    this, use the 'Z' versions of the base tags in your macros. They
    will do the right thing for help and non-help processing.

    The 'ZH0-4' tags have an additional attribute; ctx. If 'ctx' is
    not specified, the context name for the heading is defined
    internally from the heading title. If 'ctx' is provided, it
    is used internally for the context name. The context name is
    used for hyperlinks.


