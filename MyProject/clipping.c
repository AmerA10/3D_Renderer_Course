#include "clipping.h"
#include <stdbool.h>
#include <stdio.h>
#include "math.h"
#include "array.h"
#include "triangle.h"

#define NUM_PLANES 6

//Our frustum is a collection of 6 planes
plane_t frustum_planes[NUM_PLANES];



float float_lerp(float a, float b, float t) {
    float result = a + t * (b - a);

    return result;
}


//frustum planes are defined by a point and a by a normla vector

//The planes for our frustum are defined as follows

// Near  Plane : P(0,0,z_near), N=(0		  ,				 0, 	    1)
// Far   Plane : P(0,0,z_far) , N=(0		  ,				 0,		   -1)
// Top   Plane : P(0,0,0)	 ,  N=(0	      ,	   -cos(fov/2),sin(fov/2))
// Bot   Plane : P(0,0,0)	 ,  N=(0		  ,     cos(fov/2),sin(fov/2))
// Left Plane : P(0,0,0)	 ,  N=(cos(fov/2) ,			     0,sin(fov/2))
// Right  Plane : P(0,0,0)	 ,  N=(-cos(fov/2),			     0,sin(fov/2))


void init_frustum_planes(float fovx,float fovy, float z_near, float z_far) {
    float cos_half_fovx = cos(fovx / 2);
    float cos_half_fovy = cos(fovy / 2);
    float sin_half_fovx = sin(fovx / 2);
    float sin_half_fovy = sin(fovy / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fovx;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fovx;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fovx;

    frustum_planes[TOP_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fovy;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[BOT_FRUSTUM_PLANE].point = vec3_new(0, 0, 0);
    frustum_planes[BOT_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOT_FRUSTUM_PLANE].normal.y = cos_half_fovy;
    frustum_planes[BOT_FRUSTUM_PLANE].normal.z = sin_half_fovy;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_near);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = vec3_new(0, 0, z_far);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}


polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, text2_t t0, text2_t t1, text2_t t2) {
    polygon_t polygon = {
        .vertices = { v0, v1, v2 },
        .textcoords = {t0,t1,t2},
        .num_vertices = 3
    };
    return polygon;
}

void clip_polygon_against_plane(polygon_t* polygon, int plane) {
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    // Declare a static array of inside vertices that will be part of the final polygon returned via parameter
    vec3_t inside_vertices[MAX_NUM_POLY_VERTICES];
    text2_t inside_textcoords[MAX_NUM_POLY_VERTICES];
    int num_inside_vertices = 0;

    // Start the current vertex with the first polygon vertex and the current texture coordinate 
    vec3_t* current_vertex = &polygon->vertices[0];
    text2_t* current_textcoords = &polygon->textcoords[0];
    
    //last polygon vertex and last texture coordinate
    vec3_t* previous_vertex = &polygon->vertices[polygon->num_vertices - 1];
    text2_t* previous_textcoords = &polygon->textcoords[polygon->num_vertices - 1];


    // Calculate the dot product of the current and previous vertex
    float current_dot = 0;
    float previous_dot = vec3_dot(vec3_subtract(*previous_vertex, plane_point), plane_normal);

    // Loop all the polygon vertices while the current is different than the last one
    while (current_vertex != &polygon->vertices[polygon->num_vertices]) {
        current_dot = vec3_dot(vec3_subtract(*current_vertex, plane_point), plane_normal);

        // If we changed from inside to outside or from outside to inside
        if (current_dot * previous_dot < 0) {

            // Find the interpolation factor t
            float t = previous_dot / (previous_dot - current_dot);

            // Calculate the intersection point I = Q1 + t(Q2-Q1)
            vec3_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z = float_lerp(previous_vertex->z, current_vertex->z, t)
            };
                
            //This is done above                
            //= vec3_clone(current_vertex);                   // I =        Qc
            //intersection_point = vec3_subtract(intersection_point, *previous_vertex); // I =       (Qc-Qp)
            //intersection_point = vec3_mul(intersection_point, t);                     // I =      t(Qc-Qp)
            //intersection_point = vec3_add(intersection_point, *previous_vertex);      // I = Qp + t(Qc-Qp)

            //Use the linear interpolation (lerp) formula to get the iterpolated U and V texture coordinates
            text2_t interpolated_textcoord = {
                .u = float_lerp(previous_textcoords->u,current_textcoords->u,t),
                .v = float_lerp(previous_textcoords->v,current_textcoords->v,t)
            };

            // Insert the intersection point to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(&intersection_point);
            inside_textcoords[num_inside_vertices] = text2_clone(&interpolated_textcoord);
            num_inside_vertices++;
        }

        // Current vertex is inside the plane
        if (current_dot > 0) {
            // Insert the current vertex to the list of "inside vertices"
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            inside_textcoords[num_inside_vertices] = text2_clone(current_textcoords);

            num_inside_vertices++;
        }

        // Move to the next vertex and next texture coordinate
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_textcoords = current_textcoords;
        current_vertex++;
        current_textcoords++;
    }

    // At the end, copy the list of inside vertices into the destination polygon (out parameter)
    for (int i = 0; i < num_inside_vertices; i++) {
        polygon->vertices[i] = vec3_clone(&inside_vertices[i]);
        polygon->textcoords[i] = text2_clone(&inside_textcoords[i]);
    }
    polygon->num_vertices = num_inside_vertices;
}

void clip_polygon(polygon_t* polygon) 
{
	clip_polygon_against_plane(polygon, LEFT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, RIGHT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, TOP_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, BOT_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, NEAR_FRUSTUM_PLANE);
	clip_polygon_against_plane(polygon, FAR_FRUSTUM_PLANE);

}

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangle) 
{
    int index0 = 0;
    for (int i = 0; i < polygon->num_vertices - 2; i++) {
        int index1 = i + 1;
        int index2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(polygon->vertices[index0]);
        triangles[i].texcoords[0] = (polygon->textcoords[index0]);

        triangles[i].points[1] = vec4_from_vec3(polygon->vertices[index1]);
        triangles[i].texcoords[1] = (polygon->textcoords[index1]);

        triangles[i].points[2] = vec4_from_vec3(polygon->vertices[index2]);
        triangles[i].texcoords[2] = (polygon->textcoords[index2]);
    }

    *num_triangle = polygon->num_vertices - 2;
}



