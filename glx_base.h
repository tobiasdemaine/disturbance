
#ifndef _GLX_BASE_H_
#define _GLX_BASE_H_
 
/* Standard OpenGL/GLX header files */
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/gl.h>
#include <GL/glu.h>

/* Headers needed for keys used in the program */
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

/* Standard C header files */
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <sys/time.h>
#include <string.h>
#include <pthread.h>   
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <lla/LlaClient.h>
#include <lla/SimpleClient.h>
#include <lla/select_server/SelectServer.h>
#include "gui.h"
#include "video.h"
#include "mesh.h"
#include "audio.h"
#include "camera.h"
#include "db.h"
#include "helper.h"
#include "interface.h"

/* Function Definitions */
int getMS(void);
float getFPS(void); 
void createGLWindow(const char* title, int width, int height, int bpp, BOOL fsflag);
void runGLScene(void);
void killGLWindow(void);
void resizeGLScene(void);
void swapBuffers(void);
void initKeys(void);
void updateGLScene(void);
BOOL initGLScene(void);
void cleanGLScene(void);
void drawGLScene(void);
int getwinwidth(void);
int getwinheight(void);
int geteditorwidth(void);
int getmousex(void);
int getmousey(void);
void setpbuffer(void);
void unsetpbuffer(void);


//globals

#endif /* End of _GLX_BASE_H_ */
