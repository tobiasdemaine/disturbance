#include "mesh.h"

mesh_lib mesh_library[10000]; /* 10000 meshes */
int mesh_count=0;

int create_mesh(char *file){
	mesh_count++;
	mesh_library[mesh_count].scene.LoadFile(file);
	mesh_library[mesh_count].type = 1;
	mesh_set_rgba(mesh_count, 1.0, 1.0, 1.0, 1.0);
	mesh_set_rotation(mesh_count, 0, 0, 0, 0, 0);
	mesh_set_scale(mesh_count, 1, 1, 1);
	mesh_library[mesh_count].tmpr=0;
	mesh_library[mesh_count].tonce=0;
	
	return mesh_count;
}

void mesh_create_default_texture(int mesh, char *file){
	mesh_library[mesh].default_texture = load_image(file, mesh);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_WIDTH, &mesh_library[mesh].default_texture_width);
	glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_HEIGHT, &mesh_library[mesh].default_texture_height);

	for (uint i= 0; i<mesh_library[mesh].scene.GetMeshCount(); i++){
		LMesh &lmesh = mesh_library[mesh].scene.GetMesh(i);
		for(uint c=0; c<lmesh.GetTriangleCount()*3;c++){
			LVector2 tv;
			tv = lmesh.GetUV(c);
			tv.x = mesh_library[mesh].default_texture_width*tv.x;
			tv.y = mesh_library[mesh].default_texture_height*tv.y;
			lmesh.SetUV(tv, c);
		}
		
	}
}

void use_external_texture(int mesh, GLuint mytexture){
	mesh_library[mesh].default_texture = mytexture;
	if(mesh_library[mesh].tonce==0){
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, mesh_library[mesh].default_texture);
		glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_WIDTH, &mesh_library[mesh].default_texture_width);
		glGetTexLevelParameteriv(GL_TEXTURE_RECTANGLE_NV, 0,  GL_TEXTURE_HEIGHT, &mesh_library[mesh].default_texture_height);
		printf("%i %i \n", mesh_library[mesh].default_texture_width, mesh_library[mesh].default_texture_height);
		for (uint i= 0; i<mesh_library[mesh].scene.GetMeshCount(); i++){
			LMesh &lmesh = mesh_library[mesh].scene.GetMesh(i);
			for(uint c=0; c<lmesh.GetTriangleCount()*3;c++){
				LVector2 tv;
				tv = lmesh.GetUV(c);
				tv.x = mesh_library[mesh].default_texture_width*tv.x;
				tv.y = mesh_library[mesh].default_texture_height*tv.y;
				lmesh.SetUV(tv, c);
			}	
		}
	mesh_library[mesh].tonce=1;
	}
}

void mesh_set_rgba(int mesh, float r, float g, float b, float a){
	 mesh_library[mesh].r = r;
	 mesh_library[mesh].g = g;
	 mesh_library[mesh].b = b;
	 mesh_library[mesh].a = a;
}

void mesh_set_rotation(int mesh, float rotation, float rotation_x, float rotation_y, float rotation_z, BOOL auto_rotation){
	 mesh_library[mesh].rotation = rotation;
	 mesh_library[mesh].rotation_x = rotation_x;
	 mesh_library[mesh].rotation_y = rotation_y;
	 mesh_library[mesh].rotation_z = rotation_z;
	 mesh_library[mesh].auto_rotation = auto_rotation;
}

void mesh_set_postion(int mesh, float mx, float my, float mz){
	mesh_library[mesh].x = mx;
	mesh_library[mesh].y = my;
	mesh_library[mesh].z = mz;
}
void mesh_set_scale(int mesh, float scale_x, float scale_y, float scale_z){
	mesh_library[mesh].scale_x = scale_x;
	mesh_library[mesh].scale_y = scale_y;
	mesh_library[mesh].scale_z = scale_z;
}
void draw_mesh(int mesh){
	printf("draw_mesh\n");
	glPushMatrix();
	/* we will do the sceen animation basics here */
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	/*glDisable(GL_DEPTH_TEST);*/
	glRotatef(mesh_library[mesh].rotation, mesh_library[mesh].rotation_x, mesh_library[mesh].rotation_y, mesh_library[mesh].rotation_z);
	glTranslatef(mesh_library[mesh].x, mesh_library[mesh].y, mesh_library[mesh].z);
	glScalef(mesh_library[mesh].scale_x, mesh_library[mesh].scale_y, mesh_library[mesh].scale_y);
	glColor4f(mesh_library[mesh].r, mesh_library[mesh].g, mesh_library[mesh].b, mesh_library[mesh].a);
	
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, mesh_library[mesh].default_texture);
	if(mesh_library[mesh].type==1){
		for (uint i= 0; i<mesh_library[mesh].scene.GetMeshCount(); i++){
			LMesh &lmesh = mesh_library[mesh].scene.GetMesh(i);
			glVertexPointer(4, GL_FLOAT, 0, &lmesh.GetVertex(0));
			glNormalPointer(GL_FLOAT, 0, &lmesh.GetNormal(0));
			glTexCoordPointer(2, GL_FLOAT, 0, &lmesh.GetUV(0));
			glDrawElements(GL_TRIANGLES, lmesh.GetTriangleCount()*3, GL_UNSIGNED_SHORT, &lmesh.GetTriangle(0));
		}
	}
	glColor4f(1.0, 1.0, 1.0, 1.0);	
	glDisable(GL_TEXTURE_RECTANGLE_NV);
	glPopMatrix();	
	
}
