Name:    privacy-manager-server
Summary: Privacy Management
Version: 0.0.3
Release: 1
Group:   Security/Libraries
License: Apache-2.0
Source0: %{name}-%{version}.tar.gz
Source1: privacy-manager-server.service
Source1001: privacy-manager-server.manifest
Source1002: privacy-manager-server-devel.manifest
Source1003: privacy-manager-client.manifest
Source1004: privacy-manager-client-devel.manifest
Source1005: capi-security-privacy-manager.manifest
Source1006: capi-security-privacy-manager-devel.manifest
Source1007: tizenprv00.privacy-popup.manifest
BuildRequires: cmake
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires: pkgconfig(libxml-2.0)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(glib-2.0)
BuildRequires: pkgconfig(sqlite3)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(dbus-1)
BuildRequires: pkgconfig(dbus-glib-1)
BuildRequires: pkgconfig(db-util)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: gettext-tools
BuildRequires: edje-tools
BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(ail)
BuildRequires: pkgconfig(appcore-efl)
BuildRequires: pkgconfig(capi-appfw-application)
BuildRequires: pkgconfig(edje)
BuildRequires: pkgconfig(eina)
BuildRequires: pkgconfig(elementary)
BuildRequires: pkgconfig(evas)
BuildRequires: pkgconfig(ui-gadget-1)
BuildRequires: pkgconfig(icu-i18n)
BuildRequires: pkgconfig(bundle)

Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Privacy Management

%package -n privacy-manager-server-devel
Summary: privacy-manager server
Group: Development/Libraries
Requires: privacy-manager-server = %{version}-%{release}

%description -n privacy-manager-server-devel
privacy-manager server devel

%package -n privacy-manager-client
Summary: privacy-manager client
Group: System/Libraries
Requires: privacy-manager-server = %{version}-%{release}

%description -n privacy-manager-client
privacy-manager client

%package -n privacy-manager-client-devel
Summary:    privacy-manager client devel
Group:      Development/Libraries
Requires:   privacy-manager-client = %{version}-%{release}

%description -n privacy-manager-client-devel
Privacy Management(development files)

%package -n capi-security-privacy-manager
Summary:    Privacy Manager API
Group:      API/C API
License:    Apache-2.0
Requires: privacy-manager-client = %{version}-%{release}

%description -n capi-security-privacy-manager
The Privacy Manager API provides functions to get/set information about privacy information of installed packages.

%package  -n capi-security-privacy-manager-devel
Summary:  Privacy Manager API (Development)
Group:    API/C API
Requires: capi-security-privacy-manager = %{version}-%{release}

%description -n capi-security-privacy-manager-devel
The Privacy Manager API provides functions to get/set information about privacy information of installed packages.(DEV)

%package -n tizenprv00.privacy-popup
Summary:  Privacy Popup
Group:    System/Security

%description -n tizenprv00.privacy-popup
The Privacy popup provides UI to set privacy information of application.

%prep
%setup -q

%build
cp %{SOURCE1001} .
cp %{SOURCE1002} .
cp %{SOURCE1003} .
cp %{SOURCE1004} .
cp %{SOURCE1005} .
cp %{SOURCE1006} .
cp %{SOURCE1007} .
#%{!?build_type:%define build_type "Release"}

%cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DDPL_LOG="ON" 
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/usr/bin
cp res/usr/bin/* %{buildroot}/usr/bin/
mkdir -p %{buildroot}/opt/dbspace
cp res/opt/dbspace/.privacylist.db /%{buildroot}/opt/dbspace/

%make_install

mkdir -p %{buildroot}%{_unitdir}/multi-user.target.wants
install -m 0644 %{SOURCE1} %{buildroot}%{_unitdir}/privacy-manager-server.service
ln -sf %{_unitdir}/privacy-manager-server.service %{buildroot}%{_unitdir}/multi-user.target.wants/privacy-manager-server.service


%post -n privacy-manager-server
/sbin/ldconfig

echo "Check privacy DB"
if [ ! -f /opt/dbspace/.privacy.db ]
then
	echo "Create privacy DB"
	/usr/bin/privacy_manager_create_clean_db.sh
fi

%postun -p /sbin/ldconfig

%post -n privacy-manager-client -p /sbin/ldconfig

%postun -n privacy-manager-client -p /sbin/ldconfig

%post -n capi-security-privacy-manager -p /sbin/ldconfig

%postun -n capi-security-privacy-manager -p /sbin/ldconfig

%files -n privacy-manager-server
%license  LICENSE.APLv2
%defattr(-,root,root,-)
%manifest privacy-manager-server.manifest
%{_bindir}/*
%{_prefix}/lib/systemd/*
/opt/dbspace/.privacylist.db

%files -n privacy-manager-server-devel
%{_libdir}/pkgconfig/privacy-manager-server.pc

%files -n privacy-manager-client
%license  LICENSE.APLv2
%defattr(-,root,root,-)
%manifest privacy-manager-client.manifest
%{_libdir}/libprivacy-manager-client.so*
/etc/package-manager/parserlib/libprivileges.so

%files -n privacy-manager-client-devel
%manifest privacy-manager-client-devel.manifest
%defattr(-,root,root,-)
%{_includedir}/*
%{_libdir}/pkgconfig/privacy-manager-client.pc



%files -n capi-security-privacy-manager
%license  LICENSE.APLv2
%{_libdir}/libcapi-security-privacy-manager.so.*
%manifest capi-security-privacy-manager.manifest

%files -n capi-security-privacy-manager-devel
%manifest capi-security-privacy-manager-devel.manifest
%{_includedir}/privacymgr/*.h
%{_libdir}/libcapi-security-privacy-manager.so
%{_libdir}/pkgconfig/capi-security-privacy-manager.pc

%files -n tizenprv00.privacy-popup
%license  LICENSE.APLv2
%manifest tizenprv00.privacy-popup.manifest
%defattr(-,root,root,-)
/usr/bin/tizenprv00.privacy-popup
/usr/share/packages/tizenprv00.privacy-popup.xml
#/etc/smack/accesses2.d/tizenprv00.privacy-popup.rule
