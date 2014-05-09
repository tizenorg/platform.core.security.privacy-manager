Name:           privacy-manager-server
Version:        0.0.4
Release:        0
License:        Apache-2.0
Summary:        Privacy Management
Group:          Security/Libraries
Source0:        %{name}-%{version}.tar.gz
Source1:        privacy-manager-server.service
Source1001:     privacy-manager-server.manifest
Source1002:     privacy-manager-server-devel.manifest
Source1003:     privacy-manager-client.manifest
Source1004:     privacy-manager-client-devel.manifest
Source1005:     capi-security-privacy-manager.manifest
Source1006:     capi-security-privacy-manager-devel.manifest
BuildRequires:  cmake
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(db-util)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:	pkgconfig(capi-system-info)
BuildRequires:	pkgconfig(libtzplatform-config)

Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig

%description
Privacy Management

%package -n privacy-manager-server-devel
Summary:        Privacy Manager Server
Requires:       privacy-manager-server = %{version}

%description -n privacy-manager-server-devel
privacy-manager server devel

%package -n privacy-manager-client
Summary:        Privacy Manager client
Requires:       privacy-manager-server = %{version}

%description -n privacy-manager-client
privacy-manager client

%package -n privacy-manager-client-devel
Summary:        Privacy Manager client devel
Requires:       privacy-manager-client = %{version}

%description -n privacy-manager-client-devel
Privacy Management(development files)

%package -n capi-security-privacy-manager
Summary:        Privacy Manager API
Requires:       privacy-manager-client = %{version}

%description -n capi-security-privacy-manager
The Privacy Manager API provides functions to get/set information 
about privacy information of installed packages.

%package  -n capi-security-privacy-manager-devel
Summary:        Privacy Manager API (Development)
Requires:       capi-security-privacy-manager = %{version}

%description -n capi-security-privacy-manager-devel
The Privacy Manager API provides functions to get/set 
information about privacy information of installed packages.(DEV)

%prep
%setup -q
cp %{SOURCE1001} .
cp %{SOURCE1002} .
cp %{SOURCE1003} .
cp %{SOURCE1004} .
cp %{SOURCE1005} .
cp %{SOURCE1006} .

%build
%{!?build_type:%define build_type "Release"}
%cmake . -DPREFIX=%{_prefix} \
        -DEXEC_PREFIX=%{_exec_prefix} \
        -DLIBDIR=%{_libdir} \
        -DINCLUDEDIR=%{_includedir} \
        -DCMAKE_BUILD_TYPE=%{build_type} \
        -DVERSION=%{version} \
        -DFILTER_LISTED_PKG=ON \
        -DPRIVACY_POPUP=OFF
make %{?_smp_mflags}

%install
mkdir -p %{buildroot}%{_prefix}/bin
cp res/usr/bin/* %{buildroot}%{_bindir}/
mkdir -p %{buildroot}%{TZ_SYS_DB}
cp res/opt/dbspace/.privacylist.db /%{buildroot}%{TZ_SYS_DB}
mkdir -p %{buildroot}%{_datadir}/privacy-manager/
cp res/usr/share/privacy-manager/privacy-filter-list.ini %{buildroot}%{_datadir}/privacy-manager/

%make_install

mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants
#install -m 0644 %{SOURCE1} %{buildroot}%{_libdir}/systemd/system/privacy-manager-server.service
#ln -sf /usr/lib/systemd/system/privacy-manager-server.service %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants/privacy-manager-server.service


%post -n privacy-manager-server
/sbin/ldconfig

echo "Check privacy DB"
if [ ! -f %{TZ_SYS_DB}/.privacy.db ]
then
	echo "Create privacy DB"
	%{_bindir}/privacy_manager_create_clean_db.sh
fi
chsmack -a 'User' %{TZ_SYS_DB}/.privacy.db*

%postun -p /sbin/ldconfig

%post -n privacy-manager-client -p /sbin/ldconfig

%postun -n privacy-manager-client -p /sbin/ldconfig

%post -n capi-security-privacy-manager -p /sbin/ldconfig

%postun -n capi-security-privacy-manager -p /sbin/ldconfig

%files -n privacy-manager-server
%defattr(-,root,root,-)
%license  LICENSE.APLv2
%manifest privacy-manager-server.manifest
%{_libdir}/libprivacy-manager-server.so*
%{TZ_SYS_DB}/.privacylist.db
/usr/bin/*

%files -n privacy-manager-server-devel
%{_includedir}/privacy_manager/server/privacy_manager_daemon.h
%{_libdir}/pkgconfig/privacy-manager-server.pc

%files -n privacy-manager-client
%defattr(-,root,root,-)
%license  LICENSE.APLv2
%manifest privacy-manager-client.manifest
%{_libdir}/libprivacy-manager-client.so*
%{_datadir}/privacy-manager/privacy-filter-list.ini
%{_sysconfdir}/package-manager/parserlib/libprivileges.so

%files -n privacy-manager-client-devel
%defattr(-,root,root,-)
%manifest privacy-manager-client-devel.manifest
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
