#ifndef _AUDIO_MIC_HELPERS_H    // Put these two lines at the top of your file.
#define _AUDIO_MIC_HELPERS_H    // (Use a suitable name, usually based on the file name.)

#include <Audio.h>

#define BUFFER_SAMPLE       768 * AS_CHANNEL_STEREO
#define BUFFER_SIZE         BUFFER_SAMPLE * sizeof(int16_t)

// VARIABLES
// What does this do?
typedef struct audioClassStruct_t {
  AudioClass * audio;
  int channel;
  char buffer[BUFFER_SIZE];
  uint32_t read_size;
} audioClassStruct_t;

// What does this do?
audioClassStruct_t * get_audio_class_struct (void);
// What does this do?
void set_audio_channel (audioClassStruct_t * _audioStruct, int mic_channel_num);
// What does this do?
err_t init_audio_struct_and_record (audioClassStruct_t * _audioStruct);
// What does this do?
err_t read_frames_to_recorder (audioClassStruct_t * _audioStruct);

#endif // _AUDIO_MIC_HELPERS_H    // Put this line at the end of your file.
