#pragma once
#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <stdint.h>
#include "vector.h"
#include "texture.h"
#include "upng.h"

//the struct represents one triangle
//each int represents the index of each triangle vertex

//each uv will represent a uv coordinate
typedef struct {
	int a;
	int b;
	int c;
	text2_t a_uv;
	text2_t b_uv;
	text2_t c_uv;
	uint32_t color;
} face_t;


//represents the projected triangle points
typedef struct {
	vec4_t points[3];
	text2_t texcoords[3];
	uint32_t color;
	upng_t* texture;


} triangle_t;

void draw_filled_triangle(int x0, int y0, float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color);

void draw_filled_triangle_pixel(vec4_t point_a, vec4_t point_b, vec4_t point_c, int x, int y, uint32_t color);

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int Mx, int My, uint32_t color);
void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color);

void swap(int* a, int* b);

void draw_textured_triangle(
							int x0, int y0,float z0, float w0, float u0, float v0, 
							int x1, int y1,float z1, float w1, float u1, float v1,
							int x2, int y2,float z2, float w2, float u2, float v2, upng_t* texture
						   );

void draw_texel(
				int x, int y, upng_t* texture,
				vec4_t point_a, vec4_t point_b, vec4_t point_c,
				text2_t a_uv, text2_t b_uv, text2_t c_uv
);

#endif // !TRIANGLE_H
