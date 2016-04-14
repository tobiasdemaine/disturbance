#include "gui.h"
#include "helper.h"

gui tdm_gui[10000]; /* This need to be dynamically made when I figure out how */
int gui_count = 0;
int keyshift=0;
int keycaps=0;
int last_object_depress;
int last_object_release;

int button_create(int parent, char *hover_texture, char *off_texture, char *on_texture, float left, float top, float width, float height, int state, int visible, char *label, int toggel){
	gui_count = gui_count+1;
	tdm_gui[gui_count].istype = 2;
	tdm_gui[gui_count].parent = parent;
	tdm_gui[gui_count].left = left;
	tdm_gui[gui_count].top = top;
	tdm_gui[gui_count].width = width;
	tdm_gui[gui_count].height = height;
	tdm_gui[gui_count].visible = visible;
	tdm_gui[gui_count].tdm_button.state = state;
	tdm_gui[gui_count].tdm_button.hover_texture = load_image(hover_texture, gui_count);
	tdm_gui[gui_count].tdm_button.off_texture = load_image(off_texture, gui_count);
	tdm_gui[gui_count].tdm_button.on_texture = load_image(on_texture, gui_count);
	tdm_gui[gui_count].tdm_button.toggel = toggel;
	/*tdm_gui[a].tdm_button.label = *label;*/
	return gui_count;
}

int image_create(int parent, float left, float top, float width, float height, int visible, char *texture){
	gui_count = gui_count+1;
	tdm_gui[gui_count].istype = 3;
	tdm_gui[gui_count].parent = parent;
	tdm_gui[gui_count].left = left;
	tdm_gui[gui_count].top = top;
	tdm_gui[gui_count].width = width;
	tdm_gui[gui_count].height = height;
	tdm_gui[gui_count].visible = visible;
	tdm_gui[gui_count].tdm_image.texture = load_image(texture, gui_count);
	return gui_count;
}

int container_create(float left, float top, float width, float height, int visible){
	gui_count = gui_count+1;
	tdm_gui[gui_count].istype = 1;
	tdm_gui[gui_count].left = left;
	tdm_gui[gui_count].top = top;
	tdm_gui[gui_count].width = width;
	tdm_gui[gui_count].height = height;
	tdm_gui[gui_count].visible = visible;
	return gui_count;
}

/* Bitmap font stuff */
int font_create(char *fontfile) {
    GLfloat cx, cy ;
    int loop;         /* the character coordinates in our texture */
    gui_count = gui_count+1;
    tdm_gui[gui_count].istype = 4;
    tdm_gui[gui_count].tdm_font.font_texture = load_texture2d_image(fontfile, gui_count);
    tdm_gui[gui_count].tdm_font.font_base = glGenLists(256);
    tdm_gui[gui_count].height = 256/16;
    tdm_gui[gui_count].width = 10;
    glBindTexture(GL_TEXTURE_2D, tdm_gui[gui_count].tdm_font.font_texture);
    for (loop = 0; loop < 256; loop++)
    {
        cx = (float) (loop%16) / 16.0f;
        cy = (float) (loop / 16) / 16.0f;
        glNewList(tdm_gui[gui_count].tdm_font.font_base + loop, GL_COMPILE);
            glBegin(GL_QUADS);
                glTexCoord2f(cx, 1 - cy - 0.0625f);
                glVertex2i(0, 0);
                glTexCoord2f(cx + 0.0625f, 1 - cy - 0.0625f);
                glVertex2i(16, 0);
                glTexCoord2f(cx + 0.0625f, 1 - cy);
                glVertex2i(16, 16);
                glTexCoord2f(cx, 1 - cy);
                glVertex2i(0, 16);
            glEnd();
            glTranslated(10, 0, 0);
        glEndList();
    }
    return gui_count;
}

