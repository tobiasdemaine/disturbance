#include "video.h"

int        v_thr_id;   
pthread_t  v_p_thread;

mediastream av_library[10000]; /* 10000 possible media clips*/
int media_count=0;

GLubyte *map;
unsigned int ioffset;
int 	fd = -1;
struct 	video_mmap my_buf;
struct	video_capability  capability;
struct video_channel vid_chnl;
struct video_picture vid_pict;
struct video_window vid_win;
int channel = 1;

int stream_create(char *file){  /* we will start a thread here */
	media_count++;
	av_library[media_count].id = avm::CreateReadFile(file);
	av_library[media_count].current_frame=0;
	av_library[media_count].speed=1;
	av_library[media_count].avistream = av_library[media_count].id->GetStream(0, avm::IReadStream::Video );
	glGenTextures(1, &av_library[media_count].frame_texture);
        return media_count; 
}
void stream_play(int stream){
	av_library[stream].avistream->StartStreaming();
	av_library[stream].avistream->Seek(av_library[stream].current_frame);
	av_library[stream].streaminfo = av_library[stream].avistream->GetStreamInfo();
	av_library[stream].width = av_library[stream].streaminfo->GetVideoWidth();
	av_library[stream].height = av_library[stream].streaminfo->GetVideoHeight();
	av_library[stream].stream_fps = av_library[stream].streaminfo->GetFps();
}

void stream_stop(int stream){
	av_library[stream].avistream->StopStreaming();
}

void stream_seek(int stream, int frame){
	av_library[stream].current_frame = frame;
}

void stream_rewind(int stream){
	/*av_library[stream].avistream->Seek(0);*/
	av_library[stream].next=0;
	stream_get_frame(stream);
	/*stream_play(stream);*/
}

void stream_rewind_stop(int stream){
	/*av_library[stream].avistream->Seek(0);*/
	av_library[stream].next=0;
	stream_get_frame(stream);
	av_library[stream].avistream->StopStreaming();
}

void stream_set_speed(int stream,int speed){
	av_library[stream].speed = speed;
}

int stream_is_streaming(int stream){
	if(av_library[stream].avistream->IsStreaming()){
		return 1;
	}else{
		return 0;
	}
}
int stream_get_position(int stream){
	return av_library[stream].current_frame;
}
int stream_get_speed(int stream){
	return av_library[stream].speed;
}

void stream_get_frame(int stream){
	/* we do some fancy timer stuff here */
	if(av_library[stream].avistream->IsStreaming()){
		av_library[stream].next+= getMS();	
		av_library[stream].current_frame=int(av_library[stream].next/((int)(1000/av_library[stream].speed)*av_library[stream].avistream->GetFrameTime()));
		av_library[stream].avistream->Seek(av_library[stream].current_frame);
		CImage* vimage = av_library[stream].avistream->GetFrame(true);
		if (!av_library[stream].avistream->Eof() && vimage) {
			av_library[stream].current_frame = av_library[stream].avistream->GetPos();
			CImage* rgbci = new CImage(vimage, 24);
			glBindTexture(GL_TEXTURE_RECTANGLE_NV, av_library[stream].frame_texture);  
			printf("%i", rgbci->At(0,0));
			glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA, av_library[stream].width, av_library[stream].height, 0,  GL_BGR, GL_UNSIGNED_BYTE,  rgbci->At(0,0));
			rgbci->Release();
			vimage->Release();
		} else {
			av_library[stream].avistream->Seek(0);  /* back to start to stream */
			av_library[stream].next=0;
		}
	}
}

void stream_set_rgba(int stream, float r, float g, float b, float a){
	av_library[stream].r = r;
	av_library[stream].g = g;
	av_library[stream].b = b;
	av_library[stream].a = a;
}

