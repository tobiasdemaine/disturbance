#include "interface.h"


int default_font;
int waveform_canvas, detector_canvas, movie_canvas, deform_canvas, final_canvas;

/* wall points */

deform_point df[1024];
deform_point current_deform;
int current_deform_point = 1;
int current_deform_corner;
int edit_switch = 0;
int deform_point_total;
int deformer_label;
GLuint distort_map;
/* VIDEO */
int video_current;

int video_current_frame;
int video_current_length;
int video_capture_label;

int next_move;
int last_move;
int this_move;
int this_frame_count;
int this_frame;
int audio_decision[3];
int video_direction[2];
int last_dmx_x;
int last_dmx_y;
int lastMS;
int next_frame = 1;

GLuint test_image_loader;
GLint test_image_loader_w, test_image_loader_h;

GLuint video_texture;
GLuint	livefeed;

int texture_direction=0;
int decision = 1;
int last_direction;
int direction = 0;
bool running = false;
int stepz;
int step;
int from_x, from_y, to_y, to_x; 

void setup_deform_points(void);
void set_video_position(int x, int y);
void swim_to_flags(int flag_1_x, int flag_1_y, int flag_2_x, int flag_2_y, int steps);

void dmx_send(int x, int y){
	int edit_switch = 0;
	//
	char dmx_string[256];
	
	if((last_dmx_x == x) && (last_dmx_y == y)){
		// only send if different
	}else{
		printf("firing dmx\n");
		sprintf(dmx_string, "lla_set_dmx --universe 0 --dmx %i,%i", x, y);
		system(dmx_string);
		
		last_dmx_x = x;
		last_dmx_y = y;
	}

}

/// visual helper
int get_video_capture_label_id(void){
	return video_capture_label;
}

void preload_videos(void){
	// load into card memory on load for higher definition
	running = false;
	int video_current = 2;
	int video_current_frame = 1;
	int video_current_length = 18;
}

void setup_interface(void){
	dmx_send(5, 5);
	int ui_container, title_label;

	waveform_canvas = create_canvas(800, 600, 16, 9, 80);
	detector_canvas = create_canvas(800, 600, 16, 9, 80);
	movie_canvas = create_canvas(800, 600, 16, 9, 80);
	deform_canvas = create_canvas(800, 600, 16, 9, 80);
	final_canvas = create_canvas(800, 600, 16, 9, 80);
	default_font = font_create("interface/font.tga");


	glGenTextures(1, &livefeed);
	
	ui_container = container_create(0, 0, geteditorwidth()/2, getwinheight(), 1);
	title_label = label_create(ui_container, "Disturbance", default_font, 0, 10, 10, 1, 1);
	//video_capture_label = label_create(ui_container, "Video Capture", default_font, getwinheight()/2, geteditorwidth()/2+10, 10, 1, 1);
	deformer_label = label_create(ui_container, "set points", default_font, 0, geteditorwidth()/2+20, getwinheight()/2+10, 1, 1);
	set_object_visible(deformer_label, 0);
	preload_videos();
	printf("%i\n", distort_map);
	distort_map = load_image("interface/key.tga", 0);
	printf("%i\n", distort_map);

	setup_deform_points();
}

