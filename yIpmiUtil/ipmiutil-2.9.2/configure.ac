dnl Process this file with autoconf to produce a configure script.
AC_INIT(util/ipmiutil.c)
AM_INIT_AUTOMAKE(ipmiutil,2.9.2)
AM_CONFIG_HEADER(config.h)

AC_PREFIX_DEFAULT(/usr) 

dnl Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_CHECK_PROG([SED], [sed], [sed])

dnl Checks for libraries.

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/ioctl.h syslog.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_OFF_T
AC_HEADER_TIME
AC_STRUCT_TM

dnl Checks for library functions.
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MMAP
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(select socket strcspn strdup strspn strtoul memcpy)

AM_PROG_LIBTOOL 

AC_CONST
AC_TRY_COMPILE([#include <sys/types.h>
		#include <sys/socket.h>],
	       [socklen_t len = 42; return 0;],
               ac_cv_type_socklen_t=yes,
               ac_cv_type_socklen_t=no)
if test $ac_cv_type_socklen_t != yes; then
    AC_DEFINE(socklen_t, int, [if socklen_t is not defined, provide something useful])
else
    AC_DEFINE(HAVE_SOCKLEN_T, 1, [if socklen_t is defined, make note of it])
fi

tmpspec=/tmp/iuspec.tmp$$
archm=`uname -m`
sysname=`uname -s`
LIB_DIR="/usr/lib"
isredhat=0
init_scripts="scripts/ipmi_port.sh scripts/ipmiutil_evt scripts/ipmiutil_asy scripts/ipmiutil_wdt"
projdir=`pwd`
SUBDIR_S="doc scripts lib util"
os=Linux

# ltmain.sh, config.sub, et al should have been created, but check to be sure.
beforeprog="./beforeconf.sh"
progs="config.sub ltmain.sh"
for p in $progs
do
   echo -e "checking for $p... \c"
   if test -f "$p"; then
      echo "yes"
   else
      echo "no"
      echo "Run $beforeprog to configure the build environment."
      exit 1
   fi
done

#[ if test -n "$GCC"; then 
#    CFLAGS="$CFLAGS -Wall -D_REENTRANT"
#  fi]

drv_landesk=0
dnl configure for LANDESK libipmiapi or not
AC_ARG_ENABLE([landesk],
    [  --enable-landesk       adds landesk library support [[default=no]]],
    [if test "x$enableval" = "xyes"; then
	    drv_landesk=1
            LANDESK_CFLAGS="-DLINK_LANDESK"
            LANDESK_LDADD="-lipmiapi -L../lib"
            AC_SUBST(LANDESK_CFLAGS)
            AC_SUBST(LANDESK_LDADD)
    fi]
    )

dnl Configure ipmiutil for lanplus (libipmi_lanplus.a) or not (default yes)
AC_ARG_ENABLE([lanplus],
    [  --disable-lanplus      disable lanplus support [[default=enabled]]],)
if test "x$enable_lanplus" = "xno"; then
	drv_lanplus=""
	LANPLUS_CFLAGS=""
	LANPLUS_LDADD=""
        LANPLUS_LIB="" 
	LANPLUS_CRYPTO=""
	LANPLUS_SAM="no"
	LD_SAMX=""
else
	drv_lanplus="lanplus"
	LANPLUS_CFLAGS="-DHAVE_LANPLUS"
	# LANPLUS_CFLAGS="-DHAVE_LANPLUS -I${projdir}/lib/lanplus/inc"
	# LANPLUS_LDADD="-L../lib -lintf_lanplus -L/usr/local/lib -lcrypto"
	LANPLUS_LDADD="-L../lib -lipmi_lanplus -L/usr/local/lib -lcrypto"
        LANPLUS_LIB="../lib/libipmi_lanplus.a" 
	LANPLUS_CRYPTO="-lcrypto"
	LANPLUS_SAM="yes"
	LD_SAMX="../lib/libipmi_lanplus.a -lcrypto"
fi

dnl Configure libipmiutil.a for lanplus or not
dnl Only SOL requires lanplus, so usually build libipmiutil.a without lanplus
#AC_ARG_ENABLE([liblanplus],
#    [  --enable-liblanplus      build libipmiutil.a with lanplus [[default=disabled]]],)
#if test "x$enable_liblanplus" = "xyes"; then
#   LANPLUS_SAM="yes"
#   LD_SAMX= "../lib/libipmi_lanplus.a -lcrypto"
#else
#   LANPLUS_SAM="no"
#   LD_SAMX=""
#fi

# strings /lib/libssl.so* |grep EVP_sha256 
dnl configure for SHA256, build lanplus with SHA256 support, uses later openssl.
AC_ARG_ENABLE([sha256],
    [  --enable-sha256        build with SHA256 support, requires later openssl.],
    [if test "x$enableval" = "xyes"; then
	LANPLUS_CFLAGS="$LANPLUS_CFLAGS -DHAVE_SHA256"
    fi]
    )

CROSS_LFLAGS=""
CROSS_CFLAGS=""

dnl configure for standalone, with no lanplus or GPL libs 
AC_ARG_ENABLE([standalone],
    [  --enable-standalone    build standalone, with no GPL or LanPlus libs.],
    [if test "x$enableval" = "xyes"; then
        GPL_CFLAGS=""
	LANPLUS_CFLAGS=""
	LANPLUS_LDADD=""
        LANPLUS_LIB="" 
	LANPLUS_CRYPTO=""
	LANPLUS_SAM="no"
	LD_SAMX=""
	CFLAGS="-O2"
        SUBDIR_S="doc scripts util"
	if test "x$cross_compiling" = "xyes"; then
	   # cross-compiling, so link with -static (e.g. Android ARM)
	   CROSS_LFLAGS="-static"
	   CROSS_CFLAGS="-DCROSS_COMPILE"
	   sed -i -e 's/^# ipmiutil:	$(METASOURCE:.c=.o)/ipmiutil:\t$(METASOURCE:.c=.o)\n\t$(CC) $(CFLAGS) $(LDFLAGS) -o ipmiutil $(METASOURCE:.c=.o) $(LDADD)/'  util/Makefile.am
	fi
    fi]
    )

dnl configure for GPL build or not (GPL files: md2.h, ipmi_ioctl.h)
AC_ARG_ENABLE([gpl],
    [  --enable-gpl           build with some GPL code [[default=no]]],
    [if test "x$enableval" = "xyes"; then
        GPL_CFLAGS="-DALLOW_GPL"
     else
        GPL_CFLAGS=""
    fi]
    )

dnl Does this Linux have systemd enabled?  Otherwise use sysv init.
AC_ARG_ENABLE([systemd],
    [  --enable-systemd      enable systemd service type=notify support and %_unitdir [[default=disabled]]],)
if test "x$enable_systemd" = "xyes"; then
   GPL_CFLAGS="$GPL_CFLAGS -DENABLE_SYSTEMD"
   # if systemd enabled, install service scripts in unitdir
   SYSTEMD_DIR=`rpm --eval "%{_unitdir}"`
else
   # otherwise install the systemd service scripts in the data dir
  if test  "x$sysname" = "xDarwin" ; then
    # MacOS 'which' command returns 0 always
    SYSTEMD_DIR=/usr/share/ipmiutil
  else
    which rpm >/dev/null 2>&1
    if test  $? -eq 0 ; then
       datad=`rpm --eval "%{_datadir}"`
       SYSTEMD_DIR=${datad}/ipmiutil
    else
       SYSTEMD_DIR=/usr/share/ipmiutil
    fi
  fi
fi

case "$archm" in 
   ia64)
	IA64_CFLAGS="-D__IA64__"
	LIB_DIR="/usr/lib64"
        ;;
   ppc64)
	IA64_CFLAGS="-DSTUB_IO"
	LIB_DIR="/usr/lib64"
        ;;
   x86_64)
	IA64_CFLAGS=""
	LIB_DIR="/usr/lib64"
        ;;
   i386)
	IA64_CFLAGS=""
        ;;
   i586)
	IA64_CFLAGS=""
        ;;
   i686)
	IA64_CFLAGS=""
        ;;
   sun4u)
	IA64_CFLAGS="-D__SPARC__"
        ;;
   *) 
	IA64_CFLAGS="-DSTUB_IO"
        ;;