void stream_display(int stream, int swidth, int sheight, int sx, int sy){
	stream_get_frame(stream);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);	
	glMatrixMode(GL_PROJECTION);	
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, getwinwidth(), 0, getwinheight(),-1,5000);
	glMatrixMode(GL_MODELVIEW);
	//glColor4f(av_library[stream].r, av_library[stream].g, av_library[stream].b, av_library[stream].a);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);   /* will have to make sure the blend is set externally */
	glEnable(GL_BLEND);
	glPushMatrix();	
	glLoadIdentity();
	//glTranslatef(sx, getwinheight()-sy-sheight,0);
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, av_library[stream].frame_texture);
	glBegin(GL_QUADS);
		printf("%i\n", av_library[stream].frame_texture);
		glTexCoord2f(av_library[stream].width, av_library[stream].height);  
		glVertex2f(swidth,0);
		glTexCoord2f(0, av_library[stream].height);  
		glVertex2f(0,0);		
		glTexCoord2f(0, 0);  
		glVertex2f(0, sheight);
		glTexCoord2f(av_library[stream].width, 0);
		glVertex2f(swidth, sheight); 
		
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
}

int get_buffer_height(void){
	return my_buf.height;
}
int get_buffer_width(void){
	return my_buf.width;
}
unsigned char *get_buffer_map(void){
	return map;
}

video_mbuf mbuf;

void initvideo4linux(void){
   char my_video_dev[100]  = "/dev/video0";
   
   if (-1 == (fd = open(my_video_dev, O_RDWR | O_LARGEFILE))) {
	set_label_text(get_video_capture_label_id(), "Video is down");
   }
   
   if (-1 == ioctl(fd,VIDIOCGCAP,&capability)) {
	printf("Error: ioctl(fd,VIDIOCGCAP,&capability)\n");
   }
   
   //fcntl(fd,F_SETFD,FD_CLOEXEC);
   for (int i = 0; i < capability.channels; i++) {
	    vid_chnl.channel = i;
	    if (ioctl(fd, VIDIOCGCHAN, &vid_chnl) == -1) {
		printf("ioctl (VIDIOCGCHAN)");
	    }
	    printf("The name of input channel #%d is \"%s\".\n", i, vid_chnl.name);
	    
   }
   
   printf("Total %d input channel(s). Using channel #%d.\n", capability.channels, channel);
   printf("Supported resolution range: %dx%d => %dx%d\n", capability.minwidth, capability.minheight, capability.maxwidth, capability.maxheight);
    
   if (ioctl(fd, VIDIOCGPICT, &vid_pict) == -1) {
	printf("ioctl (VIDIOCGPICT)");
   }
   
   //vid_pict.palette = VIDEO_PALETTE_RGB24;
   printf("brightness : %d\n", vid_pict.brightness);
   printf("contrast : %d\n", vid_pict.contrast);
   printf("palette : %d\n", vid_pict.palette);
   
   if (ioctl(fd, VIDIOCGPICT, &vid_pict) == -1) {
	printf("ioctl (VIDIOCGPICT)");
   }
   
   vid_chnl.channel = channel;
  // vid_chnl.norm = 0;
   if (ioctl(fd, VIDIOCGCHAN, &vid_chnl) == -1) {
	printf("ioctl (VIDIOCGCHAN)");
   }

       
    	
	mbuf.size = 0;
	mbuf.frames = 0;
	mbuf.offsets[0] = 0;
	ioctl( fd, VIDIOCGMBUF, &mbuf );
	printf( "size: %i\n", mbuf.size );
	printf( "frames: %i\n", mbuf.frames );
	printf( "offset[0]: %i\n", mbuf.offsets[0] );
	printf( "offset[1]: %i\n", mbuf.offsets[1] );
	//size = mbuf.size;
	unsigned int offset;
	offset = mbuf.offsets[1];
	ioffset = mbuf.offsets[1];
	//this->offset = mbuf.offsets[1];
    
	map = (GLubyte * )mmap( NULL, mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mbuf.offsets[0] );
	//map0 = (unsigned char * )mmap( NULL, mbuf.size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, mbuf.offsets[0] );
        //frame =  (char *)mmap(0, mbufsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	/*if( (int)map == -1 )
	{
		perror( "Couldnt mmap" );
	}*/

	my_buf.frame = 0;
	my_buf.format = VIDEO_PALETTE_RGB565;// VIDEO_PALETTE_YUV422;//VIDEO_PALETTE_RGB24;
	my_buf.width = 128;
	my_buf.height = 96;
	int stop=1;
	
		ioctl(fd, VIDIOCMCAPTURE, &my_buf);
		ioctl(fd, VIDIOCSYNC, 0);
	
	
}



int get_capture_size(void){
	return mbuf.size;
}
void capture_video(void){
	ioctl(fd, VIDIOCMCAPTURE, &my_buf);
	
}