int time_wait=100;
int vmax=73;
void ai(void){
	int i;
	for(i=0;i<100;i++){
		if(get_fft_value(i) >= 100){
			audio_decision[1]++;
		}
		if(get_fft_value(i+200) >= 100){
			audio_decision[2]++;
		}
		if(get_fft_value(i+400) >= 100){
			audio_decision[3]++;
		}
	}
	int size = (int)(get_buffer_width() * get_buffer_height() * 2);
	GLubyte *pixels = new  GLubyte[ size ];
	// get the hotspot position
	/*
		
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, livefeed);
	capture_video();
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGB, get_buffer_width(), get_buffer_height(), 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, get_buffer_map());
	
	pixels = get_buffer_map();
 	int left, right;
	left = 0;
	right = 0;
	for( int p = 0; p < size; p++){
		for (int i = 0; i < (get_buffer_width()*2); i++) {
			if(i < get_buffer_width()){
				left = left + ((int)pixels[p + i] * (int)pixels[p + i+1]);
			}else{
				right = right + (int)pixels[p + i] * (int)pixels[p + i+1] ;
			}	
			i++;
		}
		p = p + get_buffer_width()*2;
		
	}
	*/

	/// AI is here
	if(running == false){
		
		// audio decision
		if((audio_decision[1] == audio_decision[2]) && (audio_decision[2] == audio_decision[3])){
			decision = 0; // calm in one spot
		}else if((audio_decision[1] < audio_decision[2]) && (audio_decision[2] > audio_decision[3])){
			decision = 1; // slightly agitated
		}else if((audio_decision[1] > audio_decision[2]) && (audio_decision[2] > audio_decision[3])){
			decision = 2; // move fast
		}else if((audio_decision[1] < audio_decision[3]) && (audio_decision[2] < audio_decision[3])){
			decision = 3; // move slowly
		}
		if(video_direction[1] > video_direction[1]){
		 	direction = 0;
		}else{
			direction = 1;
		}
		decision = rand() % 4 + 1;
		int vr = rand() % 2 + 1;
		int r = rand() % 25 + 1;
		
		running = true;
		video_current_frame = 1;
		video_current=vr;
		current_deform.tl_x = df[r].tl_x;
		current_deform.tl_y = df[r].tl_y;
		current_deform.tr_x = df[r].tr_x;
		current_deform.tr_y = df[r].tr_y;
		current_deform.bl_x = df[r].bl_x;
		current_deform.bl_y = df[r].bl_y;
		current_deform.br_x = df[r].br_x;
		current_deform.br_y = df[r].br_y;
		current_deform.dmx_x = df[r].dmx_x;
		current_deform.dmx_y = df[r].dmx_y;
		dmx_send(current_deform.dmx_x, current_deform.dmx_y);
		step=0;
		printf("decision = %i; direction =%i;", decision, direction);
		if(decision == 4){
			time_wait=1;
		}
		if(decision == 1){
			time_wait=40;
		}	
		if(decision == 2){
			time_wait=30;
		}
		if(decision == 3){
			time_wait=100;
		}
	}else{
		if(video_current_frame==vmax){
			step++;
			
			
			if(step==time_wait){
				running = false;
				video_current_frame = 1;
				last_direction = direction;
				
			}
		}else{
			video_current_frame++;	
		}
	}
	 
	
}

void swim_to_flags(int flag_1_x, int flag_1_y, int flag_2_x, int flag_2_y, int steps){
	double x_dist, y_dist;
	//printf("%i-%i=%i/%i = %f \n", flag_1_x, flag_2_x, (flag_1_x - flag_2_x), steps, ((float)flag_1_x - (float)flag_2_x) / (float)steps);
	x_dist = ((float)flag_1_x - (float)flag_2_x) / (steps);
	y_dist = ((float)flag_1_y - (float)flag_2_y) / (steps);
	//printf("x_dist=%f\n", x_dist);
	//printf("y_dist=%f\n", y_dist);
	//printf("step=%i\n", step);
	//printf("x_dist*step=%f\n", x_dist*step);
	//printf("rint(x_dist*step)=%i\n", rint(x_dist));
	set_video_position(current_deform.dmx_x + rint(x_dist),current_deform.dmx_y + rint(y_dist));
} 



void setup_deform_points(void){
	
	deform_point_total = 25;
	int i;
	i = 0;
	FILE *stream;
	char oneline[255];
	if((stream = fopen("points.db", "rt" )) != NULL){
		
		while( !feof(stream ) ){   
		i++;		
			rereadstr(stream, oneline);
			printf("%i . %s\n", i, oneline);
			sscanf(oneline, "%f,%f,%f,%f,%f,%f,%f,%f,%i,%i", &df[i].tl_x, &df[i].tl_y, &df[i].tr_x, &df[i].tr_y, &df[i].bl_x, &df[i].bl_y, &df[i].br_x, &df[i].br_y, &df[i].dmx_x, &df[i].dmx_y);
			
		}				
		fclose(stream);
		
		//deform_point_total = i;
	}else{
		for(i=1;i<=1024;i++){
			df[i].tl_x = get_canvas_width(movie_canvas);
			df[i].tl_y = get_canvas_height(movie_canvas);
			df[i].tr_x = 0.00;
			df[i].tr_y = get_canvas_height(movie_canvas);
			df[i].bl_x = get_canvas_width(movie_canvas);;
			df[i].bl_y = 0.00;
			df[i].br_x = 0.00;
			df[i].br_y = 0.00;
			df[i].dmx_x = 125;
			df[i].dmx_y = 125;
		}	
	}

	current_deform.tl_x = df[1].tl_x;
	current_deform.tl_y = df[1].tl_y;
	current_deform.tr_x = df[1].tr_x;
	current_deform.tr_y = df[1].tr_y;
	current_deform.bl_x = df[1].bl_x;
	current_deform.bl_y = df[1].bl_y;
	current_deform.br_x = df[1].br_x;
	current_deform.br_y = df[1].br_y;
	current_deform.dmx_x = 120;//df[1].dmx_x;
	current_deform.dmx_y = 120;//df[1].dmx_y;
}

