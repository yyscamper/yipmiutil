
localedir = $(datadir)/locale
INCLUDES = -I..
OS_CF = @OS_CFLAGS@ @CROSS_CFLAGS@
OS_LF = @OS_LFLAGS@ @CROSS_LFLAGS@
AM_CPPFLAGS = $(OS_CF) -DLOCALEDIR=\"$(localedir)\" 
AM_CPPFLAGS += @IA64_CFLAGS@ @GPL_CFLAGS@
LDADD = $(OS_LF)
LDSAM = $(OS_LF) @LD_SAMX@
CFLAGS_SAMX = -O2 -g -I. -I.. $(OS_CF) @LANPLUS_CFLAGS@
CFLAGS_SAM  = -O2 -g -I. -I.. $(OS_CF) 
# May be /usr/bin/install or /bin/install
INSTALLBIN = install -c 
extradir = $(datadir)/ipmiutil
inc_dir = $(includedir)
tmpobj = obj
tmpwin = tmp
LIBDIR = @LIB_DIR@

CMDMOD = ipmicmd.c mem_if.c ipmidir.c imbapi.c ipmimv.c ipmild.c ipmibmc.c ipmilipmi.c subs.c
CMDMOD += md5.c md2.c ipmilan.c
CMDMOD += ipmilanplus.c
# CMDMOD = ipmicmd.c mem_if.c @OS_DRIVERS@
# am: configure substitutions are not allowed in _SOURCES variables
LIBOBJ = ipmicmd.o mem_if.o ipmidir.o imbapi.o ipmimv.o ipmild.o ipmibmc.o ipmilipmi.o subs.o md5.o md2.o $(tmpobj)/ipmilan.o $(tmpobj)/ipmilanplus.o 
OEMMOD = oem_kontron.c oem_fujitsu.c oem_intel.c oem_sun.c oem_supermicro.c oem_dell.c oem_quanta.c oem_hp.c oem_newisys.c iekanalyzer.c 


AM_CPPFLAGS += -I. -I.. -DMETACOMMAND 
METASOURCE = ipmiutil.c ialarms.c ihealth.c ievents.c ifru.c ifru_picmg.c igetevent.c ireset.c icmd.c ilan.c isensor.c isel.c iserial.c iwdt.c isol.c idiscover.c iconfig.c ipicmg.c ifirewall.c ifwum.c ihpm.c itsol.c idcmi.c $(OEMMOD) $(CMDMOD) 
LDADD += -lpthread 

# The LanDesk library is proprietary, so it is incompatible with ALLOW_GPL.
# To build with LanDesk support:
# First copy the library to ../lib/libipmiapi.a
# Then ./configure --enable-landesk
# Result:
#  AM_CPPFLAGS += -DLINK_LANDESK
#  LDADD += -lipmiapi -L../lib
AM_CPPFLAGS += @LANDESK_CFLAGS@
LDADD += @LANDESK_LDADD@

# For lanplus plugin support (IPMI LAN 2.0 RMCP+) used by SOL:
# If ./configure --disable-lanplus
#  AM_CPPFLAGS += 
#  LDADD +=  
# Otherwise default result is lanplus enabled:
#  AM_CPPFLAGS += -DHAVE_LANPLUS
#  LDADD +=  -L../lib -lipmi_lanplus -L/usr/local/lib -lcrypto 
#  LDADD +=  -L../lib -lintf_lanplus -L/usr/local/lib -lcrypto (older)
# Could also use CMDMOD += ipmilan2.c  instead, if completed.
AM_CPPFLAGS += @LANPLUS_CFLAGS@ 
LDADD += @LANPLUS_LIB@ @LANPLUS_CRYPTO@ 
LANPLUS_OBJ = $(shell ar t @LANPLUS_LIB@ 2>/dev/null) 

# sbin_PROGRAMS are built by default and copied to /usr/sbin at install time
# EXTRA_PROGRAMS are built by default but not included in the install package
# TESTPROGS are not built by default.  To build, do 'make ifruset', for example.
bin_PROGRAMS = ipmiutil ievents idiscover
sbin_PROGRAMS = ipmi_port
DEV_LIB = libipmiutil.a
EXTRA_PROGRAMS = ipmi_sample ipmi_sample_evt
TESTPROGS = libimbapi.a iconfig ipmimv ifruset ipmi_sample2 ialarms_enc iseltime
# OLDPROGS are old/previous binaries that may exist and need to be deleted.
OLDPROGS = alarms bmchealth fruconfig getevent hwreset icmd isolconsole pefconfig sensor showsel tmconfig wdt

