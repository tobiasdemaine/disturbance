#ifndef _GUI_H_
#define _GUI_H_
#include "glx_base.h"


typedef struct{
	GLuint hover_texture;
	GLuint off_texture;
	GLuint on_texture;
	float image_width;
	float image_height;
	int state;
	char *string;
	int toggel;
	BOOL checked;
}button;

typedef struct{
	GLuint texture;
}image;

typedef struct{
	int visible;
	char *string;
	int font_base;
	int font_set;
	int editable;
	int state;
	int cursor_at;
	int numeric;
	int display_chars;
}label;

typedef struct{
	char *string;
	int font_base;
	
}input;

typedef struct{
	GLuint font_texture;
	GLuint font_base;
}texture_font;

typedef struct{
	GLuint hover_texture;
	GLuint off_texture;
	int base_id;
	int vert;
	float min;
	float max;
	float state;
}slider;

typedef struct{
	GLuint canvas_texture;
	float awidth;
	float aheight;
	float fov;
}canvas;

typedef struct{
	int hoverfunc;
	void (*hover_func)(void);
	int depressfunc;
	void (*depress_func)(void);
	int releasefunc;
	void (*release_func)(void);
	int dragfunc;
	void (*drag_func)(void);
	int deselectfunc;
	void (*deselect_func)(void);
	int  istype;
	int parent;
	int over;
	float left;
	float top;
	float width;
	float height;
	float mousexrelative;
	float mouseyrelative;
	int visible;
	int press; /* used for internal drag and drops */
	button tdm_button;
	image tdm_image;
	texture_font tdm_font;
	label tdm_label;
	slider tdm_slider;
	canvas tdm_canvas;
} gui;

int container_create(float left, float top, float width, float height,int visible);
int button_create(int parent, char *hover_texture, char *off_texture, char *on_texture, float left, float top, float width, float height, int state, int visible, char *label, int toggel);
int image_create(int parent, float left, float top, float width, float height, int visible, char *texture);
int font_create(char *fontfile);
int label_create(int parent, char *string, int font, int set, float left, float top, int visible, int editable);
int verticle_slider_create(int parent, char* button_texture_off, char* button_texture_hover, float button_width, float button_height,char* back_texture, float back_width, float back_height, float left, float top, float slide_min_value, float slide_max_value, int visible);
int horizontal_slider_create(int parent, char* button_texture_off, char* button_texture_hover, float button_width, float button_height,char* back_texture, float back_width, float back_height, float left, float top, float slide_min_value, float slide_max_value, int visible);

float get_slider_value(int object);
void set_slider_value(int object, float value);

void set_label_text(int object, char *string);

int create_canvas(int cwidth, int cheight, float awidth, float aheight, float fov);
void open_canvas(int canvas);
void close_canvas(int canvas);
void draw_canvas(int canvas, int cx, int cy, int cwidth, int cheight);
GLuint get_canvas_texture(int canvas);
int get_canvas_width(int canvas);
int get_canvas_height(int canvas);

float get_object_width(int object);
float get_object_height(int object);

void set_object_visible(int object, int visible);
int get_object_visible(int object);

void label_only_numeric(int object);
void label_display_chars(int object, int visnum);
float get_label_text_as_float(int object);
int get_label_text_as_int(int object);
char * get_label_text(int object);

void do_button_depress(int button);
void do_button_release(int button);
void gui_keypress_handler(char *key);
void gui_keyrelease_handler(char *key);

int get_last_object_depress(void);
int get_last_object_release(void);

int get_object_state(int object);
void set_object_state(int object, int thestate);

void set_object_position(int object, float left, float top);

void set_hover_function(int object, void (*thefunction)(void));
void set_depress_function(int object, void (*thefunction)(void));
void set_release_function(int object, void (*thefunction)(void));
void set_deselect_function(int object, void (*thefunction)(void));

void gui_exec(void);
#endif 
