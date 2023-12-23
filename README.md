## Open Watcom v2 Fork
|Project Build Status||Download|
|---|---|---|
|![Build Status](https://github.com/open-watcom/open-watcom-v2/actions/workflows/ci-build.yml/badge.svg)|CI Build|[Github Release](https://github.com/open-watcom/open-watcom-v2/releases/tag/Last-CI-build) , [GitHub Actions Build](https://github.com/open-watcom/open-watcom-v2/actions/workflows/ci-build.yml)|
|![Build Status](https://github.com/open-watcom/open-watcom-v2/actions/workflows/release.yml/badge.svg)|Current Release Build|[Github Release](https://github.com/open-watcom/open-watcom-v2/releases/tag/Current-build) , [GitHub Actions Build](https://github.com/open-watcom/open-watcom-v2/actions/workflows/release.yml)|
|![Build Status](https://github.com/open-watcom/open-watcom-v2/actions/workflows/coverity.yml/badge.svg) ![Coverity Scan](https://scan.coverity.com/projects/2647/badge.svg?flat=1)|Coverity Scan|[Analysis Results](https://scan.coverity.com/projects/open-watcom-open-watcom-v2) , [GitHub Actions Build](https://github.com/open-watcom/open-watcom-v2/actions/workflows/coverity.yml)|
||Releases Archive|[**All Github Releases**](https://github.com/open-watcom/open-watcom-v2/releases)
###
![WikiDocs](https://github.com/open-watcom/open-watcom-v2/workflows/WikiDocs/badge.svg)[](https://github.com/open-watcom/open-watcom-v2/actions?query=workflow%3AWikiDocs)
###
## Welcome to the Open Watcom v2 Project! 

For more information about the project and build instructions see the GitHub wiki.

Discuss the Project on GitHub, Reddit or Discord.
----------------------------------------------------------------------------

GitHub, join the discussion [`Open Watcom on GitHub`](https://github.com/open-watcom/open-watcom-v2/discussions)

Reddit Server, join the discussion [`Open Watcom on Reddit`](https://www.reddit.com/r/OpenWatcom/)

Discord Server for Open Watcom 2.0, use following [invite link](https://discord.gg/39w5wZM89p) to setup access to Open Watcom 2.0 Discord server.
This Discord Server is moderated by the Open Watcom 2.0 Github group to remove spam, unrelated discussions about personal opinions, etc. It is intended for user and developer assistance with Open Watcom 2.0. It is possible to ask about an older versions of Open Watcom, but it is primarily for Open Watcom V2.

Other general Discord server for "Open Watcom" exists [invite link](https://discord.gg/5WexPNn). It is mainly for older versions of Open Watcom.

[`Official OpenWatcom site`](http://openwatcom.org) only WEB site is up, all other services (bugzilla, Wiki, News server, Perforce) is down for long time, it looks like it is dead.

Source Tree Layout
------------------

Open Watcom allows you to place the source tree almost anywhere (although
we recommend avoiding paths containing spaces). The root of the source
tree should be specified by the `OWROOT` environment variable in `setvars`
(as described in [`Build`](https://github.com/open-watcom/open-watcom-v2/wiki/Build) document). All relative paths in this document are
taken relative to `OWROOT` location. Also this document uses the backslash
character as a path separator as is normal for DOS, Windows, and OS/2. Of
course on Linux systems a slash character should be used instead.

The directory layout is as follows:

    bld
      - The root of the build tree. Each project has a subdirectory under
        bld. For example:
          bld\cg       -> code generator
          bld\cc       -> C compiler
          bld\plusplus -> C++ compiler
          (see projects.txt for details)

    build
      - Various files used by building tools. Of most interest are the
        *.ctl files which are scripts for the builder tool (see below)
        and make files (makeint et al.).

    docs
      - Here is everything related to documentation, sources and tools.

    distrib
      - Contains manifests and scripts for building binary distribution
        packages.

    contrib
      - Third party source code which is not integral part of Open Watcom.
        This directory contains especially several DOS extenders.

    build\bin
      - This is where all build tools created during phase one are placed.

    rel
      - This is default location where the software we actually ship gets
        copied after it is built - it matches the directory structure of
        our shipping Open Watcom C/C++/FORTRAN tools. You can install the
        system by copying the rel directory to your host and then setting
        several environment variables.

        Note: the rel directory structure is created on the fly. The
        location of rel tree can be changed by OWRELROOT environment
        variable.

OpenWatcom Installation
-----------------------
[Installer installation instruction](https://open-watcom.github.io/open-watcom-v2-wikidocs/c_readme.html#Installation) in Documentation (OW Wiki).

OpenWatcom Building
-------------------
[Building instruction](https://github.com/open-watcom/open-watcom-v2/wiki/Build) in OW Wiki.

[Open Watcom Licence](https://github.com/open-watcom/open-watcom-v2/blob/master/license.txt)
---------------------
