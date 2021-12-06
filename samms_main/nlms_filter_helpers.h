#ifndef _NLMS_FILTER_HELPERS_H    // Put these two lines at the top of your file.
#define _NLMS_FILTER_HELPERS_H    // (Use a suitable name, usually based on the file name.)

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

// PROTOTYPES
filterCapture_t * get_filter_capture_data(void);
filterResult_t * get_filter_result_data(void);

#endif // _NLMS_FILTER_HELPERS_H    // Put this line at the end of your file.
