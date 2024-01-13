#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "display.h"
#include "vector.h"
#include "array.h"
#include <SDL.h>
#include "clipping.h"
#include "math.h"
#include "matrix.h"
#include "camera.h"
#include "upng.h"
#include <Windows.h>
#include "light.h"
#include "triangle.h"
#include "mesh.h"

bool is_running = false;
float delta_time = 0.0f;


#define MAX_TRIANGLES_PER_MESH 10000

//int because cant store fractions of ms
int previous_frame_time = 0;

//Need to declare this as a dynamic array
//triangle_t triangles_to_render[N_MESH_FACES];
//Array of traingles to be rendered frame by frame
triangle_t triangles_to_render[MAX_TRIANGLES_PER_MESH];



int num_triangles_to_render = 0;

mat4_t proj_matrix;

mat4_t world_matrix;

mat4_t view_matrix;

vec3_t up = { 0,1,0 };

void setup(void) 
{

	//initialize scene light direction
	init_light(vec3_new(0.0, 0.0, 1.0f));
	init_camera(vec3_new(0, 0, 0), vec3_new(0, 0, -1));

	// Initialize the perspective projection matrix
	float aspectx = (float)get_window_width() / (float)get_window_height();
	float aspecty = (float)get_window_height() / (float)get_window_width();
	float fovy = M_PI / 3.0; // the same as 180/3, or 60deg
	float fovx = atan((tan(fovy / 2)) * aspectx) * 2.0f;
	float znear = 1;
	float zfar = 50.0;
	proj_matrix = mat4_make_perspective(fovy, aspecty, znear, zfar);

	//Initialize the frustum planes with a point and a normal

	init_frustum_planes(fovx,fovy, znear, zfar);


	//TODO: Create a new mesh given the obj file, texture file, scale, translation, rotation
	load_mesh("../assets/f22.obj", "../assets/f22.png", vec3_new(1,1,1), vec3_new(-3,0,8), vec3_new(0,0,0));
	load_mesh("../assets/f117.obj", "../assets/f117.png", vec3_new(1,1,1), vec3_new(3,0,8), vec3_new(0,0,0));


}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
		//top right x button on windows
		case SDL_QUIT:
			is_running = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				is_running = false;
			}
			if (event.key.keysym.sym == SDLK_1) 
			{
				set_render_option(WireframeDot);
			}
			if (event.key.keysym.sym == SDLK_2) 
			{
				set_render_option(WireframeLine);
			}
			if (event.key.keysym.sym == SDLK_3) 
			{
				set_render_option(FilledTriangles);

			}
			if (event.key.keysym.sym == SDLK_4) 
			{
				set_render_option(FilledTriangleAndWireframeLines);

			}if (event.key.keysym.sym == SDLK_5)
			{
				set_render_option(Textured);

			}if (event.key.keysym.sym == SDLK_6)
			{
				set_render_option(Textured_Wire);
			}
			if (event.key.keysym.sym == SDLK_c)
			{
				set_back_face_culling(true);
			}
			if (event.key.keysym.sym == SDLK_v) 
			{
				set_back_face_culling(false);
			}

			if (event.key.keysym.sym == SDLK_w) 
			{
				//we want to move in the direction that the camera is facing multiplied by the camrea speed
				update_camera_forward_velocity(vec3_mul(get_camera_direction(), get_camera_move_speed() * delta_time));
				//we then add the velocity to the current camera position
				update_camera_position(vec3_add(get_camera_position(), get_camera_velocity()));
			}
			if (event.key.keysym.sym == SDLK_s)
			{
				//we want to move in the direction that the camera is facing multiplied by the camrea speed
				update_camera_forward_velocity(vec3_mul(get_camera_direction(), get_camera_move_speed() * delta_time));
				//we then add the velocity to the current camera position
				update_camera_position( vec3_subtract(get_camera_position(),get_camera_velocity()));

			}
			if (event.key.keysym.sym == SDLK_a) 
			{
				update_camera_yaw(get_camera_yaw() - get_camera_rotate_speed() * delta_time);
				
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				update_camera_yaw(get_camera_yaw() + get_camera_rotate_speed() * delta_time);

			}
			if (event.key.keysym.sym == SDLK_o)
			{
				update_camera_pitch(get_camera_pitch() - get_camera_rotate_speed() * delta_time);

			}
			if (event.key.keysym.sym == SDLK_p)
			{
				update_camera_pitch(get_camera_pitch() + get_camera_rotate_speed() * delta_time);


			}

			if (event.key.keysym.sym == SDLK_SPACE) 
			{
				update_camera_position_y(get_camera_position().y + get_camera_move_speed() * delta_time);
			}
			if (event.key.keysym.sym == SDLK_LCTRL) 
			{
				update_camera_position_y(get_camera_position().y - get_camera_move_speed() * delta_time);


			}

			break;
	}

}