int verticle_slider_create(int parent, char* button_texture_off, char* button_texture_hover, float button_width, float button_height,char* back_texture, float back_width, float back_height, float left, float top, float slide_min_value, float slide_max_value, int visible){
	int sliderbase;
	 /* make slider back first as an gui_image */
	sliderbase =  image_create(parent, left, top, back_width, back_height, visible, back_texture);
	gui_count = gui_count+1;
	 /* make slider button as new object */
	tdm_gui[gui_count].istype = 6;
	tdm_gui[gui_count].parent = parent;
	tdm_gui[gui_count].visible = visible;
	tdm_gui[gui_count].tdm_slider.base_id = sliderbase;
	tdm_gui[gui_count].tdm_slider.state= 0;
	tdm_gui[gui_count].tdm_slider.hover_texture = load_image(button_texture_hover, gui_count);
	tdm_gui[gui_count].tdm_slider.off_texture = load_image(button_texture_off, gui_count);
	tdm_gui[gui_count].width = button_width;
	tdm_gui[gui_count].height = button_height;
	 /*place slider at base */
	tdm_gui[gui_count].top = tdm_gui[sliderbase].top+tdm_gui[sliderbase].height-tdm_gui[gui_count].height;
	/* center button on back ground image*/
	tdm_gui[gui_count].left = tdm_gui[sliderbase].left+((tdm_gui[sliderbase].width/2)-(tdm_gui[gui_count].width/2));
	tdm_gui[gui_count].tdm_slider.min = slide_min_value;
	tdm_gui[gui_count].tdm_slider.max = slide_max_value;
	tdm_gui[gui_count].tdm_slider.vert = 1;
	return gui_count;
}

int horizontal_slider_create(int parent, char* button_texture_off, char* button_texture_hover, float button_width, float button_height,char* back_texture, float back_width, float back_height, float left, float top, float slide_min_value, float slide_max_value, int visible){
	 int sliderbase;
	 /* make slider back first as an gui_image */
	 sliderbase =  image_create(parent, left, top, back_width, back_height, visible, back_texture);
	 gui_count = gui_count+1;
	 /* make slider button as new object */
	tdm_gui[gui_count].istype = 6;
	tdm_gui[gui_count].parent = parent;
	tdm_gui[gui_count].visible = visible;
	tdm_gui[gui_count].tdm_slider.base_id = sliderbase;
	tdm_gui[gui_count].tdm_slider.state= 0;
	tdm_gui[gui_count].tdm_slider.hover_texture = load_image(button_texture_hover, gui_count);
	tdm_gui[gui_count].tdm_slider.off_texture = load_image(button_texture_off, gui_count);
	tdm_gui[gui_count].width = button_width;
	tdm_gui[gui_count].height = button_height;
	 /*place slider at base */
	tdm_gui[gui_count].top = tdm_gui[sliderbase].top+((tdm_gui[sliderbase].height/2)-(tdm_gui[gui_count].height/2));
	/* center button on back ground image*/
	tdm_gui[gui_count].left = tdm_gui[sliderbase].left+tdm_gui[sliderbase].width-tdm_gui[gui_count].width;
	tdm_gui[gui_count].tdm_slider.min = slide_min_value;
	tdm_gui[gui_count].tdm_slider.max = slide_max_value;
	tdm_gui[gui_count].tdm_slider.vert = 0;
	return gui_count;
}

void set_slider_value(int object, float value){
	if(tdm_gui[object].tdm_slider.vert==1){
		tdm_gui[object].top = tdm_gui[tdm_gui[object].tdm_slider.base_id].height-((value/(tdm_gui[object].tdm_slider.max/100))*(tdm_gui[tdm_gui[object].tdm_slider.base_id].height/100)-tdm_gui[tdm_gui[object].tdm_slider.base_id].top);/*tdm_gui[tdm_gui[object].parent].top*/
		/* some testing has to be done here yet */
	}
	if(tdm_gui[object].tdm_slider.vert==0){
		tdm_gui[object].left = tdm_gui[tdm_gui[object].tdm_slider.base_id].left+(value/(tdm_gui[object].tdm_slider.max/100))*(tdm_gui[tdm_gui[object].tdm_slider.base_id].width/100);
		if(tdm_gui[object].left>tdm_gui[tdm_gui[object].tdm_slider.base_id].width-tdm_gui[object].width){
			tdm_gui[object].left=tdm_gui[object].left-tdm_gui[object].width;
		}
	}
}

