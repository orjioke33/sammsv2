#include <stddef.h>
#include <Audio.h>
#include "audio_mic_helpers.h"

// VARIABLES
audioClassStruct_t _audioStruct = {
  .audio = AudioClass::getInstance(),
  .channel = 1
};

// Prototypes
audioClassStruct_t * get_audio_class_struct(void) {
  return &_audioStruct;
}

void set_audio_channel (audioClassStruct_t * _audioStruct, int mic_channel_num) {
    switch (mic_channel_num) {
    case 1: 
      _audioStruct->channel = AS_CHANNEL_MONO;
      break;
    case 2: 
      _audioStruct->channel = AS_CHANNEL_STEREO;
      break;
    case 4:
      _audioStruct->channel = AS_CHANNEL_4CH;
      break;
    default:
      _audioStruct->channel = AS_CHANNEL_MONO;
  }
}
//