# To build an imb api library (libimbapi.a):
libimbapi.a:	imbapi.c
	mkdir -p $(tmpobj)
	$(CC) -I. -I.. $(OS_CF) -fPIC $(CFLAGS) -o $(tmpobj)/imbapi.o -c imbapi.c
	$(AR) cru libimbapi.a $(tmpobj)/imbapi.o
	$(RANLIB) libimbapi.a
	rm -f $(tmpobj)/imbapi.o
 
# Build a lib to support all of the IPMI drivers (libipmiutil.a)
# If LANPLUS_SAM = yes, include LANPLUS_OBJ, else do not.
# See configure --enable-liblanplus to change this
$(DEV_LIB):	$(CMDMOD:.c=.o) @LANPLUS_LIB@
	mkdir -p $(tmpobj)
	if [ "x@LANPLUS_SAM@" = "xyes" ]; then \
	   $(CC) $(CFLAGS_SAMX) -o $(tmpobj)/ipmilanplus.o -c ipmilanplus.c; \
	   $(CC) $(CFLAGS_SAMX) -o $(tmpobj)/ipmilan.o -c ipmilan.c; \
	   ar x @LANPLUS_LIB@ ; \
	   $(AR) cru $(DEV_LIB) $(LIBOBJ) $(LANPLUS_OBJ); \
	   $(RANLIB) $(DEV_LIB); \
	else   \
	   $(CC) $(CFLAGS_SAM) -o $(tmpobj)/ipmilanplus.o -c ipmilanplus.c; \
	   $(CC) $(CFLAGS_SAM) -o $(tmpobj)/ipmilan.o -c ipmilan.c; \
	   $(AR) cru $(DEV_LIB) $(LIBOBJ) ; \
	   $(RANLIB) $(DEV_LIB) ; \
	fi 

# To build ipmiutil, need to use METACFLAGS for each .c/.o

idiscover$(EXEEXT):	idiscover.c
	$(CC) $(CFLAGS_SAM) $(LDFLAGS) $(OS_LF) -o idiscover idiscover.c

ipmimv: 	ipmimv.c
	$(CC) $(CFLAGS_SAM) $(LDFLAGS) -DTEST_BIN -o ipmimv ipmimv.c

ievents$(EXEEXT):         ievents.c
	$(CC) $(CFLAGS_SAM) $(LDFLAGS) -DALONE -o ievents ievents.c 

ipmi_sample:	$(DEV_LIB) ipmi_sample.c isensor.c ievents.c
	$(CC) $(CFLAGS_SAM) -c ipmi_sample.c
	$(CC) -g -O2 -o ipmi_sample ipmi_sample.o $(DEV_LIB) $(LDFLAGS) $(LDSAM)

ialarms_enc:	$(DEV_LIB) ialarms.c oem_intel.c
	$(CC) $(CFLAGS_SAM) -DTEST_ENC -o ialarms2.o -c ialarms.c
	$(CC) $(CFLAGS_SAM) -DNO_EVENTS -o oem_intel2.o -c oem_intel.c
	$(CC) -g -O2 -o ialarms_enc ialarms2.o oem_intel2.o $(DEV_LIB) $(LDFLAGS) $(LDSAM)

ifru_picmg2.o:	ifru_picmg.c
	$(CC) $(CFLAGS_SAM) -o ifru_picmg2.o -c ifru_picmg.c

# To build ipmi_sample with GET_SENSORS enabled, need isensors.o, ievents.o 
# Note that this does not include oem_intel, etc. for OEM SEL decoding.
ipmi_sample2:	$(DEV_LIB) ipmi_sample.c isensor.c ievents.c ifru.c isel.c ifru_picmg2.o
	$(CC) $(CFLAGS_SAM) -D GET_SENSORS -D GET_FRU -o ipmi_sample2.o -c ipmi_sample.c
	$(CC) $(CFLAGS_SAM) -o isensor2.o -c isensor.c 
	$(CC) $(CFLAGS_SAM) -o ievents2.o -c ievents.c 
	$(CC) $(CFLAGS_SAM) -o ifru2.o    -c ifru.c
	$(CC) $(CFLAGS_SAM) -o isel2.o    -c isel.c
	$(CC) -g -O2 -o ipmi_sample2 ipmi_sample2.o isensor2.o ievents2.o ifru2.o ifru_picmg2.o isel2.o $(DEV_LIB) $(LDFLAGS) $(LDSAM)

