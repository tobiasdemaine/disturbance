#ifndef _VIDEO_H_
#define _VIDEO_H_
#include "glx_base.h"
#include <avifile-0.7/avifile.h>
#include <avifile-0.7/avm_default.h>
#include <avifile-0.7/image.h>
#include <avifile-0.7/infotypes.h>

typedef struct{
	IAviReadFile* id;
	int frame;
	framepos_t current_frame;
	GLuint frame_texture;
	int state;
	int speed;
	int msec;
	int width;                      
	int height;
	int loop_begin; /* to do */
	int loop_end;	/* to do */
	int next;
	double stream_fps;  		
	avm::IReadStream *avistream;  
	StreamInfo *streaminfo;
	float r;       
	float g;
	float b;
	float a;
}mediastream;

void capture_video(void);
int get_capture_size(void);

int stream_create(char *file);

void stream_play(int stream);
void stream_stop(int stream);
void stream_rewind(int stream);
void stream_rewind_stop(int stream);
void stream_seek(int stream, int frame);

void stream_set_speed(int stream,int speed);
int stream_get_speed(int stream);
int stream_is_streaming(int stream);
int stream_get_position(int stream);

void stream_loop(int stream, int start_frame, int end_frame);

void stream_direction(int stream, int direction);


void stream_get_frame(int stream);
void stream_display(int stream, int swidth, int sheight, int sx, int sy);

void stream_set_rgba(int stream, float r, float g, float b, float a);

void initvideo4linux(void);
int get_buffer_height(void);
int get_buffer_width(void);
unsigned char *get_buffer_map(void);
#endif