void save_points(void){

	 FILE *fp;
	int i;
	if((fp=fopen("points.db", "wb"))==NULL) {
		printf("Cannot open file.\n");
	}else{
		printf("Writing file.\n");
		for(i=1;i<=deform_point_total;i++){
			fprintf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%i,%i\n", df[i].tl_x, df[i].tl_y, df[i].tr_x, df[i].tr_y, df[i].bl_x, df[i].bl_y, df[i].br_x, df[i].br_y, df[i].dmx_x, df[i].dmx_y);
		}
		fclose(fp);
	}
	
}

void set_point(int deform_point_id, int deform_corner_id, float x, float y){ 
	if(deform_corner_id == 1){
		printf("corner 1\n");
		df[deform_point_id].tl_x = df[deform_point_id].tl_x + x;
		df[deform_point_id].tl_y = df[deform_point_id].tl_y + y;
	}
	if(deform_corner_id == 2){
		printf("corner 2\n");
		df[deform_point_id].tr_x = df[deform_point_id].tr_x + x;
		df[deform_point_id].tr_y = df[deform_point_id].tr_y + y;
	}
	if(deform_corner_id == 3){
		printf("corner 3\n");
		df[deform_point_id].br_x = df[deform_point_id].br_x + x;
		df[deform_point_id].br_y = df[deform_point_id].br_y + y;
	}
	if(deform_corner_id == 4){
		printf("corner 4\n");
		df[deform_point_id].bl_x = df[deform_point_id].bl_x + x;
		df[deform_point_id].bl_y = df[deform_point_id].bl_y + y;
	}
	if(deform_corner_id == 5){ //dmx
	
		df[deform_point_id].dmx_x = df[deform_point_id].dmx_x + x;
		df[deform_point_id].dmx_y = df[deform_point_id].dmx_y + y;
		if(df[deform_point_id].dmx_x < 0){
			df[deform_point_id].dmx_x = 0;
		}
		if(df[deform_point_id].dmx_x >255){
			df[deform_point_id].dmx_x = 255;
		}
		if(df[deform_point_id].dmx_y < 0){
			df[deform_point_id].dmx_y = 0;
		}
		if(df[deform_point_id].dmx_y >255){
			df[deform_point_id].dmx_y = 255;
		}
	}
	current_deform.tl_x = df[deform_point_id].tl_x;
	current_deform.tl_y = df[deform_point_id].tl_y;
	current_deform.tr_x = df[deform_point_id].tr_x;
	current_deform.tr_y = df[deform_point_id].tr_y;
	current_deform.br_x = df[deform_point_id].br_x;
	current_deform.br_y = df[deform_point_id].br_y;
	current_deform.bl_x = df[deform_point_id].bl_x;
	current_deform.bl_y = df[deform_point_id].bl_y;
	current_deform.dmx_x = df[deform_point_id].dmx_x;
	current_deform.dmx_y = df[deform_point_id].dmx_y;
	//printf("%i, %i\n", current_deform.dmx_x, current_deform.dmx_y);
	dmx_send(current_deform.dmx_x, current_deform.dmx_y);
	
}