esac

dnl # libdir can be set with --libdir=/usr/lib (default=${exec_prefix}/lib)
echo $libdir |grep exec_prefix >/dev/null 2>&1
if test $? -ne 0; then
  LIB_DIR="$libdir"
  echo "libdir set to $libdir"
fi

PKG_DIR=/usr/src/packages
INS_LIB=""
init0=
if test "x$sysname" = "xSunOS"; then
     echo "Detected Solaris"
	os=solaris
	archp=`uname -p`
        if test -d "/usr/sfw/lib"; then
	   sfwdir=/usr/sfw
	else
	   sfwdir=/usr/local
	fi
	OS_CFLAGS="-DSOLARIS -I${sfwdir}/include"
	OS_LFLAGS="-L${sfwdir}/lib -lxnet -lsocket -lnsl -lresolv"
	OS_DRIVERS="ipmibmc.c ipmilipmi.c"
	drivers="bmc lipmi"
	INS_LIB="# "
else
  if test "x$sysname" = "xLinux"; then
	MD2_CFLAGS=""
	OS_LFLAGS=""
	OS_DRIVERS="imbapi.c ipmimv.c ipmild.c ipmidir.c"
	if test $drv_landesk -eq 1; then
	   drivers="open imb landesk direct"
	else
	   drivers="open imb direct"
	fi
        os=linux
	tmpc=/tmp/tmp.c
	tmpo=/tmp/tmp.o
	echo $ECHO_N "checking compile warning flags ... $ECHO_C"
	cfwarn="-Wno-pointer-sign -Wno-sign-conversion"
	echo "int main() { return(1); }" >$tmpc
	gcc -o $tmpo -c $cfwarn $tmpc  >/dev/null 2>&1
	if test $? -ne 0 ; then
	   cfwarn=
	   echo "ok"
	else 
	   echo "suppress excess warnings"
	fi
        rm -f $tmpc $tmpo >/dev/null 2>&1
	echo $ECHO_N "checking compile fortify flags ... $ECHO_C"
	cfhard="-fstack-protector --param=ssp-buffer-size=4 -D_FORTIFY_SOURCE=2"
	echo "int main() { return(1); }" >$tmpc
	gcc -o $tmpo -c $cfhard $tmpc  >/dev/null 2>&1
	if test $? -ne 0 ; then
	   cfhard=
	   echo "skip"
	else 
	   echo "ok, add"
	fi
        rm -f $tmpc $tmpo >/dev/null 2>&1
        if test -f "/etc/debian_version"; then
           echo "Detected Debian Linux"
           os=debian
	   PKG_DIR=..
	   INS_LIB="# "
	   # use dpkg-buildpackage instead of rpmbuild
	else 
	   which rpm  >/dev/null 2>&1
	   if test $? -eq 0; then
		 pkgtop=`rpm --eval "%{_topdir}"`
	         PKG_DIR=$pkgtop
	   fi
	fi
        if test -f "/etc/redhat-release"; then
                echo "Detected Red Hat Linux"
                os=redhat
		isredhat=1
	fi
        if test -f "/etc/SuSE-release"; then
                echo "Detected SuSE Linux"
                os=suse
		sed -e 's/# POST_INSTALL/if [ ! -f \/etc\/snmp\/snmpd.conf ]; then\n if [ -f \/etc\/ucdsnmpd.conf ]; then\n mkdir -p \/etc\/snmp; ln -s \/etc\/ucdsnmpd.conf  \/etc\/snmp\/snmpd.conf\n fi\nif [ -f \/etc\/snmpd.conf ]; then\nmkdir -p \/etc\/snmp; ln -s \/etc\/snmpd.conf  \/etc\/snmp\/snmpd.conf\nfi\nfi\n#/' -e 's/make install DEST/make install-strip DEST/'  doc/ipmiutil.spec >$tmpspec
		cp -f $tmpspec doc/ipmiutil.spec
	fi
        if test -f "/etc/mvl-release"; then
                echo "Detected MontaVista Linux"
                os=montavista
		init0=/etc/init.d
		sed -e 's/openssl-devel/openssl-dev\n%ifarch x86_pentium3 x86_pentium4\nAutoReqProv: No\n%endif/' -e 's/%{_mandir}/\/usr\/share\/man/' -e 's/%{_initrddir}/\/etc\/init.d/' -e 's/%{_sysconfdir}/\/etc/' -e 's/# POST_INSTALL/newcrypto=%{_libdir}\/libcrypto.so.4\nif [ ! -f $newcrypto ]; then\n cp -f %{_libdir}\/libcrypto.so.0 $newcrypto\nfi\n#/' -e 's/make install DEST/make install-strip DEST/' doc/ipmiutil.spec >$tmpspec
		cp -f $tmpspec doc/ipmiutil.spec
	fi
        if test -f "/etc/wrs-release" -o -d "$WIND_HOME"; then
                echo "Detected WindRiver Linux"
                os=windriver
                echo "Be sure to set up the WR build environment first"
		# WR requires extra steps in the spec file.
		grep "%configure_target" doc/ipmiutil.spec >/dev/null 2>&1
		if test $? -ne 0; then
			# WR changes have not been patched in, so make them.
			sed -e 's/Release: 1%{?dist}/Release: 1_WR/' -e 's/%build/%build\n%configure_target\n%define _config_cache config.cache\n/' -e 's/^make$/make CC="$CC" TARGET_CFLAGS="$CFLAGS" TARGET_LDADD="-L%{_host_cross_lib_dir}" LDFLAGS="$LDFLAGS" CXX="$CXX" CXXFLAGS="$CXXFLAGS" LD="$LD" AS="$AS" AR="$AR" CPP="$CPP" NM="$NM" OBJCOPY="$OPBJCOPY" OBJDUMP="$OPBJDUMP" RANLIB="$RANLIB" STRIP="$STRIP"/' -e 's/%install/%install\n%configure_target\n/' doc/ipmiutil.spec >$tmpspec
			cp -f $tmpspec doc/ipmiutil.spec
		fi
		# WRL libcrypto does not have MD2
		MD2_CFLAGS="-DSKIP_MD2"
		init0=/etc/rc.d/init.d
        fi
	if test $isredhat -eq 0 ; then
	   # set default start/stop for init scripts
           sed -i 's/# Default-Start:/# Default-Start: 3 4 5/' $init_scripts
	   sed -i 's/# Default-Stop:/# Default-Stop:  0 1 2 6/' $init_scripts
	fi
        if test -f "$LIB_DIR/libcrypto.so"; then
		strings $LIB_DIR/libcrypto.so | grep EVP_md2 >/dev/null 2>&1
		if test $? -ne 0; then
			echo "No MD2 detected in openssl libcrypto.so"
			MD2_CFLAGS="-DSKIP_MD2"
		fi
	else
		echo "No openssl $LIB_DIR/libcrypto.so detected"
		MD2_CFLAGS="-DSKIP_MD2"
	fi
	OS_CFLAGS="-DLINUX $MD2_CFLAGS $cfwarn $cfhard"
  else
     # usually "x$sysname" = "xFreeBSD", but allow NetBSD
     echo $sysname | grep BSD >/dev/null 2>&1
     if test $? -eq 0; then
	os=bsd
	OS_CFLAGS="-DBSD"
	OS_LFLAGS=""
	OS_DRIVERS="ipmimv.c ipmidir.c"
	drivers="open direct"
     else
        uname -a | grep -i HP-UX >/dev/null 2>&1
        hpux=$?
        if test $hpux -eq 0; then
           echo "Detected HP-UX"
           os=hpux
           MD2_CFLAGS="-DSKIP_MD2"
           OS_CFLAGS="-DHPUX"
           OS_LFLAGS=""
           OS_DRIVERS="ipmimv.c"
           drivers="open"
        elif test "x$sysname" = "xDarwin" ; then
           echo "Detected MacOSX"
           os=macos
           MD2_CFLAGS="-DSKIP_MD2"
           OS_CFLAGS="-DMACOS"
           OS_LFLAGS=""
	   OS_DRIVERS="ipmimv.c ipmidir.c"
	   drivers="open direct"
	   # optionally check for AppleBMC.kext here?
        else 
	   # not Solaris, Linux, BSD, or HP-UX = error
           echo "OS $sysname not yet supported"
           exit 1
        fi
     fi
  fi
