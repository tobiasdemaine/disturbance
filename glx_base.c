#include "glx_base.h"
 
#include <sys/time.h>
#include <unistd.h>
XVisualInfo *vi;
static GLXFBConfig fbconfig;
static GLXPbuffer glxpbuffer;
static GLXContext pbuffercontext;
static int allocatedpbuffer = 0;
typedef struct
{
	Display 				*dpy;
	int						screen;
	Window					win;
	GLXContext				ctx;
	XSetWindowAttributes	attr;
	BOOL					fs;
	XF86VidModeModeInfo		deskMode;
	int						x,y;
	unsigned int			width, height;
	unsigned int			bpp;
}GLWindow;

static GLWindow GLWin;	
 
int mouse_x, mouse_y;

/* */

unsigned int diff = 1;
unsigned int bytes_per_pixel = 0;
static int frames = 0;
float fps;
float getFPS(void){
	return fps;
}
int getMS(void){
	return diff;
}

/*  */

float winwidth=2304, winheight=800;

 int getwinwidth(void){
 	return (int)winwidth;
 }
 int getwinheight(void){
 	return (int)winheight;
 }
 int geteditorwidth(void){
 	return (int)1280;
 }
 int getmousex(void){
 	return mouse_x;
 }
 int getmousey(void){
 	return mouse_y;
 }
 
void send_keypress(char *keystring){
	gui_keypress_handler(keystring);
	edit_mode(keystring);
}
void send_keyrelease(char *keystring){
	gui_keyrelease_handler(keystring);
}

void destroypbuffer(void)
{
    if (allocatedpbuffer) {
	glXDestroyContext(GLWin.dpy, pbuffercontext);
	glXDestroyPbuffer(GLWin.dpy, glxpbuffer);
    }
}

void setpbuffer(void){
	glXMakeCurrent(GLWin.dpy, glxpbuffer, pbuffercontext);
}
void unsetpbuffer(void){
	glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
}

