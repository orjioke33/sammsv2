#ifndef _AUDIO_MIC_HELPERS_H    // Put these two lines at the top of your file.
#define _AUDIO_MIC_HELPERS_H    // (Use a suitable name, usually based on the file name.)

#include <Audio.h>

// VARIABLES
typedef struct audioClassStruct_t {
  AudioClass * audio;
  int channel;
} audioClassStruct_t;

audioClassStruct_t * get_audio_class_struct (void);
void set_audio_channel (audioClassStruct_t * _audioStruct, int mic_channel_num);

#endif // _AUDIO_MIC_HELPERS_H    // Put this line at the end of your file.
