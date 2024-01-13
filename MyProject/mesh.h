#pragma once
#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"


/// <summary>
/// Define a struct for dynamic size meshes with an array of vertices and faces
/// </summary>
typedef struct {

	vec3_t* vertices;	//mesh dynamic array of vertices
	face_t* faces;		//mesh dynamic array of faces
	vec3_t rotation;	//mesh rotation with x, y, and z values
	vec3_t scale;		//mesh scale with x, y, and z values
	vec3_t translation; //mesh translation on the x, y and z
	upng_t* texture;	//mesh PNG texture pointer
} mesh_t;



void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation);
void load_mesh_png_data(mesh_t* mesh, char* png_filename);
void load_mesh_obj_data(mesh_t* mesh, char* filename);

int get_num_meshes();

void free_meshes();

mesh_t* get_mesh_at(int index);


#endif // !MESH_H
