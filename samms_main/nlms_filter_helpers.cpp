#include <stddef.h>
#include "nlms_filter_helpers.h"

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

filterCapture_t * get_filter_capture_data (void) {
  return &_filterCapture;
}

filterResult_t * get_filter_result_data (void) {
  return &_filterResult;
}
