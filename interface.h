#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include "glx_base.h"


//void open_channel_canvas(int mychannel);
//void close_channel_canvas(int mychannel);
//GLuint get_channel_texture(int mychannel);
//float get_video_mix_slide();
//void prepare_canvases(void);
typedef struct {
	int dmx_x;
	int dmx_y;
	float tl_x;
	float tl_y;
	float tr_x;
	float tr_y;
	float bl_x;
	float bl_y;
	float br_x;
	float br_y;	
}deform_point;


void setup_interface(void);
void render_all(void);
void ai(void);
int get_video_capture_label_id(void);
void edit_mode(char *key);
#endif
