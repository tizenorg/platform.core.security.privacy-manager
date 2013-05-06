Name:    privacy-manager-server
Summary: Privacy Management
Version: 0.0.3
Release: 1
Group:   System/Libraries
License: SAMSUNG
Source0: %{name}-%{version}.tar.gz
Source1: privacy-manager-server.manifest
Source2: privacy-manager-client.manifest
Source3: privacy-manager-server.service
Source4: capi-security-privacy-manager.manifest
Source5: tizenprv00.privacy-popup.manifest
BuildRequires: cmake
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(db-util)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: gettext-tools

Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Privacy Management

%package -n privacy-manager-server-devel
summary: privacy-manager server
Group: Development/Libraries
Requires: privacy-manager-server = %{version}-%{release}

%description -n privacy-manager-server-devel
privacy-manager server devel

%package -n privacy-manager-client
summary: privacy-manager client
Group: Development/Libraries
Requires: privacy-manager-server = %{version}-%{release}

%description -n privacy-manager-client
privacy-manager client

%package -n privacy-manager-client-devel
Summary:    privacy-manager client devel
Group:      Development/Libraries
BuildRequires:  pkgconfig(libxml-2.0)
Requires:   privacy-manager-client = %{version}-%{release}

%description -n privacy-manager-client-devel
Privacy Management(development files)

%package -n capi-security-privacy-manager
Summary:    Privacy Manager API
Group:      TO_BE/FILLED_IN
License:    TO BE FILLED IN
BuildRequires:  cmake
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(glib-2.0)
Requires(post): /sbin/ldconfig  
Requires(postun): /sbin/ldconfig
Requires: privacy-manager-client = %{version}-%{release}

%description -n capi-security-privacy-manager
The Privacy Manager API provides functions to get/set information about privacy information of installed packages.

%package  -n capi-security-privacy-manager-devel
Summary:  Privacy Manager API (Development)
Group:    TO_BE/FILLED_IN
Requires: privacy-manager-client = %{version}-%{release}

%description -n capi-security-privacy-manager-devel
The Privacy Manager API provides functions to get/set information about privacy information of installed packages.(DEV)

%package -n tizenprv00.privacy-popup
Summary:  Privacy Popup
Group:    TO_BE/FILLED_IN
Requires(post): /sbin/ldconfig
Requires(postun): /sbin/ldconfig
BuildRequires: cmake
BuildRequires: gettext-tools
BuildRequires: edje-tools
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(icu-i18n)
BuildRequires: pkgconfig(bundle)
BuildRequires: pkgconfig(glib-2.0)

%description -n tizenprv00.privacy-popup
The Privacy popup provides UI to set privacy information of application.

%prep
%setup -q

%build
#%{!?build_type:%define build_type "Release"}

echo cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" 
cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" 
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.APLv2 %{buildroot}/usr/share/license/privacy-manager-server
mkdir -p %{buildroot}/usr/share/license
cp LICENSE.APLv2 %{buildroot}/usr/share/license/privacy-manager-client
mkdir -p %{buildroot}/usr/bin
cp res/usr/bin/* %{buildroot}/usr/bin/
mkdir -p %{buildroot}/opt/dbspace
cp res/opt/dbspace/.privacylist.db /%{buildroot}/opt/dbspace/
#mkdir -p %{buildroot}/etc/rc.d/init.d
#cp res/etc/rc.d/init.d/* %{buildroot}/etc/rc.d/init.d/

%make_install -n privacy-manager-client
install -D %{SOURCE2} %{buildroot}%{_datadir}/privacy-manager-client.manifest

%make_install -n privacy-manager
install -D %{SOURCE1} %{buildroot}%{_datadir}/privacy-manager-server.manifest

%make_install -n capi-security-privacy-manager
install -D %{SOURCE4} %{buildroot}%{_datadir}/capi-security-privacy-manager.manifest

%make_install -n privacy-manager-privacy-popup
install -D %{SOURCE5} %{buildroot}%{_datadir}/tizenprv00.privacy-popup.manifest

#mkdir -p %{buildroot}/etc/rc.d/rc3.d
#mkdir -p %{buildroot}/etc/rc.d/rc5.d
#ln -sf /etc/rc.d/init.d/privacy-manager-server.sh %{buildroot}/etc/rc.d/rc3.d/S10privacy-manager-server.sh
#ln -sf /etc/rc.d/init.d/privacy-manager-server.sh %{buildroot}/etc/rc.d/rc5.d/S10privacy-manager-server.sh

mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants
install -m 0644 %{SOURCE3} %{buildroot}%{_libdir}/systemd/system/privacy-manager-server.service
ln -sf /usr/lib/systemd/system/privacy-manager-server.service %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants/privacy-manager-server.service

%clean
rm -rf %{buildroot}

%post -n privacy-manager-server
/sbin/ldconfig

echo "Check privacy DB"
if [ ! -f /opt/dbspace/.privacy.db ]
then
	echo "Create privacy DB"
	/usr/bin/privacy_manager_create_clean_db.sh
fi

%postun
/sbin/ldconfig

%files -n privacy-manager-server
%defattr(-,root,root,-)
%manifest %{_datadir}/privacy-manager-server.manifest
%{_bindir}/*
%{_libdir}/systemd/*
#/etc/rc.d/init.d/privacy-manager-server.sh
#%attr(755,root,root) /etc/rc.d/init.d/privacy-manager-server.sh
#/etc/rc.d/rc3.d/S10privacy-manager-server.sh
#/etc/rc.d/rc5.d/S10privacy-manager-server.sh
/usr/share/license/privacy-manager-server
/opt/dbspace/.privacylist.db

%files -n privacy-manager-server-devel
%{_libdir}/pkgconfig/privacy-manager-server.pc

%files -n privacy-manager-client
%defattr(-,root,root,-)
%manifest %{_datadir}/privacy-manager-client.manifest
%{_libdir}/libprivacy-manager-client.so*
/usr/share/license/privacy-manager-client
/usr/etc/package-manager/parserlib/libprivileges.so

%files -n privacy-manager-client-devel
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/privacy-manager-client.pc

%files -n capi-security-privacy-manager
%{_libdir}/libcapi-security-privacy-manager.so.*
%manifest %{_datadir}/capi-security-privacy-manager.manifest

%files -n capi-security-privacy-manager-devel
%{_includedir}/privacymgr/*.h
%{_libdir}/libcapi-security-privacy-manager.so
%{_libdir}/pkgconfig/capi-security-privacy-manager.pc

%files -n tizenprv00.privacy-popup
%manifest %{_datadir}/tizenprv00.privacy-popup.manifest
%defattr(-,root,root,-)
/usr/bin/tizenprv00.privacy-popup
/usr/share/packages/tizenprv00.privacy-popup.xml
#/etc/smack/accesses2.d/tizenprv00.privacy-popup.rule
