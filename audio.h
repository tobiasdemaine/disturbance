#ifndef _AUDIO_H_
#define _AUDIO_H_
#include "glx_base.h"
#include "fft.h"
#define ALSA_PCM_OLD_HW_PARAMS_API
#define ALSA_PCM_OLD_SW_PARAMS_API
#include <alsa/asoundlib.h> 

void start_audio();
int get_fft_value(int id);
float get_level();
int set_volume();
int set_pan();
int get_beat();
void sound_process(void);
int get_peak(int clip_left, int clip_right,int peak);
void draw_oscilliscope_4_3d(void);

#endif
