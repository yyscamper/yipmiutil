/*
 * oem_supermicro.c
 * Handle SuperMicro OEM command functions 
 *
 * Change history:
 *  12/06/2010 ARCress - included in source tree
 *
 *---------------------------------------------------------------------
 */
/*M*
Copyright (c) 2010 Kontron America, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

  a.. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer. 
  b.. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation 
      and/or other materials provided with the distribution. 
  c.. Neither the name of Kontron nor the names of its contributors 
      may be used to endorse or promote products derived from this software 
      without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *M*/
#ifdef WIN32
#include <windows.h>
#include "getopt.h"
#else
#if defined(HPUX)
/* getopt is defined in stdio.h */
#elif defined(MACOS)
/* getopt is defined in unistd.h */
#include <unistd.h>
#include <sys/time.h>
#else
#include <getopt.h>
#endif
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ipmicmd.h"
#include "ievents.h"
#include "oem_supermicro.h"

#ifdef MOVED  /*moved to oem_supermicro.h*/
#define SUPER_NETFN_OEM           0x30
#define SUPER_CMD_BMCSTATUS       0x70
#define SUPER_CMD_RESET_INTRUSION 0x03
#define SUPER_NETFN_OEMFW     0x3C  
#define SUPER_CMD_OEMFWINFO   0x20
#endif 

void set_loglevel(int level);  /*prototype */
extern char fsm_debug;  /*mem_if.c*/

static char * progver   = "2.92";
static char * progname  = "ismcoem";
static int verbose = 0;
static char fdebug = 0;
static uchar  g_bus  = PUBLIC_BUS;
static uchar  g_sa   = BMC_SA;
static uchar  g_lun  = BMC_LUN;
static uchar  g_addrtype = ADDR_SMI;
static int vend_id = 0;
static int prod_id = 0;

int oem_supermicro_get_bmc_status(uchar *sts)
{
   int rv;
   int rlen, ilen;
   uchar rdata[16];
   uchar idata[16];
   uchar cc;

   if (sts == NULL) return(LAN_ERR_INVPARAM);
   if (vend_id == VENDOR_SUPERMICROX) { 
      /* subfunc 0xF0 is invalid for newer SMC systems */
      idata[0] = 0x02;  /* action: get status */
      ilen = 1;
   } else {
      idata[0] = 0xF0;  /* subfunction */
      idata[1] = 0x02;  /* action: get status */
      // idata[2] = 0;
      ilen = 2;
   }
   rlen = sizeof(rdata);
   rv = ipmi_cmdraw(SUPER_CMD_BMCSTATUS, SUPER_NETFN_OEM, 
		    BMC_SA, PUBLIC_BUS, BMC_LUN,
		    idata, ilen, rdata, &rlen, &cc, fdebug);
   if ((rv == 0) && (cc != 0)) rv = cc;
   if (rv == 0) { *sts = rdata[0]; }
   return(rv);
}

int oem_supermicro_set_bmc_status(uchar sts)
{
   int rv;
   int rlen, ilen;
   uchar rdata[16];
   uchar idata[16];
   uchar cc;

   if (sts > 1) sts = 1;  /* actions: 0=disable, 1=enable, 2=status*/ 
   if (vend_id == VENDOR_SUPERMICROX) {
      idata[0] = sts;
      ilen = 1;
   } else {
      idata[0] = 0xF0;  /* subfunction */
      idata[1] = sts;
      ilen = 2;
   }
   rlen = sizeof(rdata);
   rv = ipmi_cmdraw(SUPER_CMD_BMCSTATUS, SUPER_NETFN_OEM, 
		    BMC_SA, PUBLIC_BUS, BMC_LUN,
		    idata, ilen, rdata, &rlen, &cc, fdebug);
   if ((rv == 0) && (cc != 0)) rv = cc;
   return(rv);
}

int oem_supermicro_get_lan_port(uchar *val)
{
   int rv;
   int rlen, ilen;
   uchar rdata[16];
   uchar idata[16];
   uchar cc;

   idata[0] = 0x0c;  /* subfunction */
   idata[1] = 0x00;  /* get */
   ilen = 2;
   rlen = sizeof(rdata);
   rv = ipmi_cmdraw(SUPER_CMD_BMCSTATUS, SUPER_NETFN_OEM, 
		    BMC_SA, PUBLIC_BUS, BMC_LUN,
		    idata, ilen, rdata, &rlen, &cc, fdebug);
   if ((rv == 0) && (cc != 0)) rv = cc;
   if (rv == 0) { *val = rdata[0]; }
   return(rv);
}

