#include <MP.h>

#include "samms_defines.h"

#if (SUBCORE != SUBCORE_ACCEL_PROCESSING)
#error "Core selection is wrong!!"
#endif

#define MP_RECV_ACK 200

// Global MP variables
int ret;
int8_t msgid;
uint32_t msgdata;
bool startMic = false;

void setup() {
  // Open serial for debugging
  Serial.begin(115200);
  while (!Serial);
  
  int ret = MP.begin();
  if (ret < 0) {
   Serial.print("SUBCORE 2: MP.begin error = "); Serial.print(ret); Serial.print("\n");
    while(1);
  } else {
    Serial.println("SUBCORE 2: Subcore ACCEL_PROCESSING launched successfully.");
  }

}

void loop() {
  MP.RecvTimeout(1000); // 1 s timeout
  ret = MP.Recv(&msgid, &msgdata, SUBCORE_ACCEL);

  // TODO: Properly read accelerometer data from SUBCORE_ACCEL
  if (ret < 0) {
    Serial.println("SUBCORE 2: Could not receive accelerometer data");
  } else {
    Serial.print("SUBCORE 2: RECEIVED ACCEL DATA "); Serial.print(msgdata); Serial.print("\n");
  }

  // TODO: Process accelerometer data and set startMic = true if threshold hit
  
  if (startMic) { // Threshold hit
    ret = MP.Send(MP_SEND_ID_ACCEL_PROCESSING, startMic);
    if (ret < 0) {
      Serial.println("SUBCORE 2: Could not send startMic to MAIN CORE");
    } else {
      Serial.println("SUBCORE 2: startMic sent to MAIN CORE");
    }
  }

  // Toggle for testing
  if (!startMic) {
    startMic = true;
  } else {
    startMic = false;
  }

  delay(500);
}