fi

dnl determine where the init.d directory is 
if test  "x$init0" = "x" ; then
  if test  "x$os" != "xmacos" ; then
   which rpmbuild >/dev/null 2>&1
   if test  $? -eq 0 ; then
      init1=`rpmbuild --showrc |grep " _initrddir" | cut -d'}' -f2`
      if test  "x$init1" != "x" ; then
         init0=/etc/${init1}
      fi
   fi
  fi
fi
if test  "x$init0" = "x" ; then
   if test  -d ${etcdir}/rc.d/init.d ; then
        init0=/etc/rc.d/init.d;
   else
        init0=/etc/init.d;
   fi
fi
INIT_DIR=${init0}

AC_SUBST(IA64_CFLAGS)
AC_SUBST(LANPLUS_CFLAGS)
AC_SUBST(LANPLUS_LDADD)
AC_SUBST(LANPLUS_LIB)
AC_SUBST(LANPLUS_CRYPTO)
AC_SUBST(LANPLUS_SAM)
AC_SUBST(LD_SAMX)
AC_SUBST(GPL_CFLAGS)
AC_SUBST(OS_CFLAGS)
AC_SUBST(OS_LFLAGS)
AC_SUBST(OS_DRIVERS)
AC_SUBST(PKG_DIR)
AC_SUBST(INIT_DIR)
AC_SUBST(LIB_DIR)
AC_SUBST(INS_LIB)
AC_SUBST(SUBDIR_S)
AC_SUBST(CROSS_CFLAGS)
AC_SUBST(CROSS_LFLAGS)
AC_SUBST(SYSTEMD_DIR)

AC_OUTPUT(Makefile util/Makefile lib/lanplus/Makefile doc/Makefile scripts/Makefile)

if test "x$sysname" = "xSunOS"; then
	sed -e 's/^LDFLAGS = /LDFLAGS = -L\/usr\/sfw\/lib -lxnet -lsocket -lnsl /' -e 's/-DLINUX/-DSOLARIS -I\/usr\/sfw\/include/' util/Makefile >/tmp/make 
	# -e 's/ipmidir.c/ipmibmc.c/' 
	cp -f /tmp/make util/Makefile
fi

AC_MSG_RESULT([])
AC_MSG_RESULT([ipmiutil $VERSION])
AC_MSG_RESULT([  OS      : $os])
AC_MSG_RESULT([  ARCH    : $archm])
AC_MSG_RESULT([  DRIVERS : $drivers lan $drv_lanplus])
AC_MSG_RESULT([])