int oem_supermicro_set_lan_port(uchar val)
{
   int rv;
   int rlen, ilen;
   uchar rdata[16];
   uchar idata[16];
   uchar cc;

   idata[0] = 0x0c;  /* subfunction */
   idata[1] = 0x01;  /* set */
   idata[2] = val;
   ilen = 3;
   rlen = sizeof(rdata);
   rv = ipmi_cmdraw(SUPER_CMD_BMCSTATUS, SUPER_NETFN_OEM, 
		    BMC_SA, PUBLIC_BUS, BMC_LUN,
		    idata, ilen, rdata, &rlen, &cc, fdebug);
   if ((rv == 0) && (cc != 0)) rv = cc;
   return(rv);
}

char *oem_supermicro_lan_port_string(uchar val)
{
	char *p;
	switch(val) {
	case 0: p = "Dedicated"; break;
	case 1: p = "Onboard_LAN1"; break;
	case 2: p = "Failover"; break;
	default: p = "unknown"; break;
	}
	return(p);
}

static void oem_supermicro_show_lan_port(uchar val)
{
	printf("Current LAN interface is %s\n",
		oem_supermicro_lan_port_string(val));
}

int oem_supermicro_get_health(char *pstr, int sz) 
{
   int rv;
   uchar bsts;
   char *str;

   rv = oem_supermicro_get_bmc_status(&bsts);
   if (rv == 0) {
	if (bsts == 0x01) str = "BMC status = enabled";
	else str = "BMC status = disabled";
	strncpy(pstr, str, sz);
   }
   return(rv);
}

/* 
 * oem_supermicro_get_firmware_info
 *
 * From post by ipmitool developer.
 * http://sourceforge.net/mailarchive/message.php?msg_name=49ABCCC3.4040004%40cern.ch
 *
 * Request
 * 0x3C - OEM network function
 * 0x20 - OEM cmd  (SUPER_CMD_OEMFWINFO)
 *
 * Response data:
 * 4 bytes - firmware major version (LSB first)
 * 4 bytes - firmware minor version (LSB first)
 * 4 bytes - firmware sub version (LSB first)
 * 4 bytes - firmware build number (LSB first)
 * 1 byte - hardware ID
 * ? bytes - firmware tag, null terminated string
 */
int oem_supermicro_get_firmware_info(uchar *info)
{
   int rv;
   int rlen;
   uchar rdata[32];
   uchar idata[16];
   uchar cc;

   if (info == NULL) return(LAN_ERR_INVPARAM);
   rlen = sizeof(rdata);
   rv = ipmi_cmdraw(SUPER_CMD_OEMFWINFO, SUPER_NETFN_OEM, 
		    BMC_SA, PUBLIC_BUS, BMC_LUN,
		    idata, 0, rdata, &rlen, &cc, fdebug);
   if ((rv == 0) && (cc != 0)) rv = cc;
   if (rv == 0) { memcpy(info,rdata,rlen); }
   return(rv);
}

int oem_supermicro_get_firmware_str(char *pstr, int sz)
{
   int rv;
   uchar info[32];
   uint32 fwmaj;
   uint32 fwmin;
   uint32 fwsub;
   uint32 fwbld;
   uchar hwid;
   rv = oem_supermicro_get_firmware_info(info);
   if (rv == 0) {
        fwmaj = info[0] + (info[1] << 8) + (info[2] << 16) + (info[3] << 24);
        fwmin = info[4] + (info[5] << 8) + (info[6] << 16) + (info[7] << 24);
        fwsub = info[8] + (info[9] << 8) + (info[10] << 16) + (info[11] << 24);
        fwbld = info[12] +(info[13] << 8) + (info[14] << 16) + (info[15] << 24);
	hwid = info[16];
        /*info[17] = fw tag string */
        snprintf(pstr,sz,"Firmware %d.%d.%d.%d HW %d %s\n",fwmaj,fwmin,fwsub,
			fwbld,hwid,&info[17]);
   }
   return(rv);
}


