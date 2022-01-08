#include <stddef.h>
#include <stdlib.h>
#include <SDHCI.h>
#include <File.h>

#define SPL_FILE_NAME       "threshold.txt"

// Variables
SDClass _sdCard;
File _splFile;
char _splFileBuffer[8];

// Private Functions
// Read the spl value from the sd card
// Returns number of bytes read
static int read_spl_value_from_sd_card() {
  int bytesRead = -1;
  
  // Open the file
  _splFile = _sdCard.open(SPL_FILE_NAME);

  if (_splFile) {
    // Read until the file is empty
    while (_splFile.available()) {
      _splFile.read(_splFileBuffer, sizeof(_splFileBuffer) - 1);
    }

    // grab number of bytes read
    bytesRead = strlen(_splFileBuffer);

    // close the file
    _splFile.close();
  } else {
    printf("\nCould not open %s. Please check sd card!\n", SPL_FILE_NAME);
  }

  return bytesRead;
}

// Public Functions
SDClass * get_sd_card_object() {
  return &_sdCard;
}

char * get_spl_file_buffer() {
  return _splFileBuffer;
}

// Convert the value into an integer
int get_integer_spl_value_from_buffer() {
  int spl = -1;
  
  // Only convert the value if there are 0 errors
  if (read_spl_value_from_sd_card() != -1) {
    spl = atoi(_splFileBuffer);
  }

  return spl;
}
