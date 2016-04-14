#include "audio.h"
float sound;
int beat;
int        thr_id;   
pthread_t  p_thread;

short int audiobuf[2048];
short int fftdata[10000];

snd_pcm_t *pcm_handle;
snd_pcm_hw_params_t *params;
#define FREQ_SAMPLES 8192

int db2sy( float db )
{
  return (int)( ( db - 10.0 ) / ( 110.0 ) * (float)(256) );
}

inline float sqr( float arg )
{
  return arg * arg;
}
float value_e[8192];
void envelope(float alphaValue)
{
  // normalised to 1
  
  float sum = 0.0;
  for( int i = 0; i < FREQ_SAMPLES; i++ ) {
    float x = (float)( i - FREQ_SAMPLES/2 ) / FREQ_SAMPLES;
    value_e[i] = exp( - alphaValue * sqr( x ) );
    sum += value_e[i];
  }
  sum /= FREQ_SAMPLES;
  
  for( int i = 0; i < FREQ_SAMPLES; i++ )
    value_e[i] /= sum;
}

void audiothread(void* a){ 	
	int chunk_size;
	size_t chunk_bytes;
	snd_pcm_open( &pcm_handle,"default", SND_PCM_STREAM_CAPTURE, 0);
	snd_pcm_hw_params_alloca( &params);
	snd_pcm_hw_params_any( pcm_handle, params);
	snd_pcm_hw_params_set_access( pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
	snd_pcm_hw_params_set_format( pcm_handle, params, SND_PCM_FORMAT_S16_LE);
	snd_pcm_hw_params_set_rate_near( pcm_handle, params, 44100, 0);
	snd_pcm_hw_params_set_channels( pcm_handle, params, 2);
	snd_pcm_hw_params( pcm_handle, params);
	
	chunk_size = snd_pcm_hw_params_get_period_size(params, 0);
	chunk_bytes = chunk_size *4;
	snd_pcm_prepare (pcm_handle);
	int stop=1;
		
	envelope(-1);	
	    	
	float freqInBuffer[10000];
	float value_db[8192];
	
	int num_fft = 8192/2+1;
	float norm = - 27.1;

	while(stop==1){
		int freqCpt=0;
		snd_pcm_readi(pcm_handle, audiobuf, 1024); 
		int i;
		for(i=0;i<num_fft;i++){
  			freqInBuffer[freqCpt] = value_e[freqCpt++]*((float)audiobuf[i++]+(float)audiobuf[i++])/(float)2/norm;
			//freqInBuffer[i] = ((float)audiobuf[i++]+(float)audiobuf[i++])/(float)2/norm;
		}
		
		rfft(freqInBuffer, FREQ_SAMPLES, 1);      
				
		for( int i = 0; i < num_fft; i++ ){
		
			float db, p;
			
			if( i != 0 && i != num_fft - 1 )
				p = sqr( freqInBuffer[2*i+0] ) + sqr( freqInBuffer[2*i+1] );
			else 
				p = sqr( freqInBuffer[ i == 0 ? 0 : 1 ] );
			
			db = 10.0 * log10( p ) + norm; // gauge ( 990.5273 Hz, 0 dB )	  	
			value_db[i] = db;
			
			fftdata[i] = db2sy( value_db[i] );	  
			if (fftdata[i] < 0){
				fftdata[i]=0;
			}
			
	    	}
		
		
	}
}

void start_audio(void){
	thr_id = pthread_create(&p_thread, NULL, (void*(*)(void*))audiothread, (void*)0);
}

int get_fft_value(int id){
	return fftdata[id];
}

float os4[26][26];
void draw_oscilliscope_4_3d(void){
	float rw = 600;
	for(int mstep=0;mstep<26;mstep++){
		for(int ss=18; ss>0; ss--){
			os4[ss][mstep] = os4[ss - 1][mstep];
			glBegin(GL_LINES);
			glVertex3f((mstep * (100 / 25) * (rw / 100)) - 150, (os4[ss][mstep]), ss * 20);	
			glVertex3f((((mstep + 1) * (100 / 25)) * (rw / 100)) - 150, os4[ss][mstep], (ss * 20));
			glVertex3f((mstep * (100 / 25) * (rw / 100)) - 150, (os4[ss][mstep]), ss * -20);				
			glVertex3f((((mstep + 1) * (100 / 25)) * (rw / 100)) - 150, os4[ss][mstep], (ss * -20));
			glVertex3f((-mstep * (100 / 25) * (rw / 100)) - 150, os4[ss][mstep], ss * 20);				
			glVertex3f((((-mstep - 1) * (100 / 25)) * (rw / 100)) - 150, os4[ss][mstep], (ss * 20));
			glVertex3f((-mstep * (100 / 25) * (rw / 100)) - 150, os4[ss][mstep], ss * -20);
			glVertex3f((((-mstep - 1) * (100 / 25)) * (rw / 100)) - 150, os4[ss][mstep], (ss * -20));
			glEnd();
		}
	os4[0][mstep] = fftdata[mstep];
	}
}


float get_level(void){
	return sound;
}
int get_beat(void){
	return beat;
}
/* this is a really quick hack TODO properly*/

void sound_process(void){
	sound = ((fftdata[10] + fftdata[20] + fftdata[5] + fftdata[15] + fftdata[25] + fftdata[30]) / 6)/10;
	if (sound >= 5){
		beat=1;
	}else{
		beat=0;
	}
}
