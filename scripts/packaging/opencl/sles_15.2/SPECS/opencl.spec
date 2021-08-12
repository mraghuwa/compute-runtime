# spec file for package intel-opencl

#it's changed by external script
%global rel i1
%global ver xxx

%define gmmlib_sover  11
%define igc_sover 1

%if !0%{?build_type:1}
%define build_type Release
%endif

%define _source_payload w5T16.xzdio
%define _binary_payload w5T16.xzdio

Name:           intel-opencl
Epoch:          1
Version:        %{ver}
Release:        %{rel}%{?dist}
Summary:        Intel(R) Graphics Compute Runtime for OpenCL(TM)
License:        MIT
Group:          System Environment/Libraries
Url:            https://github.com/intel/compute-runtime
Source0:        %{url}/archive/%{version}/compute-runtime-%{version}.tar.xz
Source1:        copyright

ExclusiveArch:  x86_64
BuildRequires:  cmake make gcc-c++
#BuildRequires: libva-devel
BuildRequires: libigdgmm%{?name_suffix}-devel
BuildRequires: libigdfcl%{?name_suffix}-devel
Requires:       libigc%{igc_sover}%{?name_suffix}
Requires:       libigdfcl%{igc_sover}%{?name_suffix}
Requires:       libigdgmm%{gmmlib_sover}%{?name_suffix}

%description
Intel(R) Graphics Compute Runtime for OpenCL(TM) is a open source project to
converge Intel's development efforts on OpenCL(TM) compute stacks supporting
the GEN graphics hardware architecture.

%if 0%{?is_debug}
%package     -n intel-opencl%{?name_suffix}
Summary:        Intel(R) Graphics Compute Runtime for OpenCL(TM)
Conflicts:      intel-opencl

%description -n intel-opencl%{?name_suffix}
Intel(R) Graphics Compute Runtime for OpenCL(TM) is a open source project to
converge Intel's development efforts on OpenCL(TM) compute stacks supporting
the GEN graphics hardware architecture.
%endif

%package     -n intel-ocloc%{?name_suffix}
Summary:        ocloc package for opencl
%description -n intel-ocloc%{?name_suffix}

%define debug_package %{nil}

%prep
%autosetup -p1 -n compute-runtime-%{version}

%build
%cmake .. \
   -DCMAKE_BUILD_TYPE=%{build_type} \
   -DNEO_VERSION_BUILD=$(ver) \
   -DBUILD_WITH_L0=0 \
   -DCMAKE_INSTALL_PREFIX=/usr \
   -DSKIP_UNIT_TESTS=1 \
   -DRELEASE_WITH_REGKEYS=1 \
   -Wno-dev
%make_build

%install
cd build
%make_install
chmod +x %{buildroot}/%{_libdir}/intel-opencl/libigdrcl.so
rm -f %{buildroot}/%{_libdir}/intel-opencl/libigdrcl.so.debug
rm -f %{buildroot}/%{_libdir}/libocloc.so.debug
rm -rf %{buildroot}/usr/lib/debug/

#insert license into package
mkdir -p %{buildroot}/usr/share/doc/intel-opencl%{?name_suffix}/
cp -pR %{_sourcedir}/copyright %{buildroot}/usr/share/doc/intel-opencl%{?name_suffix}/.
mkdir -p %{buildroot}/usr/share/doc/intel-ocloc%{?name_suffix}/
cp -pR %{_sourcedir}/copyright %{buildroot}/usr/share/doc/intel-ocloc%{?name_suffix}/.

%files -n intel-opencl%{?name_suffix}
%defattr(-,root,root)
%{_sysconfdir}/OpenCL
%{_libdir}/intel-opencl/libigdrcl.so
/usr/share/doc/intel-opencl%{?name_suffix}/copyright

%files -n intel-ocloc%{?name_suffix}
%defattr(-,root,root)
%{_bindir}/ocloc
%{_libdir}/libocloc.so
%{_includedir}/ocloc_api.h
/usr/share/doc/intel-ocloc%{?name_suffix}/copyright

%changelog
* Wed May 6 2020 Pavel Androniychuk <pavel.androniychuk@intel.com> - 20.17.16650
- Update spec files to pull version automatically.

* Tue Apr 28 2020 Jacek Danecki <jacek.danecki@intel.com> - 20.16.16582-1
- Update to 20.16.16582

* Tue Mar 03 2020 Jacek Danecki <jacek.danecki@intel.com> - 20.08.15750-1
- Update to 20.08.15750

* Wed Nov 20 2019 Pavel Androniychuk <pavel.androniychuk@intel.com> - 19.43.14583
- Initial spec file