// TO DO: ERROR CHECKING in ALL THE FILES
// System Headers
#include <MP.h>
#include "samms_defines.h"

// Audio Headers
#include <Audio.h>
#include "nlms_filter_helpers.h"
#include "audio_mic_helpers.h"

// I2C & Accelerometer Headers
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// Audio & Filter Variables
audioClassStruct_t * audioStruct = get_audio_class_struct();
filterCapture_t * capture = get_filter_capture_struct();

// Accelerometer Variables
Adafruit_ADXL343 accel = Adafruit_ADXL343(1);

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
    while(1);
  } else {
    Serial.println("Subcore launched successfully.");
  }

  // ACCELEROMETER
  // Initialize the accelerometer
  if(!accel.begin())
  {
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  } else {
    Serial.println("ADXL343 began successfully.");
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
