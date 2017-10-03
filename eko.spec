#based on Ercole 'ercolinux' Carpanetto package
Name:          eko
Version:       5.3.4
Release:        %mkrel 1
Summary:       Eko is a small audio editor with mixer and realtime fx
Group:         Editors
Vendor:        semiletov
Distribution:  semiletov
Packager:      Peter Semiletov <semiletov@gmail.com>
URL:           http://semiletov.org/eko/
Source:        http://semiletov.org/eko/dloads/eko-%{version}.tar.bz2
License:       GPL V3
## AUTOBUILDREQ-BEGIN
BuildRequires: glibc-devel
BuildRequires: libgcc
BuildRequires: libsndfile-devel 
BuildRequires: libsamplerate-devel
BuildRequires: libportaudio-devel
#BuildRequires: libGL-devel
#BuildRequires: libhunspell-devel
BuildRequires: libqt5-devel
BuildRequires: libstdc++6-devel
#BuildRequires: libz-devel
## AUTOBUILDREQ-END
BuildRoot:     %{_tmppath}/%{name}-%{version}-root

%define debug_package %{nil}

## %debug_package

%description
EKO is sound editor. It features real-time FX rack; generators of sine, noise;
channel converter; RMS and level analysis; DC offset corrector; reverese; handy editing;
color palettes; hotkeys customizations.

%prep
%setup -q
#%patch0 -p1

%build
qmake PREFIX=%{_bindir}
%make

%install
[ "%{buildroot}" != / ] && rm -rf "%{buildroot}"
%makeinstall INSTALL_ROOT=%{buildroot}/ 

%clean
[ "%{buildroot}" != / ] && rm -rf "%{buildroot}"

%files
%defattr(-,root,root)
%{_bindir}/eko
#%doc AUTHORS COPYING
/usr/share/applications/eko.desktop
/usr/share/icons/hicolor/64x64/apps/eko_icon.png
%changelog
