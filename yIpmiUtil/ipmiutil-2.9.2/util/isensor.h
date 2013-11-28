/*
 * isensor.h
 * common routines from isensor.c 
 */
#define SDR_SZ    80     /*max SDR size*/

int  get_sdr_cache(uchar **pcache);  
void free_sdr_cache(uchar *pcache);
int  get_sdr_file(char *sdrfile, uchar **sdrlist);
int find_nsdrs(uchar *pcache);
int find_sdr_next(uchar *psdr, uchar *pcache, ushort id);
int find_sdr_by_snum(uchar *psdr, uchar *pcache, uchar snum, uchar sa);
int find_sdr_by_tag(uchar *psdr, uchar *pcache, char *tag, uchar dbg);
int find_sdr_by_id(uchar *psdr, uchar *pcache, ushort id);

void ShowSDR(char *tag, uchar *sdr);
int GetSDRRepositoryInfo(int *nret, int *fdev);
int GetSensorThresholds(uchar sens_num, uchar *data);
int GetSensorReading(uchar sens_num, void *psdr, uchar *sens_data);
double RawToFloat(uchar raw, uchar *psdr);
char *decode_entity_id(int id);
char *get_unit_type(int iunits, int ibase, int imod, int fshort);

/* 
 * decode_comp_reading
 * 
 * Decodes the readings from compact SDR sensors.
 * Use sensor_dstatus array for sensor reading types and meaning strings.
 * Refer to IPMI Table 36-1 and 36-2 for this.
 * Note that decoding should be based on sensor type and ev_type only,
 * except for end cases.
 * 
 * reading1 = sens_reading[2], reading2 = sens_reading[3]
 */
int decode_comp_reading(uchar type, uchar evtype, uchar num, 
		    uchar reading1, uchar reading2);

/* end isensor.h */
