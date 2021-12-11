#include <stddef.h>
#include "nlms_filter_helpers.h"
#include "audio_mic_helpers.h"

// VARIABLES
filterCapture_t _filterCapture = {
  .buff = NULL,
  .sample = 0,
  .chnum = 0
};

filterResult_t _filterResult = {
  .buff = NULL,
  .sample = 0
};

filterCapture_t * get_filter_capture_struct (void) {
  return &_filterCapture;
}

filterResult_t * get_filter_result_struct (void) {
  return &_filterResult;
}

void update_capture_filter (filterCapture_t * _capture, audioClassStruct_t * _audioStruct) {
  _capture->buff   = _audioStruct->buffer;
  _capture->sample = BUFFER_SAMPLE / AS_CHANNEL_STEREO;
  _capture->chnum  = AS_CHANNEL_STEREO;
}
