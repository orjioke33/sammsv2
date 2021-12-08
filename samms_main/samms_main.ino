// TO DO: ERROR CHECKING in ALL THE FILES
// System Headers
#include <MP.h>
#include "samms_defines.h"

// Audio Headers
#include <Audio.h>
#include "nlms_filter_helpers.h"
#include "audio_mic_helpers.h"

// Accelerometer Headers
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>

// Filter Variables
audioClassStruct_t * audioStruct = get_audio_class_struct();

void setup() {
  // Filter Setup - TO DO Cleanup
  int ret;

  // Open serial for debugging
  Serial.begin(115200);
  while (!Serial);

  // AUDIO & FILTERING
  Serial.println("Init Audio Library");
  int x = init_audio_struct_and_record (audioStruct);

  // Launch SubCore if recording works
  ret = MP.begin(SUBCORE1);
  if (ret < 0) {
    printf("MP.begin error = %d\n", ret);
  }
}

void loop() {
  // Filter - TO Do Cleanup
  int8_t sndid = 100; /* user-defined msgid */
  int8_t rcvid = 8;
  filterCapture_t * capture = get_filter_capture_data();

  static const int32_t buffer_sample = 768 * AS_CHANNEL_STEREO;
  static const int32_t buffer_size = buffer_sample * sizeof(int16_t);
  static char  buffer[buffer_size];
  uint32_t read_size;

  /* Read frames to record in buffer */
  int err = audioStruct->audio->readFrames(buffer, buffer_size, &read_size);

  if (err != AUDIOLIB_ECODE_OK && err != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA) {
    printf("Error err = %d\n", err);
    sleep(1);
    audioStruct->audio->stopRecorder();
    exit(1);
  }

  if ((read_size != 0) && (read_size == buffer_size)) {
    capture->buff   = buffer;
    capture->sample = buffer_sample / AS_CHANNEL_STEREO;
    capture->chnum  = AS_CHANNEL_STEREO;
    MP.Send(sndid, &capture, SUBCORE1);
    Serial.println("Sending Data to Subcore1");
  } else {
    usleep(1);
  }

}
