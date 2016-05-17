Name:           wgt-manifest-handlers
Summary:        wgt-manifest-handlers
Version:        1.5
Release:        1
Group:          Application Framework/Package Management
License:        Apache-2.0 and BSD-3-Clause
Source0:        %{name}-%{version}.tar.gz
Source100:      %{name}.manifest

BuildRequires:  boost-devel
BuildRequires:  cmake
BuildRequires:  gtest-devel
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(pkgmgr-parser)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(libiri)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(xdgmime)
BuildRequires:  pkgconfig(manifest-parser)

Requires: ca-certificates-tizen
Requires: libtzplatform-config

%description
This is a meta package that installs wgt manifest handlers.

%package devel
Summary:    Wgt manifest handlers development files
Group:      Application Framework/Package Management
Requires:   %{name} = %{version}
Requires:   boost-devel

%description devel
This package contains necessaary files for wgt-manifest-handlers development

%package tests
Summary: Unit tests for wgt-manifest-handlers
Requires: %{name} = %{version}

%description tests
Unit tests for al modules of manifest-parser

%prep
%setup -q
cp %{SOURCE100} .

%build
export CFLAGS="$(echo $CFLAGS | sed 's/-Wl,--as-needed//g')"
export CXXFLAGS="$(echo $CXXFLAGS | sed 's/-Wl,--as-needed//g')"
export FFLAGS="$(echo $FFLAGS | sed 's/-Wl,--as-needed//g')"
# BUILD_PARSER macro is defined so we can build two separate projects
# with cmake, which are on one repository. Later this macro
# is used in CMakeLists.txt files to distinguish, which project
# is currently being build.
%cmake . -DCMAKE_BUILD_TYPE=%{?build_type:%build_type} \
         -DTIZEN_VERSION=%{tizen_version} \
         -DTIZEN_PROFILE=%{profile}

make %{?_smp_mflags}

%install
%make_install

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%manifest %{name}.manifest
%defattr(-,root,root)
%{_libdir}/libwgt-manifest-handlers.so*
%license LICENSE LICENSE-xwalk

%files devel
%manifest %{name}.manifest
%{_libdir}/pkgconfig/*.pc
%{_includedir}/wgt_manifest_handlers/*.h

%files tests
%manifest %{name}.manifest
%{_bindir}/manifest-parser-ut/*
%{_datadir}/manifest-parser-ut/*