float get_slider_value(int object){
	float slider_top=0;
	float slider_bottom=0; 
	float slide_button_top=0;
	if(tdm_gui[object].tdm_slider.vert==1){
		slider_top = tdm_gui[tdm_gui[object].tdm_slider.base_id].top; /*+tdm_gui[tdm_gui[object].parent].top;*/
		slider_bottom = tdm_gui[tdm_gui[object].tdm_slider.base_id].top+tdm_gui[tdm_gui[object].tdm_slider.base_id].height-tdm_gui[object].height; /*+tdm_gui[tdm_gui[object].parent].top*/
		slide_button_top = tdm_gui[object].top; 
		float cmax = slider_top - slider_bottom;
		float cpos = slide_button_top - slider_bottom;
		return (tdm_gui[object].tdm_slider.max/100)*(cpos/(cmax/100));
	}else if(tdm_gui[object].tdm_slider.vert==0){
		float slider_top, slider_bottom, slide_button_top;
		slider_top = tdm_gui[tdm_gui[object].tdm_slider.base_id].left;
		slider_bottom = tdm_gui[tdm_gui[object].tdm_slider.base_id].left+tdm_gui[tdm_gui[object].tdm_slider.base_id].width-tdm_gui[object].width;
		slide_button_top = tdm_gui[object].left; 
		float cmax = slider_bottom - slider_top;
		float cpos = slider_bottom - slide_button_top;
		return (tdm_gui[object].tdm_slider.max/100)*(cpos/(cmax/100));
	}else{
		return 0;
	}
		
}

void set_label_text(int object, char *string){
	if(tdm_gui[object].tdm_label.display_chars!=0){
		tdm_gui[object].width = tdm_gui[tdm_gui[object].tdm_label.font_base].width*(tdm_gui[object].tdm_label.display_chars);
	}else{
		tdm_gui[object].width = tdm_gui[tdm_gui[object].tdm_label.font_base ].width*strlen(string);
	}
	
	tdm_gui[object].tdm_label.string = string;
}

void label_add_char(char *key, int object){
	char *tmp = (char *)calloc(strlen(tdm_gui[object].tdm_label.string) + 2, sizeof(char));
	strncpy(tmp, tdm_gui[object].tdm_label.string, (tdm_gui[object].tdm_label.cursor_at));
	strcat(tmp, key);
  	strncat(tmp, tdm_gui[object].tdm_label.string+tdm_gui[object].tdm_label.cursor_at, strlen(tdm_gui[object].tdm_label.string)-tdm_gui[object].tdm_label.cursor_at);
	tdm_gui[object].tdm_label.cursor_at++;
	set_label_text(object, tmp);
}

void label_only_numeric(int object){
	tdm_gui[object].tdm_label.numeric=1;
}

void label_display_chars(int object, int visnum){
	tdm_gui[object].tdm_label.display_chars=visnum;
	tdm_gui[object].width = tdm_gui[tdm_gui[object].tdm_label.font_base].width*(visnum);
}
float get_label_text_as_float(int object){
	char *pEnd;
	return strtod(tdm_gui[object].tdm_label.string, &pEnd);
}
char *  get_label_text(int object){
	return tdm_gui[object].tdm_label.string;
}
int get_label_text_as_int(int object){
	char *pEnd;
	return (int)strtod(tdm_gui[object].tdm_label.string, &pEnd);
}

char *character_to_upper(char *key){
	KeySym low;
	KeySym upper;
	XConvertCase(XStringToKeysym(key), &low, &upper);
	if(keyshift==1){
		return XKeysymToString(upper);
	}else{
		return key;
	}
}

