# RPM spec for libzip
# This was tested on OpenSUSE 10.3 x86_64; should build fine on 32bit SUSE
# and other distributions, as it uses the system-defined macros for
# installation locations etc.

Name: libzip
Version: 0.9
Release: 1
Group: System/Libraries
License: BSD
Packager: Stephen Bryant <steve@bawue.de>
BuildRoot: %{_tmppath}/%{name}-%{version}-build
Source0: %{name}-%{version}.tar.gz
Requires: zlib >= 1.1.2
BuildRequires: zlib-devel >= 1.1.2, grep, make
URL: http://www.nih.at/libzip/
Summary: Library for reading, creating, and modifying zip archives
%description
This is libzip, a C library for reading, creating, and modifying zip archives.
Files can be added from data buffers, files, or compressed data copied directly
from other zip archives. Changes made without closing the archive can be
reverted. The API is documented by man pages.


%package devel
Summary: Include files and libraries needed for development
Group: Development/Languages/C and C++
Requires: zlib-devel
%description devel
This is libzip, a C library for reading, creating, and modifying zip archives.
Files can be added from data buffers, files, or compressed data copied directly
from other zip archives. Changes made without closing the archive can be
reverted. The API is documented by man pages.

For more information, take a look at the included man pages. You can start with
libzip(3), which lists all others.

There is a mailing list for libzip. You can subscribe to it by sending a mail
with the subject "subscribe libzip-discuss" to minimalist at nih.at.

%prep
%setup

%build
%configure
%{__make} -j$(($(%{__grep} -E '^processor' /proc/cpuinfo|wc -l)+1))

%install
%{__make} DESTDIR=${RPM_BUILD_ROOT} install

%clean
%{__rm} -rf ${RPM_BUILD_ROOT}

%files
%defattr(-,root,root)
%{_prefix}/bin
%{_libdir}/*.so
%{_libdir}/*.so.*
%{_mandir}/man1

%files devel
%defattr(-,root,root)
%{_prefix}/include
%{_libdir}/*.a
%{_libdir}/*.la
%{_libdir}/pkgconfig
%{_mandir}/man3

%post
ldconfig

%postun
ldconfig
