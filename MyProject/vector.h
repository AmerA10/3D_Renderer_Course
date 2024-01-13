#pragma once
#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	float x;
	float y;
} vec2_t;

typedef struct {
	float x;
	float y;
	float z;

}vec3_t;

typedef struct {
	float x;
	float y;
	float z;
	float w;
}vec4_t;

//TODO: add functions to manipulate vectors 2D and 3D
//////////////////////////////////////////////////////////
/// Vector 3D Functions
/////////////////////////////////////////////////////////

float vec3_length(vec3_t v);

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_subtract(vec3_t a, vec3_t b);
vec3_t vec3_mul(vec3_t v, float factor);
vec3_t vec3_div(vec3_t v, float factor);

float vec3_dot(vec3_t a, vec3_t b);
vec3_t vec3_cross(vec3_t a, vec3_t b);
void vec3_normalize(vec3_t* v);

vec3_t vec3_from_vec4(vec4_t v);

vec3_t vec3_new(float x, float y, float z);
vec3_t vec3_clone(vec3_t* v);




//////////////////////////////////////////////////////////
/// Vector 2D Functions
/////////////////////////////////////////////////////////

float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_subtract(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t v, float factor);
vec2_t vec2_div(vec2_t v, float factor);
void vec2_normalize(vec2_t* v);

vec2_t vec2_new(float x, float y);

//////////////////////////////////////////////////////////
/// Vector 4D Functions (Mostly conversions)
/////////////////////////////////////////////////////////
vec4_t vec4_from_vec3(vec3_t v);
vec2_t vec2_from_vec4(vec4_t v);

#endif // !VECTOR_H