int oem_supermicro_reset_intrusion(void)
{
   int rv;
   int rlen;
   uchar rdata[32];
   uchar idata[16];
   uchar cc;
   // if (state == NULL) return(LAN_ERR_INVPARAM);
   rlen = sizeof(rdata);
   rv = ipmi_cmdraw(SUPER_CMD_RESET_INTRUSION, SUPER_NETFN_OEM, 
		    BMC_SA, PUBLIC_BUS, BMC_LUN,
		    idata, 0, rdata, &rlen, &cc, fdebug);
   if ((rv == 0) && (cc != 0)) rv = cc;
   if (rv == 0) { /* check rdata for more info */ }
   return(rv);
}

/*
 * decode_sensor_supermicro
 * inputs:
 *    sdr     = the SDR buffer
 *    reading = the 3 or 4 bytes of data from GetSensorReading
 *    pstring = points to the output string buffer
 *    slen    = size of the output buffer
 * outputs:
 *    rv     = 0 if this sensor was successfully interpreted here,
 *             non-zero otherwise, to use default interpretations.
 *    pstring = contains the sensor reading interpretation string (if rv==0)
 */
int decode_sensor_supermicro(uchar *sdr,uchar *reading,char *pstring, int slen)
{
   int rv = -1;
   uchar stype;
   uchar bval;
   char *pstr = NULL;

   if (sdr == NULL || reading == NULL) return(rv);
   if (pstring == NULL || slen == 0) return(rv);
   /* sdr[3] is 0x01 for Full, 0x02 for Compact */
   bval = reading[2];
   stype = sdr[12];
   switch(stype) {
        case 0xC0:      /* CPU Temp Sensor, EvTyp=0x70 (Full) */
	   //if (dbg) printf("supermicro %x sensor reading %x\n",stype,reading);
           rv = 0;
	   switch(bval) {
           case 0x0000: pstr = "00 Low";    break;
           case 0x0001: pstr = "01 Medium"; break;
           case 0x0002: pstr = "02 High";   break;
           case 0x0004: pstr = "04 Overheat";   break;
           case 0x0007: pstr = "07 Not Installed";   break;
	   default:  rv = -1;  break;
	   }
           break;
        case 0x08:  /* Power Supply Status (Full/Discrete) Table 42-3 */
	   rv = 0;
	   switch(bval) {
           case 0x00: pstr = "00 Absent"; break;     /*bit 0*/
           case 0x01: pstr = "01 Present"; break;    /*bit 0*/
           case 0x02: pstr = "02 Failure"; break;    /*bit 1*/
           case 0x04: pstr = "04 Predict Fail"; break;  /*bit 2*/
           case 0x08: pstr = "08 Input Lost"; break;    /*bit 3*/
	   default:  rv = -1;  break;
	   }
           break;
        default:
           break;
   }
   if (rv == 0) strncpy(pstring, pstr, slen);
   return(rv);
}

int decode_mem_supermicro(int prod, uchar b2, uchar b3, char *desc, int *psz)
{
   int array, dimm, n;
   int rv = -1;
   uchar bdata;
   if ((desc == NULL) || (psz == NULL)) return -1;
   if (b2 == 0xff) bdata = b3;  /*ff is reserved*/
   else  bdata = b2; /* normal case */
   array = (bdata & 0xc0) >> 6;
   dimm = bdata & 0x3f;
   /* bdata = 0x10 (16.) means CPU 1, DIMM 6 */
   array = bdata / 10;
   dimm  = bdata % 10;

#ifdef DMIOK
   /* Use DMI if we get confirmation about array/dimm indices. */
   if (! is_remote()) {
      fsm_debug = fdebug;
      rv = get_MemDesc(array,dimm,desc,psz);
      /* if (rv != 0) desc has "DIMM[%d}" */ 
   } 
#endif

   if (rv != 0) {
      n = sprintf(desc,"DIMM%d/CPU%d",dimm,array);
      *psz = n;
      rv = 0;
   }
   return(rv);
} /*end decode_mem_supermicro*/

