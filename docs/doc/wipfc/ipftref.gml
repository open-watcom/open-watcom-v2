.*
.*
.chap *refid='ipftref' IPF Tag Reference
.*
.np
This section acts as a reference, describing how &ipfcmd implements each tag.
.*
.section Overview
.np
Tags are the heart of IPF markup and come in four flavors:
.autopoint
.point
Tags that appear in the document header. These tags control how the document behaves.
.point
Tags that establish blocks. These tags can contain other tags as well as text. List-blocks and tables are 
special cases in that they can only contain other tags and cannot directly contain text.
.point
Tags that are in-line. These tags can contain text or other in-line tags.
.point
Formatting tags that can appear anywhere, but are usually in-line.
.endpoint
.np
This diagram classifies each tag and shows which tags can contain which other tags. Tags that are marked as 
containing 'in-line' can also contain formatting and text.
.millust begin
Tag Name            Classification
userdoc
  title             (doc header)
  docprof           (doc header)
  ctrldef           (doc header)
    pbutton         (doc header)
    ctrl            (doc header)
  ectrldef          (doc header)
  fn                (block)
    block
    in-line
  efn
  h1                (block)
    h2
      h3
        h4
          h5
            h6
    acviewport      (block)
    artlink         (in-line)
      link
    eartlink
    artwork         (in-line)
    caution         (block)
      in-line
    ecaution
    cgraphic        (block)
      in-line
    ecgraphic
    color           (format)
    ddf             (block)
    dl              (list-block)
      dthd          (block)
        in-line
      ddhd          (block)
        in-line
      dt            (block)
        in-line
      dd            (block)
        in-line
        block
        list-block
    edl
    fig             (block)
      in-line
      figcap        (block)
        text
    efig
    font            (format)
    hdref           (in-line)
    hide            (format)
      block
      in-line
    ehide
    hp1 - hp9       (format)
      block
      in-line
    ehp1 - ehp9
    i1              (block)
      text
    i2              (block)
      text
    icmd            (block)
      text
    isyn            (block)
      text
    lines           (block)
      in-line
    elines
    link            (in-line)
      in-line
    elink
    lm              (format)
    note            (block)
      in-line
    nt              (block)
      in-line
      block
    ent
    ol              (list-block)
      li            (block)
        in-line
      lp            (block)
        in-line
      list-block
    eol
    p               (block)
      in-line
    parml           (list-block)
      pt            (block)
        in-line
      pd            (block)
        in-line
      list-block
    eparml
    rm              (format)
    sl              (list-block)
      li            (block)
        in-line
      lp            (block)
        in-line
      list-block
    esl
    table           (block)
      row           (block)
        c           (block)
          lines     (block)
            text
          elines
          in-line
    etable
    ul              (list-block)
      li            (block)
        in-line
      lp            (block)
        in-line
      list-block
    eul
    warning         (block)
      in-line
    ewarning
    xmp             (block)
      in-line
    exmp
euserdoc
.millust end
.*
.section Reference
.np
The &ipfname follows the behavior of the IBM OS/2 Help Compiler as documented in the "ipfref.inf"
help file. This means that the behavior of the two compilers is very similar but not identical, since the
IBM compiler allows certain things that the documentation says it should not. The entry for each tag that 
follows describes how the &ipfname treats each tag.
.*
.beglevel
.*
:INCLUDE file='acvwprt'.
:INCLUDE file='artlink'.
:INCLUDE file='artwork'.
:INCLUDE file='caution'.
:INCLUDE file='cgraphic'.
:INCLUDE file='color'.
:INCLUDE file='ctrl'.
:INCLUDE file='ctrldef'.
:INCLUDE file='ddf'.
:INCLUDE file='dl'.
:INCLUDE file='docprof'.
:INCLUDE file='fig'.
:INCLUDE file='figcap'.
:INCLUDE file='font'.
:INCLUDE file='fn'.
:INCLUDE file='hn'.
:INCLUDE file='hdref'.
:INCLUDE file='hide'.
:INCLUDE file='hpn'.
:INCLUDE file='i1'.
:INCLUDE file='i2'.
:INCLUDE file='icmd'.
:INCLUDE file='isyn'.
:INCLUDE file='li'.
:INCLUDE file='lines'.
:INCLUDE file='link'.
:INCLUDE file='lm'.
:INCLUDE file='lp'.
:INCLUDE file='note'.
:INCLUDE file='nt'.
:INCLUDE file='ol'.
:INCLUDE file='p'.
:INCLUDE file='parml'.
:INCLUDE file='pbutton'.
:INCLUDE file='rm'.
:INCLUDE file='sl'.
:INCLUDE file='table'.
:INCLUDE file='title'.
:INCLUDE file='ul'.
:INCLUDE file='userdoc'.
:INCLUDE file='warning'.
:INCLUDE file='xmp'.
.*
.endlevel
