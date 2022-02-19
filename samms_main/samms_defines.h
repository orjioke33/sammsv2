#ifndef _SAMMS_DEFINES_H    // Put these two lines at the top of your file.
#define _SAMMS_DEFINES_H    // (Use a suitable name, usually based on the file name.)

// Subcore IDs
#define SUBCORE_ACCEL                           1
#define SUBCORE_ACCEL_PROCESSING                2
#define SUBCORE_AUDIO_PROCESSING                3

// Subcore Message IDs
// Send is even, Recv is odd
#define MP_SEND_ID_AUDIO_PROCESSING             0
#define MP_SEND_ID_ACCEL                        2
#define MP_SEND_ID_ACCEL_PROCESSING             4

#define MP_RECV_ID_AUDIO_PROCESSING             1
#define MP_RECV_ID_ACCEL                        3
#define MP_RECV_ID_ACCEL_PROCESSING             5

#endif // _SAMMS_DEFINES_H    // Put this line at the end of your file.