void update(void) {

	//ms of last frame
	//SDL_GetTicks() gets the ms since SDL_Initialize
	//wait until the current tick time since init is less than the last frame tick time + our frame target data 
	//while this does work, the CPU is still working on this while loop for the frame targe time
	//it would be much better to give up control or tell the OS "Hey go do something else for this lnog"
	//SDL_Delay does this
	//while (!SDL_TICKS_PASSED(SDL_GetTicks(), previous_frame_time + FRAME_TARGET_TIME));

	//Get a delta time factor converted to seconds to be used to updated our game objects (GetTicks and previous_frame_time is in ms, we want just s)
	delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;

	previous_frame_time = SDL_GetTicks();

	//initialize the array of triangles to render -> every frame
	//number of triangles to render counter
	num_triangles_to_render = 0;

	//loop all meshes of our scene
	for (int mesh_index = 0; mesh_index < get_num_meshes(); mesh_index++) {

		mesh_t* mesh = get_mesh_at(mesh_index);

		/*mesh.rotation.y += 0.6f * delta_time;*/
		//mesh.rotation.x+= 0.6f * delta_time;
		//mesh.rotation.z+= .002f * delta_time;
		/*mesh.translation.z = 5.0;*/

		//create a scale matrix that will be used to multiply the mesh vertices
		mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);

		mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh->rotation.x);
		mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh->rotation.y);
		mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh->rotation.z);

		mat4_t translation_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);

		//Create View Matrix, needs to happen every frame, dependingg on things rotate and how the camera moves,


		//Change the camera position per aniation frame
		//camera.position.x += 0.5f * delta_time;
		//camera.direction.y += 5.f * delta_time;
		//camera.position.y += 0.08f;

		//Compute the new camera rotation and translation for the FPS camera movement
		//Find the target point, will be based on the yaw rotation, velocity and direction

		//Initialize target as positive z-axis
		vec3_t target = { 0,0,1 };

		//Now we need to rotate target by the YAW of the camera by creating a rotation matrix
		mat4_t camera_yaw_rotation = mat4_make_rotation_y(get_camera().yaw);
		mat4_t camera_pitch_rotation = mat4_make_rotation_x(get_camera().pitch);

		mat4_t camera_rotation = mat4_identity();
		camera_rotation = mat4_mul_mat4(camera_rotation, camera_yaw_rotation);
		camera_rotation = mat4_mul_mat4(camera_rotation, camera_pitch_rotation);

		//new camera direction based off the rotation of the yaw
		update_camera_direction(vec3_from_vec4(mat4_mul_vec4(camera_rotation, vec4_from_vec3(target))));

		//offset the camere position in the direction where the camera is pointing at
		target = vec3_add(get_camera().position, get_camera().direction);


		view_matrix = mat4_look_at(get_camera().position, target, up);


		world_matrix = mat4_identity();

		world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
		world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
		world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
		world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
		world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

		int num_faces = array_length(mesh->faces);
		//loop over every triangle face of mesh
		for (int i = 0; i < num_faces; i++) {
			//we just want to render the front face of the cube for now


			face_t mesh_face = mesh->faces[i];

			//get every vertex of the triangle and add it to the vec3 array
			vec3_t face_vertices[3];
			face_vertices[0] = mesh->vertices[mesh_face.a];
			face_vertices[1] = mesh->vertices[mesh_face.b];
			face_vertices[2] = mesh->vertices[mesh_face.c];

			vec4_t transformed_vertices[3];
			//loop through each vertices of the current triangle in the current faceand apply transformation
			for (int j = 0; j < 3; j++) {

				vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

				//Create a world matrix combining scale, rotation and translation matrices


				transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

				//multiply the view matrix by the vector to transform my scene to camera space

				transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

				//saved current transformed vertex in array of transformed vertices
				transformed_vertices[j] = transformed_vertex;

			}

			/////// Back Face Culling

			//Find the N vector
			//Cannot use the face_vertices as the are not transformed yet
			vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
			vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
			vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

			vec3_t BA = vec3_subtract(vector_b, vector_a);
			vec3_t CA = vec3_subtract(vector_c, vector_a);

			vec3_normalize(&BA);
			vec3_normalize(&CA);


			////Get the cross product

			vec3_t tri_normal = vec3_cross(BA, CA);

			//Get the normal of the face normal vector
			vec3_normalize(&tri_normal);

			////Get the vector from camera to A
			vec3_t origin = { 0,0,0 };
			vec3_t a_to_camera = vec3_subtract(origin, vector_a);
			vec3_normalize(&a_to_camera);

			float DotResult = vec3_dot(a_to_camera, tri_normal);

			if (is_back_face_culling())
			{
				if (DotResult < 0.f) {
					continue;
				}


			}

			//TODO: perform clipping

			//Create polygon from original transformed triangle, this is done before projection
			polygon_t polygon = create_polygon_from_triangle(vec3_from_vec4(transformed_vertices[0]), vec3_from_vec4(transformed_vertices[1]), vec3_from_vec4(transformed_vertices[2]), mesh_face.a_uv, mesh_face.b_uv, mesh_face.c_uv);

			//clip the plolygon against six frustum planes and return new polygon with potentially new vertices
			clip_polygon(&polygon);

			//printf("number of polygon vertices after clipping: %d\n", polygon.num_vertices);

			//TODO: Break the clipped polygon apart back into individual triangles
			int num_triangles_after_clipping = 0;
			triangle_t triangles_after_clipping[MAX_NUM_POLY_TRIANGLE];

			//TODO: After clipping we need to break the polygon into triangles
			triangles_from_polygon(&polygon, triangles_after_clipping, &num_triangles_after_clipping);

			//Here we will have the array of triangles and also the num of triangles
			//Loop all assembled triangles after clipping
			for (int t = 0; t < num_triangles_after_clipping; t++) {
				triangle_t triangle_after_clipping = triangles_after_clipping[t];

				vec4_t projected_points[3];
				for (int j = 0; j < 3; j++)
				{

					//project the current vertex to onscreen point, these points are now in NDC, normalized Device Coordinates (IMAGE SPACE)
					projected_points[j] = mat4_mul_vec4_project(proj_matrix, triangle_after_clipping.points[j]);

					//Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values values top->downw
					projected_points[j].y *= -1;
					//Scale into the view
					projected_points[j].x *= (get_window_width() / 2.0f);
					projected_points[j].y *= (get_window_height() / 2.0f);

					//Invert the y-values to account for flipped screen y coordinate

					//translate projected points to the middle of the screen so we in screen space here then
					projected_points[j].x += (get_window_width() / 2.0f);
					projected_points[j].y += (get_window_height() / 2.0f);


				}

				//Calculate the light intensity based on how aligned is teh normal and the inverse of the light raw
				uint32_t triangle_color = mesh_face.color;
				float light_intensity_factor = -vec3_dot(tri_normal, get_light().direction);

				uint32_t new_triangle_color = light_apply_intensity(triangle_color, light_intensity_factor);

				//Calculate the color of the triangle

				triangle_t projected_triangle_to_render =
				{
					.points =
					{
						{projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w},
						{projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w},
						{projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w}
					},

					//Loading from the mesh the cube arrray, transporting from the mesh to the triangle we are going to render in our render function
					.texcoords = {
						{ triangle_after_clipping.texcoords[0].u, triangle_after_clipping.texcoords[0].v},
						{ triangle_after_clipping.texcoords[1].u, triangle_after_clipping.texcoords[1].v },
						{ triangle_after_clipping.texcoords[2].u, triangle_after_clipping.texcoords[2].v }
					},
					.color = new_triangle_color,
					.texture = mesh->texture,
				};

				//save the projected triangle in the arrya of triangles to render
				if (num_triangles_to_render < MAX_TRIANGLES_PER_MESH) {

					triangles_to_render[num_triangles_to_render] = projected_triangle_to_render;
					num_triangles_to_render++;
				}
			}
		}
	}
}

