#ifndef CLIPPING_H
#define CLIPPIG_H

#include "vector.h"
#include "triangle.h"

#define MAX_NUM_POLY_VERTICES 10
#define MAX_NUM_POLY_TRIANGLE 10

enum {
	LEFT_FRUSTUM_PLANE,
	RIGHT_FRUSTUM_PLANE,
	TOP_FRUSTUM_PLANE,
	BOT_FRUSTUM_PLANE,
	NEAR_FRUSTUM_PLANE,
	FAR_FRUSTUM_PLANE,
};

typedef struct {
	vec3_t point;
	vec3_t normal;

}plane_t;


typedef struct {
	vec3_t vertices[MAX_NUM_POLY_VERTICES];
	text2_t textcoords[MAX_NUM_POLY_VERTICES];
	int num_vertices;

}polygon_t;

polygon_t create_polygon_from_triangle(vec3_t v0, vec3_t v1, vec3_t v2, text2_t t0, text2_t t1, text2_t t2);

void init_frustum_planes(float fovx,float fovy, float z_near, float z_far);

void clip_polygon_against_plane(polygon_t* polygon, int plane);

void clip_polygon(polygon_t* polygon);

void triangles_from_polygon(polygon_t* polygon, triangle_t triangles[], int* num_triangles);

#endif // !CLIPPING_H