/*
 * decode_sel_supermicro
 * inputs:
 *    evt    = the 16-byte IPMI SEL event
 *    outbuf = points to the output string buffer
 *    outsz  = size of the output buffer
 * outputs:
 *    rv     = 0 if this event was successfully interpreted here,
 *             non-zero otherwise, to use default interpretations.
 *    outbuf = will contain the interpreted event text string (if rv==0)
 */
int decode_sel_supermicro(uchar *evt, char *outbuf, int outsz, char fdesc,
                        char fdbg)
{
   int rv = -1;
   ushort id;
   uchar rectype;
   ulong timestamp;
   char mybuf[64]; 
   char *type_str = "";
   char *pstr = NULL;
   int sevid;
   ushort genid;
   uchar snum;
         
   fdebug = fdbg;
   sevid = SEV_INFO;
   id = evt[0] + (evt[1] << 8);
   rectype = evt[2];
   snum = evt[11];
   timestamp = evt[3] + (evt[4] << 8) + (evt[5] << 16) + (evt[6] << 24);
   genid = evt[7] | (evt[8] << 8);
   if (rectype == 0x02) 
   {
     sprintf(mybuf,"%02x [%02x %02x %02x]", evt[12],evt[13],evt[14],evt[15]);
     switch(evt[10]) {  /*sensor type*/
        case 0xC0:      /* CPU Temp Sensor */
	   type_str = "OEM_CpuTemp";
	   switch((evt[13] &0x0f)) {  /*offset/data1 l.o. nibble*/
           case 0x02:   /* CPU Temp Sensor Overheat event offset */
		if (evt[12] & 0x80) {  /*EvTyp==0xF0 if deassert*/
		   pstr = "CpuTemp Overheat OK"; sevid = SEV_INFO; 
		} else {  /* EvTyp=0x70 assert */
		   pstr = "CpuTemp Overheat   "; sevid = SEV_MAJ; 
		}
		rv = 0;
		break;
	   default: pstr = "CpuTemp Event"; break;
	   }
	   break;
        case 0xC2:      /* CPLD Event */
	   type_str = "OEM_CPLD";
	   switch((evt[13] & 0x0f)) {  /* data1 usu 0xa0*/
	   case 0x00:
		if (evt[14] == 0x1c) 
		     { pstr = "CPLD CATERR Asserted"; sevid = SEV_CRIT; }
		else { pstr = "CPLD Event Asserted";  sevid = SEV_MIN;  }
		rv = 0;
		break;
	   default: pstr = "CPLD Event"; break;
	   }
	   break;
	default: /*other sensor types*/
	   break;
     }
   }
   if (rv == 0) {
	 format_event(id,timestamp, sevid, genid, type_str,
			snum,NULL,pstr,mybuf,outbuf,outsz);
   }
   return(rv);
}

static void usage(void)
{
   printf("Usage: %s <command> [arg]\n",progname);
   printf("   intrusion                   = reset chassis intrusion\n");
   printf("   bmcstatus [enable| disable] = get/set BMC status\n");
   printf("   firmware                    = get extra firmware info\n");
   printf("   lanport  [dedicated| lan1| failover] = get/set IPMI LAN port\n");
   printf("These commands may not work on all SuperMicro systems\n");
}

