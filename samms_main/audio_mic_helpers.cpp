#include <stddef.h>
#include <Audio.h>
#include "audio_mic_helpers.h"

// Variables
audioClassStruct_t _audioStruct = {
  .audio = AudioClass::getInstance(),
  .channel = AS_CHANNEL_STEREO,
};

// Public Functions
audioClassStruct_t * get_audio_class_struct(void) {
  return &_audioStruct;
}

err_t init_audio_struct_and_record (audioClassStruct_t * _audioStruct) {
  err_t err = -1;
  err = _audioStruct->audio->begin();
  // Select input device as AMIC
  err = _audioStruct->audio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC);
  err = _audioStruct->audio->initRecorder(AS_CODECTYPE_PCM, "/mnt/sd0/BIN", AS_SAMPLINGRATE_48000, _audioStruct->channel);
  err = _audioStruct->audio->startRecorder();
  exit_function:
    return err;
}

err_t read_frames_to_recorder (audioClassStruct_t * _audioStruct) {
  err_t err;

  // Read the frames to record in buffer
  err = _audioStruct->audio->readFrames(_audioStruct->buffer, BUFFER_SIZE, &(_audioStruct->read_size));

  // Error checking
  if (err != AUDIOLIB_ECODE_OK && err != AUDIOLIB_ECODE_INSUFFICIENT_BUFFER_AREA) {
    sleep(1);
    _audioStruct->audio->stopRecorder();
  }
  return err;
}
