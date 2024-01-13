#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include "array.h"
#include <string.h>
#include "mesh.h"


#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES];
static int mesh_count = 0;


void load_mesh(char* obj_filename, char* png_filename, vec3_t scale, vec3_t translation, vec3_t rotation) {
	//TODO:
	//Load the OBJ file to our mesh
	load_mesh_obj_data(&meshes[mesh_count], obj_filename);

	//Load the PNG file information to the mesh teture
	load_mesh_png_data(&meshes[mesh_count], png_filename);
	
	//initialize scale, translation, and rotation with the parameters
	meshes[mesh_count].scale = scale;
	meshes[mesh_count].translation = translation;
	meshes[mesh_count].rotation = rotation;


	//Add the new mesh to the array mesh
	
	mesh_count++;

	printf(meshes);
}


void load_mesh_obj_data(mesh_t* mesh, char* obj_filename) {
	//TODO: read the contents of the .obj file
	//load the vertices and faces in our mesh.vertices and faces
	FILE* asset_file;
	asset_file = fopen(obj_filename, "r");
	char str[1024];
	text2_t* textcoords = NULL;

	if (asset_file != NULL)
	{


		while (fgets(str, 1024, asset_file))
		{

			//vertex information
			if (strncmp(str, "v ", 2) == 0)
			{

				vec3_t vertex;
				sscanf(str, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
				array_push(mesh->vertices, vertex);

			}

			//Texture coordinate information
			if (strncmp(str, "vt ", 3) == 0)
			{
				text2_t textcoord;
				sscanf(str, "vt %f %f", &textcoord.u, &textcoord.v);
				array_push(textcoords, textcoord);
			}


			//face information
			if (strncmp(str, "f ", 2) == 0) {

				int vertex_indices[3];
				int texture_indices[3];
				int normal_indices[3];

				sscanf(str, "f %d/%d/%d %d/%d/%d %d/%d/%d",
					&vertex_indices[0], &texture_indices[0], &normal_indices[0],
					&vertex_indices[1], &texture_indices[1], &normal_indices[1],
					&vertex_indices[2], &texture_indices[2], &normal_indices[2]);
				//They start at one so we gotta subtract one

				face_t face =
				{

					.a = vertex_indices[0] - 1,
					.b = vertex_indices[1] - 1,
					.c = vertex_indices[2] - 1,
					.a_uv = textcoords[texture_indices[0] - 1],
					.b_uv = textcoords[texture_indices[1] - 1],
					.c_uv = textcoords[texture_indices[2] - 1],
					.color = 0xFFFFFFFF

				};

				array_push(mesh->faces, face);

			}

		}

	}
}

void load_mesh_png_data(mesh_t * mesh, char* png_filename)
{
	upng_t* png_image = upng_new_from_file(png_filename);
	if (png_image != NULL)
	{
		upng_decode(png_image);
		if (upng_get_error(png_image) == UPNG_EOK) {
			
			mesh->texture = png_image;
			
		}
	}

}

mesh_t* get_mesh_at(int index) {

	return &meshes[index];
}

int get_num_meshes() {
	return mesh_count;
}

void free_meshes() {
	for (int mesh_index = 0; mesh_index < mesh_count; mesh_index++) {
		
		upng_free(meshes[mesh_index].texture);
		array_free(meshes[mesh_index].faces);
		array_free(meshes[mesh_index].vertices);

	}
}


////TODO: Create implementation of mesh.h functions
//vec3_t cube_vertices[N_CUBE_VERTICES] = {
//	{.x = -1, .y = -1, .z = -1 }, // 1
//	{.x = -1, .y = 1, .z = -1 }, // 2
//	{.x = 1, .y = 1, .z = -1 }, // 3
//	{.x = 1, .y = -1, .z = -1 }, // 4
//	{.x = 1, .y = 1, .z = 1 }, // 5
//	{.x = 1, .y = -1, .z = 1 }, // 6
//	{.x = -1, .y = 1, .z = 1 }, // 7
//	{.x = -1, .y = -1, .z = 1 }  // 8
//
//};

//each triangle face is going to have its own UV coordinates, for now these will match its own vertices but in a normalized fashin

//face_t cube_faces[N_CUBE_FACES] = {
//	// front
//	{.a = 1, .b = 2, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFF0000 },//R
//	{.a = 1, .b = 3, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//	// right
//	{.a = 4, .b = 3, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFFFF00 },//RG
//	{.a = 4, .b = 5, .c = 6, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//	// back
//	{.a = 6, .b = 5, .c = 7, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFFFF00FF },//RB
//	{.a = 6, .b = 7, .c = 8, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//	// left
//	{.a = 8, .b = 7, .c = 2, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFF00FFFF },//GB
//	{.a = 8, .b = 2, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//	// top
//	{.a = 2, .b = 7, .c = 5, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFF0000FF },//B
//	{.a = 2, .b = 5, .c = 3, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF },
//	// bottom
//	{.a = 6, .b = 8, .c = 1, .a_uv = { 0, 1 }, .b_uv = { 0, 0 }, .c_uv = { 1, 0 }, .color = 0xFF00FF00 },//G
//	{.a = 6, .b = 1, .c = 4, .a_uv = { 0, 1 }, .b_uv = { 1, 0 }, .c_uv = { 1, 1 }, .color = 0xFFFFFFFF }
//};

//void load_cube_mesh_data(void) {
//	vec3_t cube_vertex;
//	face_t cube_face;
//	for (int i = 0; i < N_CUBE_VERTICES; i++) {
//		cube_vertex = cube_vertices[i];
//		array_push(mesh.vertices, cube_vertex);
//		
//	}
//	for (int i = 0; i < N_CUBE_FACES; i++) {
//
//		cube_face = cube_faces[i];
//		array_push(mesh.faces, cube_face);
//
//	}
//}