static int ipmi_smcoem_main(int  argc, char **argv)
{
   int rv = 0;
   char msg[80];
   uchar val;

   if (strncmp(argv[0],"intrusion",9) == 0) {
      printf("Clearing Chassis Intrusion ...\n");
      rv = oem_supermicro_reset_intrusion();
   } else if (strncmp(argv[0],"bmcstatus",9) == 0) {
      printf("Getting BMC status ...\n");
      rv = oem_supermicro_get_health(msg, sizeof(msg));
      if (rv != 0) return(rv);
      printf("%s\n",msg);
      if (argv[1] != NULL) {
         if (strncmp(argv[1],"disable",7) == 0) {
	    val = 0;
         } else if (strncmp(argv[1],"enable",6) == 0) {
	    val = 1;
	 } else {
	    usage();
	    return(ERR_USAGE);
	 }
         printf("Setting BMC status to %s ...\n",argv[1]);
         rv = oem_supermicro_set_bmc_status(val);
         if (rv != 0) return(rv);
         rv = oem_supermicro_get_health(msg, sizeof(msg));
         if (rv == 0) printf("%s\n",msg);
      }
   } else if (strncmp(argv[0],"firmware",8) == 0) {
      printf("Getting SMC Firmare Information ...\n");
      rv = oem_supermicro_get_firmware_str(msg, sizeof(msg));
      if (rv == 0) printf("%s\n",msg);
   } else if (strncmp(argv[0],"lanport",9) == 0) {
      rv = oem_supermicro_get_lan_port(&val);
      if (rv == 0) {
        oem_supermicro_show_lan_port(val);
        if (argv[1] != NULL) {
           if (strncmp(argv[1],"dedicated",9) == 0) {
	      val = 0;
           } else if (strncmp(argv[1],"lan1",4) == 0) {
	      val = 1;
           } else if (strncmp(argv[1],"failover",8) == 0) {
	      val = 2;
	   } else {
	      usage();
	      return(ERR_USAGE);
	   }
           printf("Setting LAN interface to %s ...\n",argv[1]);
	   rv = oem_supermicro_set_lan_port(val);
           if (rv != 0) return(rv);
           rv = oem_supermicro_get_lan_port(&val);
	   if (rv == 0) oem_supermicro_show_lan_port(val);
        }
      }
   } else {
      usage();
      rv = ERR_USAGE;
   }
   return(rv);
}

#ifdef METACOMMAND
int i_smcoem(int argc, char **argv)
#else
#ifdef WIN32
int __cdecl
#else
int
#endif
main(int argc, char **argv)
#endif
{
   int rv = 0;
   uchar devrec[16];
   int c, i;
   char *s1;

   printf("%s ver %s\n", progname,progver);
   set_loglevel(LOG_NOTICE);
   parse_lan_options('V',"4",0);  /*default to admin priv*/

   while ( (c = getopt( argc, argv,"m:xzEF:J:N:P:R:T:U:V:YZ:?")) != EOF )
      switch(c) {
          case 'm': /* specific IPMB MC, 3-byte address, e.g. "409600" */
                    g_bus = htoi(&optarg[0]);  /*bus/channel*/
                    g_sa  = htoi(&optarg[2]);  /*device slave address*/
                    g_lun = htoi(&optarg[4]);  /*LUN*/
                    g_addrtype = ADDR_IPMB;
                    if (optarg[6] == 's') {
                             g_addrtype = ADDR_SMI;  s1 = "SMI";
                    } else { g_addrtype = ADDR_IPMB; s1 = "IPMB"; }
                    ipmi_set_mc(g_bus,g_sa,g_lun,g_addrtype);
                    printf("Use MC at %s bus=%x sa=%x lun=%x\n",
                            s1,g_bus,g_sa,g_lun);
                    break;
          case 'x': fdebug = 2; /* normal (dbglog if isol) */
                    verbose = 1;
                    break;
          case 'z': fdebug = 3; /*full debug (for isol)*/
                    verbose = 1;
                    break;
          case 'N':    /* nodename */
          case 'U':    /* remote username */
          case 'P':    /* remote password */
          case 'R':    /* remote password */
          case 'E':    /* get password from IPMI_PASSWORD environment var */
          case 'F':    /* force driver type */
          case 'T':    /* auth type */
          case 'J':    /* cipher suite */
          case 'V':    /* priv level */
          case 'Y':    /* prompt for remote password */
          case 'Z':    /* set local MC address */
                parse_lan_options(c,optarg,fdebug);
                break;
          default:
		usage();
		return(ERR_USAGE);
                break;
      }
   for (i = 0; i < optind; i++) { argv++; argc--; }
   if (argc == 0) {
	usage();
	return(ERR_USAGE);
   }

   rv = ipmi_getdeviceid(devrec,16,fdebug);
   if (rv == 0) {
      char ipmi_maj, ipmi_min;
      ipmi_maj = devrec[4] & 0x0f;
      ipmi_min = devrec[4] >> 4;
      vend_id = devrec[6] + (devrec[7] << 8) + (devrec[8] << 16);
      prod_id = devrec[9] + (devrec[10] << 8);
      show_devid( devrec[2],  devrec[3], ipmi_maj, ipmi_min);
   }

   rv = ipmi_smcoem_main(argc, argv);

   ipmi_close_();
   return(rv);
}
/* end oem_supermicro.c */
