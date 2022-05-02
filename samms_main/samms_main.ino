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
#include <Adafruit_DRV2605.h> // Haptic driver

// GNSS / GPS
#include <GNSS.h>
#include "gnss_helpers.h"

// SD Card Headers
#include "sd_card_helpers.h"

#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

// SD Card Variables
SDClass * sdCard = get_sd_card_object();
int splThreshold = 70;

// I2C Variables
Adafruit_DRV2605 hapticDriver;

// Global MP vars
int ret;
uint32_t rcvdata;
int8_t rcvid;
int micData = 30;

void setup() {
  // Open serial for debugging
  Serial.begin(115200);
  while (!Serial);
  int subid, ret;

  // Start all subscores
  for (subid = 1; subid <= 3; subid++) {
    ret = MP.begin(subid);
    if (ret < 0) {
      MPLog("MP.begin(%d) error = %d\n", subid, ret);
      while(1);
    } else {
      Serial.print("MAIN CORE: Began subcore "); Serial.print(subid); Serial.print(" successfully.\n");
    }
  }

  // Wait HW initialization done. Setup gnss for clock
  sleep(3);
  setup_gnss();

  // haptic driver, i2c0 bus
  if (!hapticDriver.begin()) {
    Serial.println("MAIN CORE: Oops, no DRV2605L detected ... Check your wiring!");
    // while(1);
  } else {
    Serial.println("MAIN CORE: DRV2605L began successfully.");
    hapticDriver.selectLibrary(1);
    hapticDriver.setMode(DRV2605_MODE_INTTRIG); //internal trigger when sending GO command
    hapticDriver.setWaveform(0, 48); // Buzz 80%
    hapticDriver.setWaveform(1, 0);  // end waveform
  }

  // SDCard initialization
  if (!sdCard->begin()) {
    Serial.println("MAIN CORE: SD card not mounted. Cannot continue.");
     while(1);
  } 
  else {
    splThreshold = get_integer_spl_value_from_buffer();
    Serial.print("SD Card Contents: \""); Serial.print(get_spl_file_buffer()); Serial.print("\"\n");
    if (splThreshold >= 0) {
      Serial.print("SPL threshold set: "); Serial.print(splThreshold); Serial.print(" dB\n");
    } else {
      Serial.print("Negative SPL value set!! Please check txt file on SD card.");
    }
  }

  // Send SP threshold to audio processing core
  Serial.print("MAIN CORE: Send splThreshold: "); Serial.print(splThreshold); Serial.print("\n");

  ret = MP.Send(MP_SEND_ID_MAIN_CORE, splThreshold, SUBCORE_AUDIO_PROCESSING); // sendid, data, subcore
  if (ret < 0) {
    printf("MAIN CORE: MP.Send error splThreshold to SUBCORE_AUDIO_PROCESSING = %d\n", ret);
  }

  // Wait 1 second to receive ack
  // MP.RecvTimeout(1000);
}

void loop() {

  // Check if we can start the mics
  MP.RecvTimeout(1000); // 1 second timeout
  ret = MP.Recv(&rcvid, &rcvdata, SUBCORE_ACCEL_PROCESSING);
  if (ret < 0) {
    Serial.print("MAIN CORE: MP.Recv error = "); Serial.print(ret); Serial.print("\n");
  } else {
    Serial.print("MAIN CORE: Received start mic flag: "); Serial.print(rcvdata); Serial.print("\n");
  }

  // TODO: Turn on microphones
  if (rcvdata == 1 && rcvid == MP_SEND_ID_ACCEL_PROCESSING) { // Microphones can start
    // TODO: Collect microphone data
    // TODO: Send mic data to SUBCORE_AUDIO_PROCESSING
    ret = MP.Send(MP_SEND_ID_MAIN_CORE, micData, SUBCORE_AUDIO_PROCESSING); // sendid, data, subcore
    if (ret < 0) {
      printf("MAIN CORE: MP.Send error micData to SUBCORE_AUDIO_PROCESSING = %d\n", ret);
    }
  }

  ret = MP.Recv(&rcvid, &rcvdata, SUBCORE_AUDIO_PROCESSING);
  if (ret < 0) {
    Serial.print("MAIN CORE: MP.Recv error = "); Serial.print(ret); Serial.print("\n");
  } else {
    Serial.print("MAIN CORE: spl value: "); Serial.print(rcvdata); Serial.print("\n");
    if (rcvdata < 50) {
      Serial.println("MAIN CORE: BUZZ HAPTIC FEED BACK. UNDER 50 db!!!");
    }
  }

  // TODO: Only update gnss once we get a valid time stamp
  // And then update it once 2 hours have elapsed. Just need to
  // update it every 2 hours for a valid time stamp
  update_gnss();
  restart_gnss_on_timeout();

  delay(500);
}