void render(void)
{

	clear_color_buffer(0xFF000000);
	clear_z_buffer();

	draw_grid();
	
	//loop over projectd and render
	//Push to middle of screen vertically and horizontally
	//pixel color

	//get size of triangles to render array


	for (int i = 0; i < num_triangles_to_render; i++) 
	{
		triangle_t triangle = triangles_to_render[i];
		//draw every point in the triangle

		if (should_render_filled_triangles())
		{
			draw_filled_triangle
			(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w,
				triangle.color
			);
		}

		if (should_render_textured_triangles()) {

	//		//need to also pass UVs 

			draw_textured_triangle
			(
				(float)triangle.points[0].x, (float)triangle.points[0].y, (float)triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v, //vertex A
				(float)triangle.points[1].x, (float)triangle.points[1].y, (float)triangle.points[1].z, triangle.points[1].w,triangle.texcoords[1].u, triangle.texcoords[1].v, //vertex B
				(float)triangle.points[2].x, (float)triangle.points[2].y, (float)triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v, //Vertex C
				triangle.texture
			);
		}
		if (should_render_dots()) 
		{

			draw_rect(triangle.points[0].x, triangle.points[0].y, 5, 5, 0xFFF20587);
			draw_rect(triangle.points[1].x, triangle.points[1].y, 5, 5, 0xFFF20587);
			draw_rect(triangle.points[2].x, triangle.points[2].y, 5, 5, 0xFFF20587);
		}

		if (should_render_wire_traingles())
		{
			draw_triangle
			(
				triangle.points[0].x, triangle.points[0].y,
				triangle.points[1].x, triangle.points[1].y,
				triangle.points[2].x, triangle.points[2].y,
				0xFFFFFFFF
			);
		}


	}
	

	
	render_color_buffer();

}
/// <summary>
/// Free te memory that was dynamically allocated by the program
/// </summary>
void free_resources() {

	free_meshes();


	//upng_free(png_texture);
	//array_free(mesh.faces);
	//array_free(mesh.vertices);
}

int main(int argc, char* args[]) 
{

	is_running = initialize_window();

	setup();


	while (is_running) 
	{
		process_input();
		update();
		render();
	}

	destroy_window();
	free_resources();

	return 0;
}