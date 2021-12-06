// System Headers
#include <MP.h>

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

const int subcore = 1;

void setup() {
  // Filter Setup - TO DO Cleanup
  int ret;

  // Open serial for debugging
  Serial.begin(115200);
  while (!Serial);

  // AUDIO & FILTERING
  Serial.println("Init Audio Library");
  audioStruct->audio->begin();
  Serial.println("Init Audio Recorder");
  // Select input device as AMIC
  audioStruct->audio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);
  set_audio_channel(audioStruct, AS_CHANNEL_STEREO);
  audioStruct->audio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", AS_SAMPLINGRATE_48000, audioStruct->channel);

  // Launch SubCore
  ret = MP.begin(subcore);
  if (ret < 0) {
    printf("MP.begin error = %d\n", ret);
  }

  Serial.println("Rec start!");
  audioStruct->audio->startRecorder();

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
    MP.Send(sndid, &capture, subcore);
    Serial.println("Sending Data to Subcore1");
  } else {
    usleep(1);
  }

}
