// TO DO: ERROR CHECKING in ALL THE FILES
// System Headers
#include <MP.h>
#include "samms_defines.h"

// Audio Headers
#include <Audio.h>
#include "nlms_filter_helpers.h"
#include "audio_mic_helpers.h"

// I2C & Accelerometer Headers
// To Do: Move accelerometer headers to separate files
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// SD Card Headers
#include "sd_card_helpers.h"

// Audio & Filter Variables
audioClassStruct_t * audioStruct = get_audio_class_struct();
filterCapture_t * capture = get_filter_capture_struct();

// SD Card Variables
SDClass * sdCard = get_sd_card_object();
int splThreshold = 0;

void setup() {
  // Open serial for debugging
  // To Do: function to print to serial from anywhere in code base
  Serial.begin(115200);
  while (!Serial);

  // AUDIO & FILTERING
  Serial.println("Init Audio Library");
  int x = init_audio_struct_and_record (audioStruct);

  // Launch SubCore if recording works
  int ret = MP.begin(SUBCORE1);
  if (ret < 0) {
    printf("MP.begin error = %d\n", ret);
  }

  // SDCard initialization
  if (!sdCard->begin()) {
    Serial.println("SD card not mounted. Cannot continue.");
    while(1);
  } else {
    splThreshold = get_integer_spl_value_from_buffer();
    Serial.print("SPL threshold set: "); Serial.print(splThreshold); Serial.print(" dB\n");
  }
}

void loop() {
  err_t err;

  // Read frames to the recorder
  err = read_frames_to_recorder(audioStruct);

  // Update the capture filter
  if ((audioStruct->read_size != 0) && (audioStruct->read_size == BUFFER_SIZE)) {
    update_capture_filter (capture, audioStruct);
    MP.Send(AUDIO_MP_SEND_ID, capture, SUBCORE1); // Add error checking
    // Serial.println("Sending Data to Subcore1"); // DEBUG
  } else {
    usleep(1);
  }

}
