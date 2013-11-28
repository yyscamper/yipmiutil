/*
 * oem_hp.c
 * Handle HP OEM command functions 
 *
 * Change history:
 *  02/23/2012 ARCress - included in source tree
 *
 *---------------------------------------------------------------------
 */
/*M*
Copyright (c) 2012 Kontron America, Inc.
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
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "ipmicmd.h"
#include "ievents.h"
#include "isensor.h"

static char *redund_str(uchar b)
{
   char *pstr;
   if (b == 0x00) pstr = "Disabled"; 
   else if (b == 0x01) pstr = "Fully Redundant";
   else if (b == 0x02) pstr = "Redundancy Lost";
   else if (b == 0x0b) pstr = "AC Lost";
   else pstr = "Redundancy Degraded"; 
   return(pstr);
}

/*
 * decode_sensor_hp
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
int decode_sensor_hp(uchar *sdr,uchar *reading,char *pstring, int slen)
{
   int rv = -1;
   char *pstr = NULL;
   uchar stype, evtype, b;
   ushort v;

   if (sdr == NULL || reading == NULL) return(rv);
   if (pstring == NULL || slen == 0) return(rv);
   /* sdr[3] is SDR type 1=full, 2=compact */
   stype = sdr[12];  /*sensor type*/
   evtype = sdr[13];  /*event type */
   if (stype == 0xC0) {  /* HP OEM Sensor, no sensor reading */
      strncpy(pstring,"na",slen);    /*oem*/
      rv = 0;
   } else {
      if ((reading[2] & 0x40) == 0x40) {  /*Init/Unavail state*/
	   strncpy(pstring,"Init",slen);
	   rv = 0;
      } else if ((sdr[20] & 0xC0) == 0xC0) { /*unit1==discrete*/
	   b = reading[2] & 0x3f;
	   pstr = "DiscreteUnit";
           if (evtype == 0x0b) pstr = redund_str(b);
	   snprintf(pstring,slen,"%02x%02x %s",reading[3],reading[2],pstr); 
	   rv = 0;
      } else if (evtype == 0x6f) { /*evtype==0x6f special*/
	   pstr = "DiscreteEvt";
           if (stype == 0x08) {  /*Power Supply presence*/
              if (reading[2] & 0x01) pstr = "Present"; 
	      else pstr = "Absent";
	   }
	   snprintf(pstring,slen,"%02x%02x %s",reading[3],reading[2],pstr); 
	   rv = 0;
      } else if (evtype == 0x0B) {  /*Redundancy*/
	   b = reading[2] & 0x3f;
	   pstr = "DiscretePS";  /*Power Supplies*/
           if (evtype == 0x0b) pstr = redund_str(b);
	   snprintf(pstring,slen,"%02x%02x %s",reading[3],reading[2],pstr); 
	   rv = 0;
      } else if (evtype == 0x0A) {  /*Discrete Fan*/
	   v = reading[2] + (reading[3] & 0x3f);
	   if (v & 0x001)      pstr = "Transition to Running"; 
	   else if (v & 0x002) pstr = "Transition to In Test"; 
	   else if (v & 0x004) pstr = "Transition to Power Off"; 
	   else if (v & 0x008) pstr = "Transition to On Line"; 
	   else if (v & 0x010) pstr = "Transition to Off Line"; 
	   else if (v & 0x020) pstr = "Transition to Off Duty"; 
	   else if (v & 0x040) pstr = "Transition to Degraded"; 
	   else if (v & 0x080) pstr = "Transition to Power"; 
	   else if (v & 0x100) pstr = "Install Error"; 
	   else pstr = "Unknown"; 
	   snprintf(pstring,slen,"%02x%02x %s",reading[3],reading[2],pstr); 
	   rv = 0;
      } else if (evtype == 0x09) {  /* stype==0x03 Power Meter */
	   b = reading[2] & 0x3f;
	   if (b & 0x01) pstr = "Disabled";
	   else if (b & 0x02) pstr = "Enabled";
	   else pstr = "Unknown";
	   snprintf(pstring,slen,"%02x%02x %s",reading[3],reading[2],pstr); 
	   rv = 0;
      }
   }
   return(rv);
}

void show_oemsdr_hp(uchar *sdr)
{
   int len, i;

   len = sdr[4] + 5;
   if (sdr[8] == 0x02) len = 18;
   printf("HP: ");
   for (i = 8; i < len; i++)
	printf("%02x ",sdr[i]);
   if (sdr[8] == 0x02) printf("%s",&sdr[18]);
   printf("\n");
   return;
}

/* end oem_hp.c */
