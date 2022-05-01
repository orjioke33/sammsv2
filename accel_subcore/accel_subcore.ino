#include <Wire.h>
#include <MP.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h> // Accelerometer

#include "samms_defines.h"

#if (SUBCORE != SUBCORE_ACCEL)
#error "Core selection is wrong!!"
#endif

Adafruit_ADXL343 accel = Adafruit_ADXL343(1);

int ret;
int8_t msgid;
uint32_t msgdata;
int16_t accelData = 22;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  int ret = MP.begin();
  if (ret < 0) {
   Serial.print("SUBCORE 1: MP.begin error = "); Serial.print(ret); Serial.print("\n");
    while(1);
  } else {
    Serial.println("SUBCORE 1: Subcore ACCEL launched successfully.");
  }
  
  // I2C devices
  // accelerometer, i2c0 bus
  if(!accel.begin()) {
    Serial.println("SUBCORE 1: Ooops, no ADXL343 detected ... Check your wiring!");
    // while(1);
  } else {
    Serial.println("SUBCORE 1: ADXL343 began successfully.");
  }

}

void loop() {
  //TODO: Read in accel data

  // Send accel data for processing
  Serial.println("SUBCORE 1: Sending accel data to SUBCORE 2.");

  // We can either send the data value by value or if the subcores
  // have shared memory, we can copy the array to a new address and
  // send the base address of the copy once the original is filled
  ret = MP.Send(MP_SEND_ID_ACCEL, accelData, SUBCORE_ACCEL_PROCESSING); // sendid, data, subcore
  if (ret < 0) {
    Serial.print("SUBCORE 1: MP.Send error = "); Serial.print(ret); Serial.print("\n");
  }

  delay(500);
}