ipmi_sample_evt:	$(DEV_LIB) ipmi_sample_evt.c isensor.c ievents.c 
	$(CC) $(CFLAGS_SAM) -o ipmi_sample_evt.o -c ipmi_sample_evt.c
	$(CC) $(CFLAGS_SAM) -o isensor2.o -c isensor.c 
	$(CC) $(CFLAGS_SAM) -DSENSORS_OK -o ievents2.o -c ievents.c 
	$(CC) -g -O2 -o ipmi_sample_evt ipmi_sample_evt.o isensor2.o ievents2.o   $(DEV_LIB) $(LDFLAGS) $(LDSAM)

ifruset:	$(DEV_LIB) ifruset.c ifru_picmg2.o
	$(CC) $(CFLAGS_SAM) -c ifruset.c
	$(CC)  -g -O2 -o ifruset ifruset.o ifru_picmg2.o $(DEV_LIB) $(LDFLAGS) $(LDADD)

iseltime:	$(DEV_LIB) iseltime.c 
	$(CC) $(CFLAGS_SAM) -c iseltime.c
	$(CC)  -g -O2 -o iseltime iseltime.o $(DEV_LIB) $(LDFLAGS) $(LDADD)

ipmi_port$(EXEEXT):      ipmi_port.c
	$(CC) $(CFLAGS_SAM) $(LDFLAGS) -o ipmi_port ipmi_port.c 

iconfig:      iconfig.c $(DEV_LIB)
	$(CC) $(CFLAGS_SAM) $(LDFLAGS) -o iconfig iconfig.c $(DEV_LIB) @LANPLUS_CRYPTO@

# @LANPLUS_LIB@ is ../lib/libipmi_lanplus.a
../lib/libipmi_lanplus.a:
	cd ../lib;  make

ipmiutil_SOURCES  = $(METASOURCE) 

ipmiutil$(EXEEXT):	$(METASOURCE:.c=.o) @LANPLUS_LIB@ 
	$(CC) $(CFLAGS) $(LDFLAGS) -o ipmiutil $(METASOURCE:.c=.o) $(LDADD) 

ievents_SOURCES    = ievents.c

idiscover_SOURCES = idiscover.c 

ipmi_port_SOURCES = ipmi_port.c 


EXTRA_DIST = imb_api.h ipmicmd.h ipmidir.h ipmilan.h ipmilanplus.h AnsiTerm.h  ifirewall.h ifwum.h ihpm.h ipicmg.h ipmiutil.h md2.h oem_dell.h oem_fujitsu.h oem_intel.h oem_kontron.h oem_sun.h idcmi.h isensor.h iekanalyzer.h

all-am: Makefile $(bin_PROGRAMS) $(sbin_PROGRAMS) $(EXTRA_PROGRAMS) $(DEV_LIB)

install-data-am:  $(EXTRA_PROGRAMS) $(DEV_LIB)
	@INS_LIB@ mkdir -p $(DESTDIR)$(extradir)
	@INS_LIB@ mkdir -p $(DESTDIR)$(LIBDIR)
	@INS_LIB@ mkdir -p $(DESTDIR)$(inc_dir)
	@INS_LIB@ cp -f ipmi_sample.c $(DESTDIR)$(extradir)
	@INS_LIB@ cp -f ipmi_sample_evt.c $(DESTDIR)$(extradir)
	@INS_LIB@ cp -f ipmicmd.h   $(DESTDIR)$(inc_dir)
	@INS_LIB@ cp -f isensor.c ievents.c $(DESTDIR)$(extradir)
	@INS_LIB@ cp -f isensor.h ievents.h $(DESTDIR)$(extradir)
	@INS_LIB@ cp -f Makefile.sample $(DESTDIR)$(extradir)/Makefile
	@INS_LIB@ cp -f $(DEV_LIB)      $(DESTDIR)$(LIBDIR)

clean-generic:   
	rm -f $(DEV_LIB) $(EXTRA_PROGRAMS) $(OLDPROGS) $(TESTPROGS)
	if [ -d $(tmpobj) ]; then rm -rf $(tmpobj) ;  fi
	if [ -d $(tmpwin) ]; then rm -rf $(tmpwin) ;  fi
	rm -f *.log *.tmp debug*.list *.o *.pdb *.lo *.la *.so
