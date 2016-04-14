#ifndef _HELPER_H_
#define _HELPER_H_
#include "glx_base.h"
#include "gui.h"
GLuint load_image(char *image, int gui_c);
GLuint load_texture2d_image(char *image, int gui_c);
GLuint load_sequence_from_disk_image(char *image, GLuint itexture);
void set_wireframe_render(void);
void set_solid_render(void);
void set_color_from_rgba(int r, int g, int b, int a);
	
void set_color_from_rgb(int r, int g, int b, int a);
#endif
