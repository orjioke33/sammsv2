#ifndef _NLMS_FILTER_HELPERS_H    // Put these two lines at the top of your file.
#define _NLMS_FILTER_HELPERS_H    // (Use a suitable name, usually based on the file name.)

#include "audio_mic_helpers.h"

// TYPEDEFS
// What does this do?
typedef struct filterCapture_t 
{
  void *buff;
  int  sample;
  int  chnum;
} filterCapture_t;

// What does this do?
typedef struct filterResult_t
{
  void *buff;
  int sample;
} filterResult_t;

// What does this do?
void update_capture_filter (filterCapture_t * _capture, audioClassStruct_t * _audioStruct);

// PROTOTYPES
filterCapture_t * get_filter_capture_struct(void);
filterResult_t * get_filter_result_struct(void);

#endif // _NLMS_FILTER_HELPERS_H    // Put this line at the end of your file.
