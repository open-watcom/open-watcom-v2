[![Build Status](https://dev.azure.com/open-watcom/open-watcom-v2/_apis/build/status/open-watcom-v2-CI?branchName=master&stageName=Last%20CI%20build) - CI Build](https://dev.azure.com/open-watcom/open-watcom-v2/_build/latest?definitionId=11&branchName=master)

[![Build Status](https://dev.azure.com/open-watcom/open-watcom-v2/_apis/build/status/open-watcom-v2-Release?branchName=master&stageName=GitHub%20Release) - Release Build](https://dev.azure.com/open-watcom/open-watcom-v2/_build/latest?definitionId=14&branchName=master)

[![Build Status](https://dev.azure.com/open-watcom/open-watcom-v2/_apis/build/status/open-watcom-v2-Coverity%20Scan?branchName=master) - Coverity Scan](https://dev.azure.com/open-watcom/open-watcom-v2/_build/latest?definitionId=16&branchName=master)

[![WikiDocs](https://github.com/open-watcom/open-watcom-v2/workflows/WikiDocs/badge.svg)](https://github.com/open-watcom/open-watcom-v2/actions?query=workflow%3AWikiDocs)
 
[![Mirror](https://github.com/open-watcom/open-watcom-v2/workflows/Mirror/badge.svg)](https://github.com/open-watcom/open-watcom-v2/actions?query=workflow%3AMirror)

Open Watcom v2 Fork
===================

Welcome to the Open Watcom v2 Project! 

For more information about the project and build instructions see the GitHub wiki.

Discuss the Project on Discord, Reddit, GitHub or News Groups.
----------------------------------------------------------------------------

We now have an Official Discord server!
Join the discussion [`here`](https://discord.gg/5WexPNn)!

Reddit Server, join the discussion [`Open Watcom on Reddit`](https://www.reddit.com/r/OpenWatcom/)

GitHub, join the discussion [`Open Watcom on GitHub`](https://github.com/open-watcom/open-watcom-v2/issues)

Open Watcom News Groups, join the discussion [`Open Watcom News Groups`](news.openwatcom.org) It looks like this site is dead.

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
        location of rel tree can be changed by `OWRELROOT` environment
        variable.

OpenWatcom Installation
-----------------------
[Installer installation instruction](https://open-watcom.github.io/open-watcom-v2-wikidocs/c_readme.html#Installation) in Documentation (OW Wiki).

OpenWatcom Building
-------------------
[Building instruction](https://github.com/open-watcom/open-watcom-v2/wiki/Build) in OW Wiki.
