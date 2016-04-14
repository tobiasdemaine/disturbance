#ifndef _MESH_H_
#define _MESH_H_
#include "glx_base.h"
#include "helper.h"
#include "3ds.h"
typedef struct{
	L3DS scene;
	int tonce;
	LMap diffuse;
	LMap specular;
	GLuint default_texture;
	int default_texture_width;
	int default_texture_height;
	int type;
	float r;       
	float g;
	float b;
	float a;
	float rotation;
	float rotation_x;
	float rotation_y;
	float rotation_z;
	float scale_x;
	float scale_y;
	float scale_z;
	float x;
	float y;
	float z;
	BOOL auto_rotation;
	float tmpr;
}mesh_lib;

int create_mesh(char *file);

void mesh_create_default_texture(int mesh, char *file);

void use_external_texture(int mesh, GLuint mytexture);

void mesh_set_postion(int mesh, float mx, float my, float mz);

void mesh_set_scale(int mesh, float scale_x, float scale_y, float scale_z); 

void mesh_set_rotation(int mesh, float rotation, float rotation_x, float rotation_y, float rotation_z, BOOL auto_rotation);

void draw_mesh(int mesh);

void mesh_set_rgba(int mesh, float r, float g, float b, float a);
#endif