void edit_mode(char *key){
	if (strcmp(key, "q")==0){
		decision = 0;
	}
	if (strcmp(key, "w")==0){
		decision = 1;
	}
	if (strcmp(key, "e")==0){
		decision = 2;
	}
	if (strcmp(key, "r")==0){
		decision = 3;
	}
	if (strcmp(key, "o")==0){
		direction = 0;
	}
	if (strcmp(key, "p")==0){
		direction = 1;
	}
	if (strcmp(key, "Shift_L")==0){
		if(edit_switch == 0){
			edit_switch = 1;		
			set_label_text(deformer_label, "edit d.p 1:1");
			set_object_visible(deformer_label, 1);
			current_deform_point = 1;
			current_deform_corner = 1;
			current_deform.tl_x = df[current_deform_point].tl_x;
			current_deform.tl_y = df[current_deform_point].tl_y;
			current_deform.tr_x = df[current_deform_point].tr_x;
			current_deform.tr_y = df[current_deform_point].tr_y;
			current_deform.br_x = df[current_deform_point].br_x;
			current_deform.br_y = df[current_deform_point].br_y;
			current_deform.bl_x = df[current_deform_point].bl_x;
			current_deform.bl_y = df[current_deform_point].bl_y;
			current_deform.dmx_x = df[current_deform_point].dmx_x;
			current_deform.dmx_y = df[current_deform_point].dmx_y;
			printf("%i, %i\n", current_deform.dmx_x, current_deform.dmx_y);
			dmx_send(current_deform.dmx_x, current_deform.dmx_y);
		}else{
			edit_switch = 0;
			set_object_visible(deformer_label,0);
		}
	}
	if(edit_switch == 1){
		if (strcmp(key, "equal")==0){
			if(current_deform_point==25){
				printf("xxxxxx %i\n", current_deform_point);
				current_deform_point=0;
			}
			current_deform_point++;
			//char *my_text=(char *)calloc(5 , sizeof(char));
			char my_text[255];
			sprintf(my_text, "edit d.p %i:1", current_deform_point);
			set_label_text(deformer_label, my_text);
			current_deform_corner = 1;
			printf("current_deform_point %i\n", current_deform_point);
			set_point(current_deform_point, current_deform_corner, 0, 0);
		}
		if (strcmp(key, "1")==0){
			printf("current_deform_point %i\n", current_deform_point);
			//char *my_text=(char *)calloc(5 , sizeof(char));
			char my_text[255];
			sprintf(my_text, "edit d.p %i:1", current_deform_point);
			set_label_text(deformer_label, my_text);
			current_deform_corner = 1;
			set_point(current_deform_point, current_deform_corner, 0, 0);
		}
		if (strcmp(key, "2")==0){
			//char *my_text=(char *)calloc(5 , sizeof(char));
			char my_text[255];
			sprintf(my_text, "edit d.p %i:2", current_deform_point);
			set_label_text(deformer_label, my_text);
			current_deform_corner = 2;
			set_point(current_deform_point, current_deform_corner, 0, 0);
		}
		if (strcmp(key, "3")==0){
			//char *my_text=(char *)calloc(5 , sizeof(char));
			char my_text[255];
			sprintf(my_text, "edit d.p %i:3", current_deform_point);
			set_label_text(deformer_label, my_text);
			current_deform_corner = 3;
			set_point(current_deform_point, current_deform_corner, 0, 0);
		}
		if (strcmp(key, "4")==0){
			//char *my_text=(char *)calloc(5 , sizeof(char));
			char my_text[255];
			sprintf(my_text, "edit d.p %i:4", current_deform_point);
			set_label_text(deformer_label, my_text);
			current_deform_corner = 4;
			set_point(current_deform_point, current_deform_corner, 0, 0);
		}
		if (strcmp(key, "5")==0){
			//char *my_text=(char *)calloc(5 , sizeof(char));
			char my_text[255];
			sprintf(my_text, "edit d.p %i:dmx", current_deform_point);
			set_label_text(deformer_label, my_text);
			current_deform_corner = 5;
			set_point(current_deform_point, current_deform_corner, 0, 0);
		}
		if (strcmp(key, "Up")==0){
			set_point(current_deform_point, current_deform_corner, 0, 1);
		}
		if (strcmp(key, "Down")==0){
			set_point(current_deform_point, current_deform_corner, 0, 0-1);
		}
		if (strcmp(key, "Left")==0){
			set_point(current_deform_point, current_deform_corner, 0-1, 0);
		}
		if (strcmp(key, "Right")==0){
			set_point(current_deform_point, current_deform_corner, 1, 0);
		}
		if (strcmp(key, "s")==0){
			save_points();
		}
		
	}
	
	
}
int get_top_left_corner_id(void){
	int _x, _y, _i;
	for(int i=1;i<=deform_point_total;i++){
		if(df[i].dmx_x <= _x){
			if(df[i].dmx_y <= _y){
				_i = i;
				_x = df[i].dmx_x;
				_y = df[i].dmx_y;
			}
		}
	}
	return _i ;
}
int get_bottom_left_corner_id(void){
	int _x, _y, _i;
	for(int i=1;i<=deform_point_total;i++){
		if(df[i].dmx_x <= _x){
			if(df[i].dmx_y >= _y){
				_i = i;
				_x = df[i].dmx_x;
				_y = df[i].dmx_y;
			}
		}
	}
	return _i ;
}
int get_top_right_corner_id(void){
	int _x, _y, _i;
	for(int i=1;i<=deform_point_total;i++){
		if(df[i].dmx_x >= _x){
			if(df[i].dmx_y <= _y){
				_i = i;
				_x = df[i].dmx_x;
				_y = df[i].dmx_y;
			}
		}
	}
	return _i ;
}
int get_bottom_right_corner_id(void){
	int _x, _y, _i;
	for(int i=1;i<=deform_point_total;i++){
		if(df[i].dmx_x <= _x){
			if(df[i].dmx_y >= _y){
				_i = i;
				_x = df[i].dmx_x;
				_y = df[i].dmx_y;
			}
		}
	}
	return _i ;
}	