void label_insert_char(char *key, int object){
	/* to do key filters space */
	if(tdm_gui[object].tdm_label.numeric){
		if(strcmp(key, "period")==0){
			label_add_char(".", object);
		}
		if(strcmp(key, "minus")==0){
			label_add_char("-", object);
		}
		if(strlen(key)==1){
			if((strcmp(key, "0")==0)||(strcmp(key, "1")==0)||(strcmp(key, "2")==0)||(strcmp(key, "3")==0)||(strcmp(key, "4")==0)||(strcmp(key, "5")==0)){
				label_add_char(character_to_upper(key), object);
			}
			if((strcmp(key, "6")==0)||(strcmp(key, "9")==0)){
				label_add_char(character_to_upper(key), object);
			}
			if((strcmp(key, "7")==0)||(strcmp(key, "8")==0)){
				label_add_char(character_to_upper(key), object);
			}
		}
	}else{
	if(strlen(key)==1){
		label_add_char(character_to_upper(key), object);
	}
	if(strcmp(key, "space")==0){
		label_add_char(" ", object);
	}
	if(strcmp(key, "period")==0){
		label_add_char(".", object);
	}
	if(strcmp(key, "comma")==0){
		label_add_char(",", object);
	}
	if(strcmp(key, "apostrophe")==0){
		label_add_char("'", object);
	}
	if(strcmp(key, "bracketleft")==0){
		label_add_char("[", object);
	}
	if(strcmp(key, "bracketright")==0){
		label_add_char("]", object);
	}
	if(strcmp(key, "slash")==0){
		label_add_char("/", object);
	}
	if(strcmp(key, "backslash")==0){
		label_add_char("\\", object);
	}
	if(strcmp(key, "semicolon")==0){
		label_add_char(";", object);
	}
	if(strcmp(key, "grave")==0){
		label_add_char("/", object);
	}
	}
	if(strcmp(key, "BackSpace")==0){
		if(tdm_gui[object].tdm_label.cursor_at > 0){
			char *tmp = (char *)calloc(strlen(tdm_gui[object].tdm_label.string), sizeof(char));
			strncpy(tmp, tdm_gui[object].tdm_label.string, (tdm_gui[object].tdm_label.cursor_at)-1);
			strncat(tmp, tdm_gui[object].tdm_label.string+tdm_gui[object].tdm_label.cursor_at, strlen(tdm_gui[object].tdm_label.string)-tdm_gui[object].tdm_label.cursor_at);
			tdm_gui[object].tdm_label.cursor_at--;
			set_label_text(object, tmp);
		}
	}
	if(strcmp(key, "Delete")==0){
		char *tmp = (char *)calloc(strlen(tdm_gui[object].tdm_label.string), sizeof(char));
		strncpy(tmp, tdm_gui[object].tdm_label.string, (tdm_gui[object].tdm_label.cursor_at));
		strncat(tmp, tdm_gui[object].tdm_label.string+tdm_gui[object].tdm_label.cursor_at+1, strlen(tdm_gui[object].tdm_label.string)-tdm_gui[object].tdm_label.cursor_at-1);
		set_label_text(object, tmp);
	}
	if(strcmp(key, "Left")==0){
		if(tdm_gui[object].tdm_label.cursor_at > 0){
			tdm_gui[object].tdm_label.cursor_at--;
		}
	}
	if(strcmp(key, "Right")==0){
		if(tdm_gui[object].tdm_label.cursor_at < (signed)strlen(tdm_gui[object].tdm_label.string)){
		tdm_gui[object].tdm_label.cursor_at++;
		}
	}
	if(strcmp(key, "End")==0){
		tdm_gui[object].tdm_label.cursor_at=strlen(tdm_gui[object].tdm_label.string);
	}
	if(strcmp(key, "Home")==0){
		tdm_gui[object].tdm_label.cursor_at=0;
	}
}

/* float ceilf (float x); */ /* this is strange as its included a long time before this */

void set_label_cusor(int object){
	float tmpa = (float)tdm_gui[object].mousexrelative/(float)tdm_gui[tdm_gui[object].tdm_label.font_base].width;
	tmpa = ceilf(tmpa);
	tdm_gui[object].tdm_label.cursor_at = (int)tmpa;
}

void draw_label_cusor(int object){
	/* cursor is only glLine */	
	glEnable(GL_LINE_SMOOTH);
	glBegin(GL_LINES);
		glVertex2f((tdm_gui[object].tdm_label.cursor_at*tdm_gui[tdm_gui[object].tdm_label.font_base].width)-tdm_gui[object].width+4, 0);
		glVertex2f((tdm_gui[object].tdm_label.cursor_at*tdm_gui[tdm_gui[object].tdm_label.font_base].width)-tdm_gui[object].width+4, (float)tdm_gui[object].height);
	glEnd();
	glDisable(GL_LINE_SMOOTH);
}

int label_create(int parent, char *string, int font, int set, float left, float top, int visible, int editable){
	gui_count = gui_count+1;
	tdm_gui[gui_count].istype = 5;
	tdm_gui[gui_count].parent = parent;
	tdm_gui[gui_count].height = tdm_gui[font].height;
	tdm_gui[gui_count].width = tdm_gui[font].width*strlen(string);
	tdm_gui[gui_count].tdm_label.string = string;
	tdm_gui[gui_count].tdm_label.font_base = font;
	tdm_gui[gui_count].tdm_label.font_set = set;
	tdm_gui[gui_count].left = left;
	tdm_gui[gui_count].top = top;
	tdm_gui[gui_count].visible = visible;
	tdm_gui[gui_count].tdm_label.editable = editable;
	tdm_gui[gui_count].tdm_label.state= 0;
	tdm_gui[gui_count].tdm_label.display_chars= 0;
	return gui_count;
}