int setuppbuffer(char **msg, int pwidth, int pheight)
{
    /* off-screen pbuffers have DRAWABLE_TYPE=PBUFFER_BIT,
       X_RENDERABLE=False, GLX_PRESERVED_CONTENTS? */
    						  
    int pbconfig[] = {GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        GLX_RED_SIZE, 4,
        GLX_GREEN_SIZE, 4,
        GLX_BLUE_SIZE, 4,
        GLX_ALPHA_SIZE, 0, /* XXX Sun Creator3D does not support dst alpha :*/
        GLX_DEPTH_SIZE, 0,
        GLX_STENCIL_SIZE, 0,
        None};
    
	 
       GLXFBConfig *fbconfigs = NULL;
    int nconfigs = 0;
    int i;
    int value;
    int pbattriblist[] = {GLX_PBUFFER_WIDTH, 800, GLX_PBUFFER_HEIGHT, 600, GLX_PRESERVED_CONTENTS, True,	None}; 
    
    
	
    fbconfigs = glXChooseFBConfig(GLWin.dpy, GLWin.screen, pbconfig, &nconfigs);

    if (fbconfigs == NULL || nconfigs < 1) {
        *msg = "no fbconfigs";
        return -1;
    }

    /* loop through, until we find one that succeeds */
    for(i = 0; i < nconfigs; i++) {
        /*printf("fbconfig %d of %d\n", i, nconfigs)*/;
        if (glXGetFBConfigAttrib(GLWin.dpy, fbconfigs[i],
                                 GLX_MAX_PBUFFER_PIXELS, &value) == Success) {
            if (value < pwidth * pheight) {
                printf("not enough pixels: %d, need %d\n",
                       value, pwidth*pheight);
                continue;
            }
        }
        if (glXGetFBConfigAttrib(GLWin.dpy, fbconfigs[i],
                                 GLX_MAX_PBUFFER_WIDTH, &value) == Success) {
            if (value < pwidth) {
                printf("width too small: %d, need %d\n", value, pwidth);
                continue;
            }
        }
        if (glXGetFBConfigAttrib(GLWin.dpy, fbconfigs[i],
                                 GLX_MAX_PBUFFER_HEIGHT, &value) == Success) {
            if (value < pheight) {
                printf("height too small: %d, need %d\n", value, pheight);
                continue;
            }
        }

        /* if ok, pick this one */
        break;
    }

    if (i >= nconfigs) {
        XFree(fbconfigs);
        *msg = "could not find a suitable fbconfig for pbuffer";
        return -1;
    }

    fbconfig = fbconfigs[i]; /* XXX memory... */
    XFree(fbconfigs);

    /* make window glx drawable */
    glxpbuffer = glXCreatePbuffer(GLWin.dpy, fbconfig, pbattriblist); 
    if (glxpbuffer == None) {
	*msg = "glXCreatePbuffer";
	return -1;
    }

    /* create direct-rendering GLX context */
    pbuffercontext = glXCreateContext (GLWin.dpy, vi, GLWin.ctx, True);
    /*pbuffercontext = glXCreateNewContext(GLWin.dpy, fbconfig, GLX_RGBA_TYPE, NULL, True);*/
    if (pbuffercontext == NULL) {
	*msg = "glXCreateNewContext failed";
        return -1;
    }

    if (!glXIsDirect(GLWin.dpy, pbuffercontext))
        printf("falling back to indirect rendering (remote server?)\n");

    /* bind context to glx pbuffer drawable */
   

    allocatedpbuffer = 1;
    return 0;
}

 
void createGLWindow(const char* title, int width, int height, int bpp, BOOL fsflag)
{
	int attrListSgl[] = {GLX_RGBA, GLX_RED_SIZE, 4,
							GLX_GREEN_SIZE, 4,
							GLX_BLUE_SIZE, 4,
							GLX_DEPTH_SIZE, 16,
							None};

	int attrListDbl[] = {GLX_RGBA, GLX_DOUBLEBUFFER,
							GLX_RED_SIZE, 4,
							GLX_GREEN_SIZE, 4,
							GLX_BLUE_SIZE, 4,
							GLX_DEPTH_SIZE, 16,
							None};

	
	Colormap cmap;
	int dpyWidth, dpyHeight;
	int i;
	int vidModeMajorVersion, vidModeMinorVersion;
	XF86VidModeModeInfo **modes;
	int modeNum;
	int bestMode;
	Atom wmDelete;
	Window winDummy;
	unsigned int borderDummy;
	char *errmsg;
	GLWin.fs = fsflag;
	bestMode = 0;

	GLWin.dpy = XOpenDisplay(0);
	GLWin.screen = DefaultScreen(GLWin.dpy);
	XF86VidModeQueryVersion(GLWin.dpy, &vidModeMajorVersion,
							&vidModeMinorVersion);

	XF86VidModeGetAllModeLines(GLWin.dpy, GLWin.screen, &modeNum, &modes);

	GLWin.deskMode = *modes[0];

	for (i = 0; i < modeNum; i++)
	{
		if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height))
		{
			bestMode = i;
		}
	}

	vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListDbl);
	if(NULL == vi)
	{
		vi = glXChooseVisual(GLWin.dpy, GLWin.screen, attrListSgl);
		printf("Singlebuffered : true\n");
	}
	else
	{
		printf("Doublebuffered : true\n");
	}

	GLWin.ctx = glXCreateContext(GLWin.dpy, vi, 0, GL_TRUE);

	cmap = XCreateColormap(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),vi->visual, AllocNone);
	GLWin.attr.colormap = cmap;
	GLWin.attr.border_pixel = 0;

	if(GLWin.fs)
	{
		XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, modes[bestMode]);
		XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
		dpyWidth = modes[bestMode]->hdisplay;
		dpyHeight = modes[bestMode]->vdisplay;
		XFree(modes);
		GLWin.attr.override_redirect = True;
		GLWin.attr.event_mask = ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask |
								StructureNotifyMask;
		GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
								0, 0, dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
								CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
								&GLWin.attr);
		XWarpPointer(GLWin.dpy, None, GLWin.win, 0, 0, 0, 0, 0, 0);
		XMapRaised(GLWin.dpy, GLWin.win);
		XGrabKeyboard(GLWin.dpy, GLWin.win, True, GrabModeAsync,GrabModeAsync, CurrentTime);
		XGrabPointer(GLWin.dpy, GLWin.win, True, ButtonPressMask,
		GrabModeAsync, GrabModeAsync, GLWin.win, None, CurrentTime);
	}
	else
	{
		GLWin.attr.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | StructureNotifyMask;
		GLWin.win = XCreateWindow(GLWin.dpy, RootWindow(GLWin.dpy, vi->screen),
								  0, 0, width, height, 0, vi->depth, InputOutput, vi->visual,
								  CWBorderPixel | CWColormap | CWEventMask, &GLWin.attr);
		wmDelete = XInternAtom(GLWin.dpy, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(GLWin.dpy, GLWin.win, &wmDelete, 1);
		XSetStandardProperties(GLWin.dpy, GLWin.win, title,
		title, None, NULL, 0, NULL);
		XMapRaised(GLWin.dpy, GLWin.win);
	}
	
	glXMakeCurrent(GLWin.dpy, GLWin.win, GLWin.ctx);
	XGetGeometry(GLWin.dpy, GLWin.win, &winDummy, &GLWin.x, &GLWin.y,
				&GLWin.width, &GLWin.height, &borderDummy, &GLWin.bpp);

	printf("Direct Rendering: %s\n",glXIsDirect(GLWin.dpy, GLWin.ctx) ? "true" : "false");
	
	printf("Running in %s mode\n",GLWin.fs ? "fullscreen" : "window");
	if (setuppbuffer(&errmsg, width, height) != 0) printf("\\\n failed to create pbuffer: %s\n", errmsg);
	resizeGLScene();
}

