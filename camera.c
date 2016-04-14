#include "camera.h"
typedef struct{
	GLfloat dblAngle1;
	GLfloat dblAngle2;
	float cup;
	float cdist;
}cameras;
cameras cam[100]; /* 100 inbuilt cameras */
float tmpc;
float tickz(float tick){
	tmpc=(((float)getMS())/1000);
	return tmpc*tick;
}

void use_inbuilt_camera_path(int camera){
	GLfloat xtrans;
	GLfloat ztrans;
	GLfloat ytrans;
	if (camera == 1){
		float mspin=(25);
		cam[1].dblAngle1 = cam[1].dblAngle1 + tickz(0.3);
		cam[1].dblAngle2 = cam[1].dblAngle2 + tickz(0.3);
        	xtrans = mspin * cos(cam[1].dblAngle1) * cos(cam[1].dblAngle2);
		ztrans = mspin * sin(cam[1].dblAngle2);
		ytrans = mspin * sin(cam[1].dblAngle1) * cos(cam[1].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
	if (camera == 2){
		float mspin=(25);
		cam[2].dblAngle1 = cam[2].dblAngle1 - tickz(0.9);
        	cam[2].dblAngle2 = cam[2].dblAngle2 - tickz(0.03);
		xtrans = mspin * cos(cam[2].dblAngle1) * cos(cam[2].dblAngle2);
		ztrans = mspin * sin(cam[2].dblAngle2);
		ytrans = mspin * sin(cam[2].dblAngle1) * cos(cam[2].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
	if (camera == 3){
		float mspin=(10);
		cam[3].dblAngle1 = cam[3].dblAngle1 - tickz(0.01);
        	cam[3].dblAngle2 = tickz(0.01);
        	xtrans = mspin * cos(cam[3].dblAngle1) * cos(cam[3].dblAngle2);
		ztrans = mspin * sin(cam[3].dblAngle2);
		ytrans = mspin * sin(cam[3].dblAngle1) * cos(cam[3].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
	if (camera == 4){
		if (cam[4].cup == 1) cam[4].cdist+=1;
		else cam[4].cdist-=0.7;
		if (cam[4].cdist>300) cam[4].cup=0;
		if (cam[4].cdist<5) cam[4].cup=1;
		float mspin=cam[4].cdist;
		cam[4].dblAngle1 = cam[4].dblAngle1 - tickz(0.01);
        	cam[4].dblAngle2 = cam[4].dblAngle2 + tickz(0.1);
        	xtrans = mspin * cos(cam[4].dblAngle1) * cos(cam[4].dblAngle2);
		ztrans = mspin * sin(cam[4].dblAngle2);
		ytrans = mspin * sin(cam[4].dblAngle1) * cos(cam[4].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
	if (camera == 5){
		if (cam[5].cup == 1) cam[5].cdist+=0.2;
		else cam[5].cdist-=1;
		if (cam[5].cdist>100) cam[5].cup=0;
		if (cam[5].cdist<5) cam[5].cup=1;
		float mspin=cam[5].cdist;
		cam[5].dblAngle1 = cam[5].dblAngle1 - tickz(0.1);
        	cam[5].dblAngle2 = cam[5].dblAngle2 + tickz(0.001);
        	xtrans = mspin * cos(cam[5].dblAngle1) * cos(cam[5].dblAngle2);
		ztrans = mspin * sin(cam[5].dblAngle2);
		ytrans = mspin * sin(cam[5].dblAngle1) * cos(cam[5].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
	if (camera == 6){
		if (cam[6].cup == 1) cam[6].cdist+=1;
		else cam[6].cdist-=1;
		if (cam[6].cdist>300) cam[6].cup=0;
		if (cam[6].cdist<5) cam[6].cup=1;
		float mspin=cam[7].cdist;
		cam[6].dblAngle1 = cam[6].dblAngle1 - tickz(0.01);
        	cam[6].dblAngle2 = cam[6].dblAngle2 + tickz(0.06);
        	xtrans = mspin * cos(cam[6].dblAngle1) * cos(cam[6].dblAngle2);
		ztrans = mspin * sin(cam[6].dblAngle2);
		ytrans = mspin * sin(cam[6].dblAngle1) * cos(cam[6].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
	if (camera == 7){
		if (cam[7].cup == 1) cam[7].cdist+=3;
		else cam[7].cdist-=3;
		if (cam[7].cdist>500) cam[7].cup=0;
		if (cam[7].cdist<5) cam[7].cup=1;
		float mspin=cam[7].cdist;
		cam[7].dblAngle1 = cam[7].dblAngle1 - tickz(0.01);
        	cam[7].dblAngle2 = cam[7].dblAngle2 + tickz(0.3);
        	xtrans = mspin * cos(cam[7].dblAngle1) * cos(cam[7].dblAngle2);
		ztrans = mspin * sin(cam[7].dblAngle2);
		ytrans = mspin * sin(cam[7].dblAngle1) * cos(cam[7].dblAngle2);
		gluLookAt(xtrans, ytrans, ztrans, 0,0,0, 0,1,0);
	}
}