void  set_object_position(int object, float left, float top){
	tdm_gui[object].left = left;
	tdm_gui[object].top = top;
}

void set_hover_function(int object, void (*thefunction)(void)){
	 tdm_gui[object].hoverfunc = 1;
	 tdm_gui[object].hover_func = thefunction;
}

void set_depress_function(int object, void (*thefunction)(void)){
	 tdm_gui[object].depressfunc = 1;
	 tdm_gui[object].depress_func = thefunction;
}

void set_release_function(int object, void (*thefunction)(void)){
	 tdm_gui[object].releasefunc = 1;
	 tdm_gui[object].release_func = thefunction;
}

void set_deselect_function(int object, void (*thefunction)(void)){
	 tdm_gui[object].deselectfunc = 1;
	 tdm_gui[object].deselect_func = thefunction;
}

float get_object_width(int object){
	return tdm_gui[object].width;
}
float get_object_height(int object){
	return tdm_gui[object].height;
}

void gui_pass(){ /*  detects if a gui object is under the mouse and activates a state change; */
	int b, c;
	float slider_top, slider_bottom, slide_button_top;
	for(c=1;c<=gui_count;c++){
		if(tdm_gui[c].istype == 1){ 
			for(b=0;b<=gui_count;b++){
				if(tdm_gui[b].parent == c){
					if ((getmousex() >= tdm_gui[b].left+tdm_gui[c].left) && (getmousex() <= tdm_gui[b].width+tdm_gui[b].left+tdm_gui[c].left)&& (getmousey()>=tdm_gui[b].top+tdm_gui[c].top) && (getmousey()<=tdm_gui[b].top+tdm_gui[b].height+tdm_gui[c].top)){
						tdm_gui[b].over = 1;
						/* execute all the stored functions for the gu elements*/
						if(tdm_gui[b].hoverfunc==1){
							tdm_gui[b].hover_func();
						}
						if((tdm_gui[b].istype== 2)){
							if(tdm_gui[b].tdm_button.state == 0){
								tdm_gui[b].tdm_button.state = 1;
							}
						}
						if((tdm_gui[b].istype== 6)){
							if(tdm_gui[b].tdm_slider.state == 0){
								tdm_gui[b].tdm_slider.state = 1;
							}
						}
						
					}else{
						tdm_gui[b].over = 0;
						if(tdm_gui[b].istype== 2){
							if((tdm_gui[b].tdm_button.state == 1)||(tdm_gui[b].tdm_button.state == 2)){
								tdm_gui[b].tdm_button.state = 0;
							}
						}
						if(tdm_gui[b].istype== 6){
							if(tdm_gui[b].tdm_slider.state == 1){
								tdm_gui[b].tdm_slider.state = 0;
							}
						}
						
					}
					/* we can do the hover and drag */
					if(tdm_gui[b].press==1){
						/*do verticle slider drag*/
						if(tdm_gui[b].tdm_slider.vert==1){
						/* to do keep it in the same place where the mouse is when clicked */
							slider_top = tdm_gui[tdm_gui[b].tdm_slider.base_id].top;//+tdm_gui[tdm_gui[b].parent].top;
							slider_bottom = tdm_gui[tdm_gui[b].tdm_slider.base_id].top+tdm_gui[tdm_gui[b].tdm_slider.base_id].height-tdm_gui[b].height;//+tdm_gui[tdm_gui[b].parent].top
							/*printf("t %f : b %f : bt %f\n", slider_top, slider_bottom, tdm_gui[b].top);*/
							/*object_top_real = tdm_gui[tdm_gui[b].tdm_slider.base_id].top+tdm_gui[tdm_gui[b].parent].top;*/
							slide_button_top = getmousey() - (tdm_gui[tdm_gui[b].parent].top+(tdm_gui[b].mouseyrelative)); 
							
							if ((slide_button_top >=  slider_top)&&(slide_button_top <= slider_bottom)){
								tdm_gui[b].top = slide_button_top;
							}
							
						}
						if(tdm_gui[b].tdm_slider.vert==0){
						/* to do keep it in the same place where the mouse is when clicked */
							slider_top = tdm_gui[tdm_gui[b].tdm_slider.base_id].left;
							slider_bottom = tdm_gui[tdm_gui[b].tdm_slider.base_id].left+tdm_gui[tdm_gui[b].tdm_slider.base_id].width-tdm_gui[b].width;
							/*printf("t %f : b %f : bt %f\n", slider_top, slider_bottom, tdm_gui[b].top);*/
							/*object_top_real = tdm_gui[tdm_gui[b].tdm_slider.base_id].top+tdm_gui[tdm_gui[b].parent].top;*/
							slide_button_top = getmousex() - (tdm_gui[tdm_gui[b].parent].left+(tdm_gui[b].mousexrelative)); 
							
							if ((slide_button_top >=  slider_top)&&(slide_button_top <= slider_bottom)){
								tdm_gui[b].left = slide_button_top;
							}
						}
					}
				}
			}
		}
	}
}

