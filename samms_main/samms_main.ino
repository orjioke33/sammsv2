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
#include <Adafruit_ADXL343.h> // Accelerometer
#include <Adafruit_DRV2605.h> // Haptic driver

// SD Card Headers
#include "sd_card_helpers.h"

// Audio & Filter Variables
audioClassStruct_t * audioStruct = get_audio_class_struct();
filterCapture_t * capture = get_filter_capture_struct();

// SD Card Variables
SDClass * sdCard = get_sd_card_object();
int splThreshold = 0;

// Accelerometer Variables
Adafruit_ADXL343 accel = Adafruit_ADXL343(1);
Adafruit_DRV2605 hapticDriver;

void setup() {
  // Open serial for debugging
  // To Do: function to print to serial from anywhere in code base
  // To Do: print core number with Serial prints
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

  // I2C devices
  // accelerometer, i2c0 bus
  if(!accel.begin()) {
    Serial.println("Ooops, no ADXL343 detected ... Check your wiring!");
    while(1);
  } else {
    Serial.println("ADXL343 began successfully.");
  }

  // haptic driver, i2c0 bus
  if (!hapticDriver.begin()) {
    Serial.println("Oops, no DRV2605L detected ... Check your wiring!");
    while(1);
  } else {
    Serial.println("DRV2605L began successfully.");
    hapticDriver.selectLibrary(1);
    // I2C trigger by sending 'go' command (default)
    //internal trigger when sending GO command
    hapticDriver.setMode(DRV2605_MODE_INTTRIG);
    // set the effect to play
    hapticDriver.setWaveform(0, 48); // Buzz 80%
    hapticDriver.setWaveform(1, 0);  // end waveform
  }

  // SDCard initialization
  if (!sdCard->begin()) {
    Serial.println("SD card not mounted. Cannot continue.");
    while(1);
  } else {
    splThreshold = get_integer_spl_value_from_buffer();
    if (splThreshold >= 0) {
      Serial.print("SPL threshold set: "); Serial.print(splThreshold); Serial.print(" dB\n");
    } else {
      Serial.print("Negative SPL value set!! Please check txt file on SD card.");
    }
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

  hapticDriver.go(); // buzz!
  delay(1000);

}
