Name: repsheet
Version: 2.0.0
Release: 1
Summary: Core library
License: ASL 2.0
Group: Development/Libraries
URL: https://github.com/repsheet/librepsheet
Source0: https://github.com/repsheet/librepsheet/archive/%{name}-%{version}.zip
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: hiredis
BuildRequires: pcre-devel hiredis-devel gcc automake autoconf libtool check check-devel

%description
This library contains the business logic for Repsheet

%package devel
Summary: Development files for %{name}
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Development files (Headers, libraries for dynamic linking, etc) for %{name}

%package static
Summary: Static library for %{name}
Group: Development/Libraries

%description static
Library for static linking for %{name}

%prep
%setup -q

%build
./autogen.sh
%configure
make

%install
# rm -rf $RPM_BUILD_ROOT
# install -D -m755 src/repsheet $RPM_BUILD_ROOT/%{_bindir}/repsheet
make install DESTDIR=$RPM_BUILD_ROOT

rm -f $RPM_BUILD_ROOT%{_libdir}/*.la

%check
make check

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root,-)
#/%{_lib}/*.so.*
%{_libdir}/*.so.*
%doc LICENSE

%files devel
%defattr(-,root,root,-)
%{_libdir}/*.so
%{_libdir}/pkgconfig/*
%{_includedir}/*.h
%doc LICENSE

%files static
%defattr(-,root,root,-)
%{_libdir}/*.a
%doc LICENSE

%changelog
* Fri Jul 25 2014 Aaron Bedra <aaron@aaronbedra.com> - 2.0.0-1
- Updated API, error handling, and additional functionality
* Mon May 19 2014 Aaron Bedra <aaron@aaronbedra.com> - 1.0.0-1
- Initial Release