/* runGLScene(void)
 *
 * Main loop for the program.
 */
void runGLScene(void)
{
	XEvent event;
	struct timeval time1;
	struct timeval time2;
	int running = 1;
	while(running)
	{
		gettimeofday(&time1, NULL);
		while(XPending(GLWin.dpy) > 0)
		{
			XNextEvent(GLWin.dpy, &event);
			switch(event.type)
			{
				case Expose:
					if (event.xexpose.count != 0)
						break;
					break;
				case ConfigureNotify:
					if ((event.xconfigure.width != (int)GLWin.width) ||
						(event.xconfigure.height != (int)GLWin.height))
					{
						GLWin.width = event.xconfigure.width;
						GLWin.height = event.xconfigure.height;
						winwidth = event.xconfigure.width;
						winheight = event.xconfigure.height;
						printf("Resize Event\n");
						resizeGLScene();
					}
					break;
				case KeyPress:
					send_keypress(XKeysymToString(XLookupKeysym(&event.xkey,0)));
					switch(XLookupKeysym(&event.xkey,0))
					{
					case XK_Escape:									/* Quit application */
						running = 0;
						break;
					case XK_F1:										
						/* Switch between fullscreen and windowed mode */
						killGLWindow();
						GLWin.fs = !GLWin.fs;
						createGLWindow("TDM", (int)winwidth, (int)winheight, 24, GLWin.fs);
						break;
					}
					break;
				case KeyRelease:
					send_keyrelease(XKeysymToString(XLookupKeysym(&event.xkey,0)));
					break;
				case ButtonRelease:	
					do_button_release(1);
					break;
				case ButtonPress:
					do_button_depress(1);
					break;
				case MotionNotify:
				{
					mouse_x = event.xmotion.x;
					mouse_y = event.xmotion.y;
					break;
				}
				case ClientMessage:
					if (*XGetAtomName(GLWin.dpy, event.xclient.message_type) == *"WM_PROTOCOLS")
					{
						running = 0;
					}
					break;
				default:
					break;
			}
		}
		
		drawGLScene();
		swapBuffers();
		gettimeofday(&time2, NULL);
		diff = (int)((time2.tv_sec * 1000) + (time2.tv_usec / 1000));
		diff -= (int)((time1.tv_sec * 1000) + (time1.tv_usec / 1000));
		
		/*printf("frames = %i\n", frames);
		printf("ms = %i\n", diff);
		printf("fps = %f\n", fps);*/
		
		frames++;
	}
}

/* killGLWindow(void)
 *
 * Destroys our window and OpenGL Context. Closes the display
 */
void killGLWindow(void)
{
	if(GLWin.ctx)
	{
		if(!glXMakeCurrent(GLWin.dpy, None, NULL))
		{
			printf("Error releasing drawing context : killGLWindow\n");
		}
		glXDestroyContext(GLWin.dpy, GLWin.ctx);
		GLWin.ctx = NULL;
	}

	if(GLWin.fs)
	{
		XF86VidModeSwitchToMode(GLWin.dpy, GLWin.screen, &GLWin.deskMode);
		XF86VidModeSetViewPort(GLWin.dpy, GLWin.screen, 0, 0);
	}
	XCloseDisplay(GLWin.dpy);
}

/* swapBuffers(void)
 *
 * Swaps the buffers (easy enough) ;o)
 */
void swapBuffers(void)
{
	glXSwapBuffers(GLWin.dpy, GLWin.win);
}

/* resizeGLScene(void)
 *
 * Resizes the window
 */
void resizeGLScene(void)
{
	/*float width,height;
	if(GLWin.height == 0)
		GLWin.height = 1;
	if(GLWin.width == 0)
		GLWin.width = 1;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,GLWin.width,GLWin.height);
	width = 0.5f;
	height = 0.5f * ((float)(GLWin.height)/GLWin.width);
	glFrustum(-width,width,-height,height,1.0f,100.0f);
	glMatrixMode(GL_MODELVIEW);*/
}