void draw_quads(int b, int c){
	/*draw a rectangle*/
	glBegin(GL_QUADS);
		glTexCoord2f(tdm_gui[b].width, tdm_gui[b].height);  
		glVertex2f(tdm_gui[b].width, tdm_gui[b].height);
		glTexCoord2f(0, tdm_gui[b].height);  
		glVertex2f(0, tdm_gui[b].height);
		glTexCoord2f(0, 0);  
		glVertex2f(0,0);
		glTexCoord2f(tdm_gui[b].width, 0); 
		glVertex2f(tdm_gui[b].width,0);
	glEnd();	
}

void gui_show(){
	int b;
	int c;
	for(c=1;c<=gui_count;c++){
		if(tdm_gui[c].istype == 1){ /*is a container*/
			/* go to ortho view*/
			glLoadIdentity();
			glDisable(GL_DEPTH_TEST);	
			glMatrixMode(GL_PROJECTION);	
			glPushMatrix();
			glLoadIdentity();
			glOrtho(0, getwinwidth(), 0, getwinheight(),-1,5000);
			glMatrixMode(GL_MODELVIEW);	
			glColor4f(1.0, 1.0, 1.0, 1.0);		
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			/* make sure the GL_TEXTURE_RECTANGLE is turned on */
			for(b=0;b<=gui_count;b++){
				if((tdm_gui[b].parent == c)&&(tdm_gui[c].visible==1)){/* display only those in the container */
					glPushMatrix();	
					glLoadIdentity();
					glTranslatef(tdm_gui[b].left+tdm_gui[c].left, getwinheight()-(tdm_gui[b].top+tdm_gui[c].top)-tdm_gui[b].height,0);
					if((tdm_gui[b].istype== 2)&&(tdm_gui[b].visible==1)){ /* is a button */
						/*bind a texture depending on widget  identity and status*/
						
						glEnable(GL_TEXTURE_RECTANGLE_NV);
						if(tdm_gui[b].tdm_button.state==0){
							glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[b].tdm_button.off_texture);
						}
						if(tdm_gui[b].tdm_button.state==1){
							glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[b].tdm_button.hover_texture);
						}
						if((tdm_gui[b].tdm_button.state==2)||(tdm_gui[b].tdm_button.state==3)){
							glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[b].tdm_button.on_texture);
						}
						draw_quads(b, c);
						glDisable(GL_TEXTURE_RECTANGLE_NV);
					}
					if((tdm_gui[b].istype== 6)&&(tdm_gui[b].visible==1)){ /* is a slider button */
						/*bind a texture depending on widget  identity and status*/	
						glEnable(GL_TEXTURE_RECTANGLE_NV);
						if(tdm_gui[b].tdm_slider.state==0){
							glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[b].tdm_slider.off_texture);
						}
						if(tdm_gui[b].tdm_slider.state==1){
							glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[b].tdm_slider.hover_texture);
						}
						draw_quads(b, c);
						glDisable(GL_TEXTURE_RECTANGLE_NV);
					}
					if((tdm_gui[b].istype== 3)&&(tdm_gui[b].visible==1)){ /* is an image */
						glEnable(GL_COLOR_MATERIAL);
						glEnable(GL_TEXTURE_RECTANGLE_NV);
						glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[b].tdm_image.texture);
						draw_quads(b, c);
						glDisable(GL_TEXTURE_RECTANGLE_NV);
						
						glDisable(GL_COLOR_MATERIAL);
					}
					if((tdm_gui[b].istype==5)&&(tdm_gui[b].visible==1)){/* is a label */
						glEnable(GL_TEXTURE_2D);
						glEnable(GL_COLOR_MATERIAL);
						if(tdm_gui[b].over == 1){
							glColor4f(1.0, 1.0, 1.0, 1.0);		
						}
						if(tdm_gui[b].tdm_label.state == 1){
							glColor4f(1.0, 1.0, 1.0, 1.0);		
						}
						glBindTexture(GL_TEXTURE_2D, tdm_gui[tdm_gui[b].tdm_label.font_base].tdm_font.font_texture);
						glListBase(tdm_gui[tdm_gui[b].tdm_label.font_base].tdm_font.font_base - 32 + (128 * tdm_gui[b].tdm_label.font_set));
						if(tdm_gui[b].tdm_label.display_chars>0){
							char *tmp = (char *)calloc(tdm_gui[b].tdm_label.display_chars, sizeof(char));
							strncat(tmp, tdm_gui[b].tdm_label.string, tdm_gui[b].tdm_label.display_chars);
							glCallLists(strlen(tmp), GL_BYTE, tmp);
							free(tmp);
						}else{
    							glCallLists(strlen(tdm_gui[b].tdm_label.string), GL_BYTE, tdm_gui[b].tdm_label.string);
						}
						glDisable(GL_TEXTURE_2D);
						if(tdm_gui[b].tdm_label.state == 1){
							draw_label_cusor(b);							
						}
						glDisable(GL_COLOR_MATERIAL);
						glColor4f(1.0, 1.0, 1.0, 1.0);		
					}
				}
			}
			/*exit ortho*/
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();			
			glMatrixMode(GL_MODELVIEW);	
			glPopMatrix();
			glLoadIdentity();
			glEnable(GL_DEPTH_TEST);
		}
	}
}
void gui_exec(void){
	gui_pass();
	gui_show();
}
void do_button_depress(int button){
	int c;
	for(c=1;c<=gui_count;c++){
		if (tdm_gui[c].over == 1){
			if ((tdm_gui[c].visible == 1)&&(tdm_gui[tdm_gui[c].parent].visible == 1)){
			tdm_gui[c].mouseyrelative = (getmousey() - (tdm_gui[tdm_gui[c].parent].top + tdm_gui[c].top));
			tdm_gui[c].mousexrelative = (getmousex() - (tdm_gui[tdm_gui[c].parent].left + tdm_gui[c].left));
			/* do on depress call*/
			if(tdm_gui[c].depressfunc==1){
				last_object_depress = c;
				tdm_gui[c].depress_func();
			}
			if((tdm_gui[c].istype==2)&&(tdm_gui[c].tdm_button.state!=3)){
				tdm_gui[c].tdm_button.state=2;
			}
			if(tdm_gui[c].istype==6){
				tdm_gui[c].press = 1;
			}
			}
		}
	}
}
void do_button_release(int button){
	int c;
	for(c=1;c<=gui_count;c++){
		if (tdm_gui[c].over == 1){
			/* do on release call*/
			if ((tdm_gui[c].visible == 1)&&(tdm_gui[tdm_gui[c].parent].visible == 1)){
			if(tdm_gui[c].istype==2){
				if(tdm_gui[c].tdm_button.toggel==1){ 
					if(tdm_gui[c].tdm_button.state==3){
						tdm_gui[c].tdm_button.state=1;  /*release toggel call*/
					}else{
						tdm_gui[c].tdm_button.state=3;  /*enable toggel callimage*/
					}
				}else{
					tdm_gui[c].tdm_button.state=1;
				}
			}
			if(tdm_gui[c].istype==6){
				tdm_gui[c].press = 0;
			}
			if(tdm_gui[c].istype==5){
				if(tdm_gui[c].tdm_label.editable==1){
					tdm_gui[c].tdm_label.state = 1;
					set_label_cusor(c);
				}
			}
			if(tdm_gui[c].releasefunc==1){
				last_object_release=c;
				tdm_gui[c].release_func();
			}
			}
		}else{
			if ((tdm_gui[c].visible == 1)&&(tdm_gui[tdm_gui[c].parent].visible == 1)){
			if(tdm_gui[c].istype==6){
				if(tdm_gui[c].press == 1){
					tdm_gui[c].press = 0;
				}
			}
			if(tdm_gui[c].istype==5){
				if(tdm_gui[c].tdm_label.editable==1){
					if(tdm_gui[c].tdm_label.state == 1){
						tdm_gui[c].tdm_label.state = 0;
						if(tdm_gui[c].deselectfunc==1){
							tdm_gui[c].deselect_func();
							printf("deselct shit\n\n");
						}
					}
				}
			}
			}
		}
	}
}
int get_last_object_depress(void){
	return last_object_depress;
}
int get_last_object_release(void){
	return last_object_release;
}

