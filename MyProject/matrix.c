#include "matrix.h"
#include "math.h"

mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up)
{

	vec3_t z = vec3_subtract(target, eye); // forward (z) vector
	vec3_normalize(&z);

	vec3_t x = vec3_cross(up, z); // right (x) vector
	vec3_normalize(&x);


	vec3_t y = vec3_cross(z, x); // up (y) vector, no need to normalize this one as both z and x are already normalized


	// | x.x  x.y  x.z  -dot(x,eye) |
	// | y.x  y.y  y.z  -dot(y,eye) |
	// | z.x  z.y  z.z  -dot(z,eye) |
	// |   0      0      0        1 |

	mat4_t view_matrix = 
	{ {
		{x.x,x.y,x.z, -vec3_dot(x,eye)},
		{y.x,y.y,y.z, -vec3_dot(y,eye)},
		{z.x,z.y,z.z, -vec3_dot(z,eye)},
		{  0,  0,  0,                1}

		
	}};


	return view_matrix;

}


// |(h/w)*1/tan(fov/2)			0				 0							0		|
// |				0			1 / tan(fov / 2)				 0							0		|
// |				0			0		zf/(zf-zn)		(-zfar*znear)/(zf-zn)		|
// |				0			0				 1							0		|

//The 1 in the last row is there to save the z value in the w 



mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar) {
	mat4_t m = { {{ 0 }} };
	m.m[0][0] = aspect * (1 / tan(fov / 2));
	m.m[1][1] = 1 / tan(fov / 2);
	m.m[2][2] = zfar / (zfar - znear);
	m.m[2][3] = (-zfar * znear) / (zfar - znear);
	m.m[3][2] = 1.0;
	return m;
}

vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v){


	//multiply the projection matrix by the our original vector
	vec4_t result = mat4_mul_vec4(mat_proj, v);

	//Usually you would do the clipping here but for the sake of this course and having an easier understanding of things, it is done in view space 

	//perform perspective divide with original z-value that is now stored in w

	if (result.w != 0) {

		result.x /= result.w;
		result.y /= result.w;
		result.z /= result.w;

	}


	return result;

}

mat4_t mat4_identity() {
	// |1 0 0 0|
	// |0 1 0 0|
	// |0 0 1 0|
	// |0 0 0 1|

	//array of array
	mat4_t mat =
	{ 
		{

			{1,0,0,0},
			{0,1,0,0},
			{0,0,1,0},
			{0,0,0,1}
		} 
	};

	return mat;
}

mat4_t mat4_make_scale(float scale_x, float scale_y, float scale_z) {
	// |sx 0 0 0|
	// |0 sy 0 0|
	// |0 0 sz 0|
	// |0 0 0  1|

	//array of arrays
	mat4_t mat = mat4_identity();
	mat.m[0][0] = scale_x;
	mat.m[1][1] = scale_y;
	mat.m[2][2] = scale_z;

	return mat;
}

mat4_t mat4_make_translation(float translation_x, float translation_y, float translation_z) {
	// |1 0 0 tx|
	// |0 1 0 ty|
	// |0 0 1 tz|
	// |0 0 0  1|

	//array of arrays
	mat4_t mat = mat4_identity();
	mat.m[0][3] = translation_x;
	mat.m[1][3] = translation_y;
	mat.m[2][3] = translation_z;

	return mat;
}

mat4_t mat4_make_rotation_z(float angle) {

	float c = cos(angle);
	float s = sin(angle);

	mat4_t m = mat4_identity();

	m.m[0][0] = c;
	m.m[0][1] = -s;
	m.m[1][0] = s;
	m.m[1][1] = c;

	return m;

}
mat4_t mat4_make_rotation_x(float angle) {

	float c = cos(angle);
	float s = sin(angle);

	mat4_t m = mat4_identity();

	m.m[1][1] = c;
	m.m[1][2] = -s;
	m.m[2][1] = s;
	m.m[2][2] = c;

	return m;
}
mat4_t mat4_make_rotation_y(float angle) {

	float c = cos(angle);
	float s = sin(angle);

	mat4_t m = mat4_identity();

	m.m[0][0] = c;
	m.m[0][2] = s;
	m.m[2][0] = -s;
	m.m[2][2] = c;

	return m;


}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {

	mat4_t mat = mat4_identity();
	//i is rows
	for (int i = 0; i < 4; i++) {

		//j is columns
		for (int j = 0; j < 4; j++) {

			mat.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];

		}

	}

	return mat;

}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
	vec4_t result;
	result.x = (m.m[0][0] * v.x) + (m.m[0][1] * v.y) + (m.m[0][2] * v.z) + (m.m[0][3] * v.w);
	result.y = (m.m[1][0] * v.x) + (m.m[1][1] * v.y) + (m.m[1][2] * v.z) + (m.m[1][3] * v.w);
	result.z = (m.m[2][0] * v.x) + (m.m[2][1] * v.y) + (m.m[2][2] * v.z) + (m.m[2][3] * v.w);
	result.w = (m.m[3][0] * v.x) + (m.m[3][1] * v.y) + (m.m[3][2] * v.z) + (m.m[3][3] * v.w);

	return result;
}