/*
int get_nearest_vector_from_corner(int corner){
	int i;
	int last_point = 0;
	float last_point_distance = 10000.00;
	float point_distance = 0.00;
	float x_dist, y_dist;
	for(i=1;i<=deform_point_total;i++){
		if(df[i].dmx_x == current_deform.dmx_x){
			if(df[i].dmx_y == current_deform.dmx_y){
				last_point = i;
				break;
			}
		}
		if(corner == 1){
			if(df[i].dmx_x <= current_deform.dmx_x){
				if(df[i].dmx_y >= current_deform.dmx_y){
					x_dist = current_deform.dmx_x - df[i].dmx_x;
					y_dist = current_deform.dmx_y - df[i].dmx_y;
					point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					if(point_distance <= last_point_distance){
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
		if(corner == 2){
			if(df[i].dmx_x >= current_deform.dmx_x){
				if(df[i].dmx_y >= current_deform.dmx_y){
					x_dist = current_deform.dmx_x - df[i].dmx_x;
					y_dist = current_deform.dmx_y - df[i].dmx_y;
					point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					if(point_distance <= last_point_distance){
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
		if(corner == 3){
			
			//printf("..\ndf[i].dmx_x %i current_deform.dmx_x %i\n", df[i].dmx_x, current_deform.dmx_x);
			//printf("df[i].dmx_y %i current_deform.dmx_y %i\n", df[i].dmx_y, current_deform.dmx_y);
			if(df[i].dmx_x >= current_deform.dmx_x){
				if(df[i].dmx_y <= current_deform.dmx_y){
					x_dist = current_deform.dmx_x - df[i].dmx_x;
					y_dist = current_deform.dmx_y - df[i].dmx_y;
					point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					//printf("%i point_distance %f\n", i, point_distance);
					//printf("%i last_point_distance %f\n", i, last_point_distance);	
					if(point_distance <= last_point_distance){
						//printf("less_distance %f\n", point_distance);
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
		if(corner == 4){
			if(df[i].dmx_x <= current_deform.dmx_x){
				if(df[i].dmx_y <= current_deform.dmx_y){
					x_dist = current_deform.dmx_x - df[i].dmx_x;
					y_dist = current_deform.dmx_y - df[i].dmx_y;
					point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					if(point_distance <= last_point_distance){
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
	}
	return last_point;
}*/
int get_nearest_vector_from_corner(int corner){
	int i;
	int last_point = 0;
	float last_point_distance = 10000.00;
	float point_distance = 0.00;
	float x_dist, y_dist;
	for(i=1;i<=deform_point_total;i++){
		if(df[i].dmx_x == current_deform.dmx_x){
			if(df[i].dmx_y == current_deform.dmx_y){
				last_point = i;
				break;
			}
		}
		if(corner == 1){
			if(df[i].dmx_x <= current_deform.dmx_x){
				if(df[i].dmx_y >= current_deform.dmx_y){
					//x_dist = current_deform.dmx_x - df[i].dmx_x;
					//y_dist = current_deform.dmx_y - df[i].dmx_y;
					//point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					point_distance = current_deform.dmx_x - df[i].dmx_x;
					if(point_distance <= last_point_distance){
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
		if(corner == 2){
			if(df[i].dmx_x >= current_deform.dmx_x){
				if(df[i].dmx_y >= current_deform.dmx_y){
					//x_dist = current_deform.dmx_x - df[i].dmx_x;
					//y_dist = current_deform.dmx_y - df[i].dmx_y;
					//point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					point_distance = current_deform.dmx_x - df[i].dmx_x;
					if(point_distance <= last_point_distance){
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
		if(corner == 3){
			
			//printf("..\ndf[i].dmx_x %i current_deform.dmx_x %i\n", df[i].dmx_x, current_deform.dmx_x);
			//printf("df[i].dmx_y %i current_deform.dmx_y %i\n", df[i].dmx_y, current_deform.dmx_y);
			if(df[i].dmx_x >= current_deform.dmx_x){
				if(df[i].dmx_y <= current_deform.dmx_y){
					//x_dist = current_deform.dmx_x - df[i].dmx_x;
					//y_dist = current_deform.dmx_y - df[i].dmx_y;
					//point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));

					point_distance = current_deform.dmx_x - df[i].dmx_x;
					//printf("%i point_distance %f\n", i, point_distance);
					//printf("%i last_point_distance %f\n", i, last_point_distance);	
					if(point_distance <= last_point_distance){
						//printf("less_distance %f\n", point_distance);
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
		if(corner == 4){
			if(df[i].dmx_x <= current_deform.dmx_x){
				if(df[i].dmx_y <= current_deform.dmx_y){
					//x_dist = current_deform.dmx_x - df[i].dmx_x;
					//y_dist = current_deform.dmx_y - df[i].dmx_y;
					//point_distance = sqrt(((x_dist*x_dist)+(y_dist*y_dist)));
					point_distance = current_deform.dmx_x - df[i].dmx_x;
					if(point_distance <= last_point_distance){
						last_point = i;
					}
					last_point_distance = point_distance;
				}
			}
		}
	}
	return last_point;
}



void set_video_position(int x, int y){
	printf("x %i, y %i\n", x, y);
	if((last_dmx_x == x) && (last_dmx_y == y)){
		// if same we don't do anything
	}else{
		int tl, tr, bl, br;
		int id;
		current_deform.dmx_x = x;
		current_deform.dmx_y = y;
		for(int i=1;i<=12;i++){
			if(x >= last_dmx_x){
				if(y >= last_dmx_y){
					id = get_nearest_vector_from_corner(3);
				}else{
					id = get_nearest_vector_from_corner(1);
				}
			}else{
				if(y >= last_dmx_y){
					id = get_nearest_vector_from_corner(4);
				}else{
					id = get_nearest_vector_from_corner(2);
				}
			}
		}
		if((df[id].dmx_x - current_deform.dmx_x) !=0){
			current_deform.tl_x += (current_deform.tl_x-df[id].tl_x)/(df[id].dmx_x - current_deform.dmx_x) ;
			current_deform.tr_x += (current_deform.tr_x-df[id].tr_x)/(df[id].dmx_x - current_deform.dmx_x) ;
			current_deform.bl_x += (current_deform.bl_x-df[id].bl_x)/(df[id].dmx_x - current_deform.dmx_x) ;
			current_deform.br_x += (current_deform.br_x-df[id].br_x)/(df[id].dmx_x - current_deform.dmx_x) ;
		}
		if((df[id].dmx_y - current_deform.dmx_y) !=0){
			current_deform.tl_y += (current_deform.tl_y-df[id].tl_y)/(df[id].dmx_y - current_deform.dmx_y) ;
			current_deform.tr_y += (current_deform.tr_y-df[id].tr_y)/(df[id].dmx_y - current_deform.dmx_y) ;
			current_deform.bl_y += (current_deform.bl_y-df[id].bl_y)/(df[id].dmx_y - current_deform.dmx_y) ;
			current_deform.br_y += (current_deform.br_y-df[id].br_y)/(df[id].dmx_y - current_deform.dmx_y);
		}
		printf("df[id].dmx_x %i\n", df[id].dmx_x);
		printf("id = %i; current_deform.tl_x = %f df[id].tl_x=%f \n", id, current_deform.tl_x, df[id].tl_x);
		/*

		tl = get_nearest_vector_from_corner(1);
		tr = get_nearest_vector_from_corner(2);
		br = get_nearest_vector_from_corner(3);
		bl = get_nearest_vector_from_corner(4);	
		

		printf("tl %i; tr %i; br %i; bl %i\n", tl, tr, br, bl);
		//printf("df[tl].tl_x + ((df[tl].dmx_x - current_deform.dmx_x) * ((df[tl].tl_x - df[tr].tl_x) / (()df[tl].dmx_x - df[tr].dmx_x)));\n");
		//printf("%f + ((%i) * ((%f) / (%i)));\n", df[tl].tl_x, df[tl].dmx_x - current_deform.dmx_x, df[tl].tl_x - df[tr].tl_x, df[tl].dmx_x - df[tr].dmx_x);
		//tl
		//printf("before current_deform.tl_x %f\n", current_deform.tl_x);
		if((df[tl].dmx_x - df[tr].dmx_x) != 0){
		current_deform.tl_x = df[tl].tl_x + ((df[tl].dmx_x - current_deform.dmx_x) * ((df[tl].tl_x - df[tr].tl_x) / (df[tl].dmx_x - df[tr].dmx_x)));
		}
		//printf("after current_deform.tl_x %f\n", current_deform.tl_x);
		if((df[tl].dmx_y - df[bl].dmx_y) != 0){		
		current_deform.tl_y = df[tl].tl_y + ((df[tl].dmx_y - current_deform.dmx_y) * ((df[tl].tl_y - df[bl].tl_y) / (df[tl].dmx_y - df[bl].dmx_y)));
		}



		//tr
		if((df[tl].dmx_y - df[bl].dmx_y) != 0){
		current_deform.tr_x = df[tl].tr_x + ((df[tr].dmx_x - current_deform.dmx_x) * ((df[tl].tr_x - df[tr].tr_x) / (df[tl].dmx_x - df[tr].dmx_x)));
		}
		if((df[tr].dmx_y - df[br].dmx_y) != 0){
		current_deform.tr_y = df[tr].tr_y + ((df[tr].dmx_y - current_deform.dmx_y) * ((df[tr].tr_y - df[br].tr_y) / (df[tr].dmx_y - df[br].dmx_y)));
		}		
		//br
		if((df[bl].dmx_x - df[br].dmx_x) != 0){
		current_deform.br_x = df[bl].br_x + ((df[bl].dmx_x - current_deform.dmx_x) * ((df[bl].br_x - df[br].br_x) / (df[bl].dmx_x - df[br].dmx_x)));
		}
		if((df[tr].dmx_y - df[br].dmx_y) != 0){
		current_deform.br_y = df[tr].br_y + ((df[tl].dmx_y - current_deform.dmx_y) * ((df[tr].br_y - df[br].br_y) / (df[tr].dmx_y - df[br].dmx_y)));
		}
		//bl
		if((df[bl].dmx_x - df[br].dmx_x) != 0){	
		current_deform.bl_x = df[bl].bl_x + ((df[bl].dmx_x - current_deform.dmx_x) * ((df[bl].bl_x - df[br].bl_x) / (df[bl].dmx_x - df[br].dmx_x)));
		}
		if((df[tl].dmx_y - df[bl].dmx_y) != 0){
		current_deform.bl_y = df[tl].bl_y + ((df[tl].dmx_y - current_deform.dmx_y) * ((df[tl].bl_y - df[bl].bl_y) / (df[tl].dmx_y - df[bl].dmx_y)));
		}
		//printf("current_deform.tl_x = %i\n", current_deform.tl_x);
		*/
		
	}
	
	
	last_dmx_x = x;
	last_dmx_y = y;
	
}
void render_all(void){
	if(edit_switch != 1){
		ai();
	}	

	open_canvas(waveform_canvas);
	use_inbuilt_camera_path(5);
	glColor4f(1.0, 0.0, 0.0, 1.0);	
	draw_oscilliscope_4_3d();
	close_canvas(waveform_canvas);

	open_canvas(detector_canvas);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, get_canvas_width(movie_canvas), 0, get_canvas_height(movie_canvas),-1,5000);
	glMatrixMode(GL_MODELVIEW);
	glColor4f(1.0, 1.0, 1.0, 1.0);	
	glDisable(GL_COLOR_MATERIAL);	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
	glEnable(GL_BLEND);
	glPushMatrix();	
	glLoadIdentity();
//	glTranslatef(0, getwinheight()-600,0);
	glEnable(GL_TEXTURE_RECTANGLE_NV);

	glBindTexture(GL_TEXTURE_RECTANGLE_NV, livefeed	);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_WIDTH, &test_image_loader_w);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_HEIGHT, &test_image_loader_h);
	
	
	//glBindTexture(GL_TEXTURE_RECTANGLE_NV, av_library[media_count].frame_texture);
	glBegin(GL_QUADS);
		glTexCoord2f(test_image_loader_w, test_image_loader_h);  
		glVertex2f(get_canvas_width(movie_canvas),get_canvas_height(movie_canvas));
		glTexCoord2f(0, test_image_loader_h);  
		glVertex2f(0, get_canvas_height(movie_canvas));		
		glTexCoord2f(0, 0);  
		glVertex2f(0, 0);
		glTexCoord2f(test_image_loader_w, 0);
		glVertex2f(get_canvas_width(movie_canvas), 0); 
		
	glEnd();

	
	glColor4f(1.0, 1.0, 1.0, 1.0);		
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();			
	glMatrixMode(GL_MODELVIEW);	
	glPopMatrix();
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	close_canvas(detector_canvas);

	open_canvas(movie_canvas);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, get_canvas_width(movie_canvas), 0, get_canvas_height(movie_canvas),-1,5000);
	glMatrixMode(GL_MODELVIEW);
	glColor4f(1.0, 1.0, 1.0, 1.0);		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   /* will have to make sure the blend is set externally */
	glEnable(GL_BLEND);
	glPushMatrix();	
	glLoadIdentity();
	glDisable(GL_COLOR_MATERIAL);
//	glTranslatef(0, getwinheight()-600,0);
	glEnable(GL_TEXTURE_RECTANGLE_NV);

	glDeleteTextures( 1, &test_image_loader);


	char my_file[128];
	
	
	sprintf(my_file, "video/%i/fish1%i.tga", video_current, video_current_frame);
	test_image_loader = load_image(my_file, 0);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, test_image_loader);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_WIDTH, &test_image_loader_w);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_HEIGHT, &test_image_loader_h);
	
	//glBindTexture(GL_TEXTURE_RECTANGLE_NV, av_library[media_count].frame_texture);
	glBegin(GL_QUADS);
		
		glTexCoord2f(test_image_loader_w, test_image_loader_h);  
		glVertex2f(get_canvas_width(movie_canvas),get_canvas_height(movie_canvas));
		glTexCoord2f(0, test_image_loader_h);  
		glVertex2f(0, get_canvas_height(movie_canvas));		
		glTexCoord2f(0, 0);  
		glVertex2f(0, 0);
		glTexCoord2f(test_image_loader_w, 0);
		glVertex2f(get_canvas_width(movie_canvas), 0); 
		
	glEnd();

	
	glColor4f(1.0, 1.0, 1.0, 1.0);		
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();			
	glMatrixMode(GL_MODELVIEW);	
	glPopMatrix();
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	close_canvas(movie_canvas);

	open_canvas(deform_canvas);
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	
	glLoadIdentity();
	
	glDisable(GL_DEPTH_TEST);	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, get_canvas_width(movie_canvas), 0, get_canvas_height(movie_canvas),-1,5000);
	glMatrixMode(GL_MODELVIEW);
	//
		
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   
	glEnable(GL_BLEND);
	glPushMatrix();	
	glLoadIdentity();
	//glTranslatef(0, getwinheight()-600,0);
	if(edit_switch == 1){
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, distort_map);
	}
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_WIDTH, &test_image_loader_w);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_HEIGHT, &test_image_loader_h);
	//if(video_current_frame!=vmax){
		//glBindTexture(GL_TEXTURE_RECTANGLE_NV, av_library[media_count].frame_texture);
		glColor4f(1.0, 1.0, 1.0, 1.0);		
		glBegin(GL_QUADS);
			if(last_direction == 0){
				glTexCoord2f(test_image_loader_w, test_image_loader_h);  
				glVertex2f(current_deform.tl_x, current_deform.tl_y);
				glTexCoord2f(0, test_image_loader_h);  
				glVertex2f(current_deform.tr_x, current_deform.tr_y);
				glTexCoord2f(0, 0);  
				glVertex2f(current_deform.br_x, current_deform.br_y); 
				glTexCoord2f(test_image_loader_w, 0);
				glVertex2f(current_deform.bl_x, current_deform.bl_y);
			}else{
				glTexCoord2f(0, test_image_loader_h);  
				glVertex2f(current_deform.tl_x, current_deform.tl_y);
				glTexCoord2f(test_image_loader_w, test_image_loader_h);
				glVertex2f(current_deform.tr_x, current_deform.tr_y);
				glTexCoord2f(test_image_loader_w, 0);			 
				glVertex2f(current_deform.br_x, current_deform.br_y); 
				glTexCoord2f(0, 0);  			
				glVertex2f(current_deform.bl_x, current_deform.bl_y);
			}
		
		glEnd();
	//}
	
	
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glDisable(GL_BLEND);
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();			
	glMatrixMode(GL_MODELVIEW);	
	glPopMatrix();
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	close_canvas(deform_canvas);

	

	draw_canvas(waveform_canvas, 0, 0, geteditorwidth()/2, getwinheight()/2);
	draw_canvas(movie_canvas, 0, getwinheight()/2,geteditorwidth()/2, getwinheight()/2);
	draw_canvas(deform_canvas, geteditorwidth()/2, getwinheight()/2, geteditorwidth()/2, getwinheight()/2);
	draw_canvas(detector_canvas, geteditorwidth()/2, 0, geteditorwidth()/2, getwinheight()/2);
	draw_canvas(deform_canvas, 1280, 0, 1024, 768);
	
}

	




/* video for linux */