void gui_keypress_handler(char *key){
	int c;
	for(c=1;c<=gui_count;c++){
		if(tdm_gui[c].istype==5){
			if(tdm_gui[c].tdm_label.state == 1){
				label_insert_char(key, c);
			}
		}
	}
	if (strcmp(key, "Shift_L")==0){
		keyshift = 1;
	}
	if (strcmp(key, "Shift_R")==0){
		keyshift = 1;
	}
	printf("press %s \n", key);
}
void gui_keyrelease_handler(char *key){
	if (strcmp(key, "Shift_L")==0){
		keyshift = 0;
	}
	if (strcmp(key, "Shift_R")==0){
		keyshift = 0;
	}
}

void setupTexture(GLuint *textureid, int ah, int aw){
	unsigned int* data;											
	data = (unsigned int*)malloc(sizeof(unsigned int) * 4 * ah * aw);
	
	glGenTextures(1, textureid);					
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, *textureid);			
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 4, ah, aw, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);			
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	free(data);	
}

int create_canvas(int cwidth, int cheight, float awidth, float aheight, float fov){
	gui_count = gui_count+1;
	tdm_gui[gui_count].istype = 7;
	tdm_gui[gui_count].width = cwidth;
	tdm_gui[gui_count].height = cheight;
	tdm_gui[gui_count].tdm_canvas.awidth = awidth;
	tdm_gui[gui_count].tdm_canvas.aheight = aheight;
	tdm_gui[gui_count].tdm_canvas.fov = fov;
	setupTexture(&tdm_gui[gui_count].tdm_canvas.canvas_texture, cwidth, cheight);
	printf("canvas created gui_count id:%i \n", gui_count);
	return gui_count;
}
void open_canvas(int canvas){
	/* opens canvas */
	setpbuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glViewport(0,0,(int)tdm_gui[canvas].width,(int)tdm_gui[canvas].height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(tdm_gui[canvas].tdm_canvas.fov, (GLfloat)(tdm_gui[canvas].tdm_canvas.awidth)/(GLfloat)(tdm_gui[canvas].tdm_canvas.aheight), 1, 7000);
}
void close_canvas(int canvas){
	/* close canvas copies to canvas texture*/
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[canvas].tdm_canvas.canvas_texture);
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, (int)tdm_gui[canvas].width, (int)tdm_gui[canvas].height);
	unsetpbuffer();
}
void draw_canvas(int canvas, int cx, int cy, int cwidth, int cheight){
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, getwinwidth(), 0, getwinheight(),-1,5000);
	glMatrixMode(GL_MODELVIEW);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glPushMatrix();	
	glLoadIdentity();
	glTranslatef(cx, getwinheight()-cy-cheight,0);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glColor4f(1.0, 1.0, 1.0, 1.0);		


	glBindTexture(GL_TEXTURE_RECTANGLE_NV, tdm_gui[canvas].tdm_canvas.canvas_texture);
	glBegin(GL_QUADS);
		glTexCoord2f(tdm_gui[canvas].width, tdm_gui[canvas].height);  
		glVertex2f(cwidth, cheight);
		glTexCoord2f(0, tdm_gui[canvas].height);  
		glVertex2f(0, cheight);
		glTexCoord2f(0, 0);  
		glVertex2f(0,0);
		glTexCoord2f(tdm_gui[canvas].width, 0); 
		glVertex2f(cwidth,0);
	glEnd();	
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();			
	glMatrixMode(GL_MODELVIEW);	
	glPopMatrix();
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
}

void set_object_visible(int object, int visible){
	tdm_gui[object].visible = visible;
}
int get_object_visible(int object){
	return tdm_gui[object].visible;
}

int get_object_state(int object){
	return tdm_gui[object].tdm_button.state;
}
void set_object_state(int object, int thestate){
	tdm_gui[object].tdm_button.state = thestate;
}

GLuint get_canvas_texture(int canvas){
	return tdm_gui[canvas].tdm_canvas.canvas_texture;
}

int get_canvas_width(int canvas){
	return (int)tdm_gui[canvas].width;
}

int get_canvas_height(int canvas){
	return (int)tdm_gui[canvas].height;
}
