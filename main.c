#include "glx_base.h"
//global

BOOL initGLScene(void)
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glFlush();
	
	start_audio();
	//initvideo4linux();
	setup_interface();
	system("bash /home/tobias/startup.sh");
	return True;
}

void drawGLScene(void)
{
	sound_process();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	render_all();
	gui_exec();
}

void updateGLScene(void)
{
}

void cleanGLScene(void)
{
}

int main(void)
{
	createGLWindow("Disturbance", 2304, 800, 24, false);
	initGLScene();
	runGLScene();
	cleanGLScene();
	killGLWindow();
	return 0;
}
