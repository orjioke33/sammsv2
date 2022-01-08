#ifndef _SD_CARD_HELPERS_H    // Put these two lines at the top of your file.
#define _SD_CARD_HELPERS_H    // (Use a suitable name, usually based on the file name.)
#include <SDHCI.h>

SDClass * get_sd_card_object();
char * get_spl_file_buffer();
int get_integer_spl_value_from_buffer();

#endif // _SD_CARD_HELPERS_H    // Put this line at the end of your file.
