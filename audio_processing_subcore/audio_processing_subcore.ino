/*
 *  SubCore.ino - Example from Sony incorporating nLMS filtering method from CMSIS
 *  Made to test subcore filtering method using Generalized Sidelobe Canceller
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <MP.h>

/* Use CMSIS library */
#ifndef ARM_MATH_CM4
#define ARM_MATH_CM4
#endif

#ifndef __FPU_PRESENT
#define __FPU_PRESENT 1U
#endif
#include <arm_math.h>
#define NUMTAPS 32
#define BLOCKSIZE 1024
#define MU 0.5f


#include "ring_buff.h"
#include "samms_defines.h"

/* Select FFT length */

#define FFTLEN 1024

const int g_channel = 2;

/* Ring buffer */

#define INPUT_BUFFER (1024 * 4)
RingBuff ringbuf[g_channel] = {
  RingBuff(INPUT_BUFFER),
  RingBuff(INPUT_BUFFER)
};

/* Allocate the larger heap size than default */

USER_HEAP_SIZE(64 * 1024);

/* CMSIS Definitions */

arm_lms_norm_instance_f32 lmsNormInstance;
float32_t nlmsCoeffs[NUMTAPS];
float32_t nlmsStateF32[NUMTAPS + BLOCKSIZE];
float32_t err_signal[BLOCKSIZE];
float32_t convergedSignal[BLOCKSIZE];

float32_t pSrc_left[BLOCKSIZE];
float32_t pSrc_right[BLOCKSIZE];
float32_t pSum[BLOCKSIZE];
float32_t pDiff[BLOCKSIZE];

/* MultiCore definitions */

struct Capture {
  void *buff;
  int  sample;
  int  chnum;
};

struct Result {
  void *buffer;
  int sample;
};


void setup()
{
  int ret = 0;

  /* Initialize MP library */
  ret = MP.begin();
  if (ret < 0)
  {
    errorLoop(2);
  }
  /* receive with non-blocking */
  MP.RecvTimeout(MP_RECV_POLLING);

  arm_lms_norm_init_f32(&lmsNormInstance, NUMTAPS, nlmsCoeffs, nlmsStateF32, MU, BLOCKSIZE);
  printf("Subcore %d Initialized \n",1);
}

void loop()
{
  int      ret;
  int8_t   recvId;
  int8_t   sendId;
  Capture *capture;
  Result result;

  /* Receive PCM captured buffer from MainCore */
  ret = MP.Recv(&recvId, &capture);
  if (ret >= 0) 
  {
    printf("Message Received\n");
    if (capture->chnum == 1) 
    {
      /* the faster optimization */
      ringbuf[0].put((q15_t*)capture->buff, capture->sample);
    } else {
      int i;
      for (i = 0; i < capture->chnum; i++) 
      {
        ringbuf[i].put((q15_t*)capture->buff, capture->sample, capture->chnum, i);
      }
    }
  }

  while (ringbuf[0].stored() >= FFTLEN)
  {
    // Pull data out of the ring buffer
    ringbuf[0].get(pSrc_left, FFTLEN);
    ringbuf[1].get(pSrc_right, FFTLEN);

    // Create sum and difference of signals
    for (int i = 0; i < BLOCKSIZE; i++)
    {
      pSum[i] = pSrc_left[i] + pSrc_right[i];
      pDiff[i] = pSrc_left[i] - pSrc_right[i];
    }
    
    // Perform nLMS filtering
    arm_lms_norm_f32(&lmsNormInstance, pSum, pDiff, convergedSignal, err_signal, BLOCKSIZE);
    
    printf("filtering Occuring\n");
    result.buffer = err_signal;
    result.sample = BLOCKSIZE;

  }
}


void errorLoop(int num)
{
  int i;

  while (1) {
    for (i = 0; i < num; i++) {
      ledOn(LED0);
      delay(300);
      ledOff(LED0);
      delay(300);
    }
    delay(1000);
  }
}
