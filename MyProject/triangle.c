#include "triangle.h"
#include "display.h"
#include "vector.h"
#include "swap.h"
#include "stdio.h"
#include "math.h"

//tHIS ENTIRE PROJECT NEEDS REFACTORING BUT WORKS :)


vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p) {

	vec2_t ac = vec2_subtract(c, a);
	vec2_t ab = vec2_subtract(b, a);
	vec2_t ap = vec2_subtract(p, a);
	vec2_t pc = vec2_subtract(c, p);
	vec2_t pb = vec2_subtract(b, p);

	//Compue the area f the fll parallegram/triangle ABC using 2D cross product
	float area_parallelogram_abc = (ac.x * ab.y - ac.y * ab.x); // ||AC x AB ||
	
	// Alpha is the area of the small parallelogram/traingle PBC divided by the area of the parallelogram/triangle ABC
	float alpha = (pc.x * pb.y - pc.y * pb.x) / area_parallelogram_abc; 

	// Beta is teh area of the small parallelogram/triangle APC divided by the area of the full parallelogram/triangle ABC
	float beta = (ac.x * ap.y - ac.y * ap.x) / area_parallelogram_abc; 

	float gamma = 1 - alpha - beta;

	vec3_t weights = { alpha, beta, gamma };
	return weights;
}


////////////////////////////////////////////////////////////////////////////////
/// Function to draw the textured pixel at position x and y using interpolation, the performance fucking sucks dick but it is very readable

void draw_texel(
	int x, int y, upng_t* texture, 
	vec4_t point_a, vec4_t point_b, vec4_t point_c, 
	text2_t a_uv, text2_t b_uv, text2_t c_uv
	//float u0, float v0, float u1, float v1, float u2, float v2
) 
{
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);

	// Calculate the barycentric coordinates of our point 'p' inside the triangle
	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// Variables to store the interpolated values of U, V, and also 1/w for the current pixel
	float interpolated_u;
	float interpolated_v;
	float interpolated_reciprocal_w;

	// Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
	interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
	interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

	// Also interpolate the value of 1/w for the current pixel
	interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;


	// Now we can divide back both interpolated values by 1/w
	interpolated_u /= interpolated_reciprocal_w;
	interpolated_v /= interpolated_reciprocal_w;

	// Map the UV coordinate to the full texture width and height, it makes sense that the result of this operation should not be bigger than the texture_width/height and larger than or equal to 0

	//Get Mesh texture width and height dimensions
	int texture_width = upng_get_width(texture);
	int texture_height = upng_get_height(texture);


	//Using the modules is a hacky solution but work for now
	int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
	int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;


	//Adjust 1/w so the pixel that are closer to the camer, have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	//If we already drew a pixel at this position here, only update the pixel at the same position, if the depth value of what we are drawing is going to place the pixel infront of it (less of it)
	//Basically only place a pixel if the new pixel is going to be placed infront of the existing pixel

	if (x >= 0 && x < get_window_width() && y >= 0 && y < get_window_height()) {

		uint32_t* texture_buffer = (uint32_t*) upng_get_buffer(texture);


		if (interpolated_reciprocal_w < get_zbuffer_at(x,y)) {


			//Draw a pixel at poistion x,y with the color that comes from the texture
			draw_pixel(x, y, texture_buffer[(texture_width * tex_y) + tex_x]);

			//Update the -zbuffer value with the 1/w of this current pixel

			update_zbuffer_at(x,y,interpolated_reciprocal_w);
		}
	}
}

//TODO implement triangle.h functions
///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////s
void draw_textured_triangle(
	int x0, int y0, float z0, float w0, float u0, float v0,
	int x1, int y1, float z1, float w1, float u1, float v1,
	int x2, int y2, float z2, float w2, float u2, float v2, upng_t* texture)
{


	//Loop all the pixels of the triangle to render them based on the color that comes from the textured array

	//Need to sort the vertices by their y coordinate ascending (y0 < y1 < y2) Making sure to swap the UVs as well
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
		float_swap(&u1, &u2);
		float_swap(&v1, &v2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
		float_swap(&u0, &u1);
		float_swap(&v0, &v1);
	}
	
	//Flip the V component to account for inverted UV-coordinates (v grows downwards)
	v0 = 1 - v0;
	v1 = 1 - v1;
	v2 = 1 - v2;

	//Create vectors point after we sort the vertices
	vec4_t point_a = {x0,y0, z0, w0};
	vec4_t point_b = {x1,y1, z1, w1};
	vec4_t point_c = {x2,y2, z2, w2};

	text2_t a_uv = {
		.u = u0,
		.v = v0
	};
	text2_t b_uv = {
		.u = u1,
		.v = v1
	};
	text2_t c_uv = {
		.u = u2,
		.v = v2
	};

	//Render the upper part of the triangle
	////////////////////////////////////////////////////

	//The slop usually is how much y is increasing given the x is increasing one one
	//What we want is, given that y is increasing one by one, how much is our x incresing?
	float inv_slope1 = 0;
	float inv_slope2 = 0;

	//we don't want to divide by a negative or a zero and we want a float

	if (y1 - y0 != 0)
	{
		inv_slope1 = (float)(x1 - x0) / abs((y1 - y0));
	}

	if (y2 - y0 != 0) {
		inv_slope2 = (float)(x2 - x0) / abs((y2 - y0));
	}

	if (y1 - y0 != 0) {
		//go from y0 to y1
		for (int y = y0; y <= y1; y++)
		{
			//what the fuck is this doing? This somehow creates two x points each placed at the respective slopes of x0 to x1 (start) and x0 to x2 (end)
			int x_start = x1 + (y - y1) * inv_slope1;
			int x_end = x0 + (y - y0) * inv_slope2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++)
			{
				//TODO: Draw our pixel with the color that comes from the texture
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
			}
		}
	}

	//Render the LOWER part of the triangle
	////////////////////////////////////////////////////

	//The slop usually is how much y is increasing given the x is increasing one one
	//What we want is, given that y is increasing one by one, how much is our x incresing?
	inv_slope1 = 0;
	inv_slope2 = 0;

	//we don't want to divide by a negative or a zero and we want a float

	if (y2 - y1 != 0)
	{
		inv_slope1 = (float)(x2 - x1) / abs((y2 - y1));
	}

	if (y2 - y0 != 0) {
		inv_slope2 = (float)(x2 - x0) / abs((y2 - y0));
	}

	if (y2 - y1 != 0) {
		//go from y0 to y1
		for (int y = y1; y <= y2; y++)
		{
			//what the fuck is this doing? This somehow creates two x points each placed at the respective slopes of x0 to x1 (start) and x0 to x2 (end)
			int x_start = x1 + (y - y1) * inv_slope1;
			int x_end = x0 + (y - y0) * inv_slope2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++)
			{
				//TODO: Draw our pixel with the color that comes from the texture
				draw_texel(x, y, texture, point_a, point_b, point_c, a_uv,b_uv,c_uv);

			}
		}
	}
}

void draw_filled_triangle_pixel(vec4_t point_a, vec4_t point_b, vec4_t point_c,int x, int y, uint32_t color) 
{
	vec2_t p = { x, y };
	vec2_t a = vec2_from_vec4(point_a);
	vec2_t b = vec2_from_vec4(point_b);
	vec2_t c = vec2_from_vec4(point_c);

	// Calculate the barycentric coordinates of our point 'p' inside the triangle
	vec3_t weights = barycentric_weights(a, b, c, p);

	float alpha = weights.x;
	float beta = weights.y;
	float gamma = weights.z;

	// Interpolate the value of 1/w for the current pixel
	float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

	// Adjust 1/w so the pixels that are closer to the camera have smaller values
	interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

	// Only draw the pixel if the depth value is less than the one previously stored in the z-buffer

	if (x >= 0 && x < get_window_width() && y >= 0 && y < get_window_height()) {

		if (interpolated_reciprocal_w < get_zbuffer_at(x,y)) {
			// Draw a pixel at position (x,y) with a solid color
			draw_pixel(x, y, color);

			// Update the z-buffer value with the 1/w of this current pixel
			update_zbuffer_at(x,y,interpolated_reciprocal_w);
		}
	}
}


void draw_filled_triangle(int x0, int y0,float z0, float w0, int x1, int y1, float z1, float w1, int x2, int y2, float z2, float w2, uint32_t color)
{

	//Loop all the pixels of the triangle to render them based on the color that comes from the textured array

	//Need to sort the vertices by their y coordinate ascending (y0 < y1 < y2) Making sure to swap the UVs as well
	// We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}
	if (y1 > y2) {
		int_swap(&y1, &y2);
		int_swap(&x1, &x2);
		float_swap(&z1, &z2);
		float_swap(&w1, &w2);
	}
	if (y0 > y1) {
		int_swap(&y0, &y1);
		int_swap(&x0, &x1);
		float_swap(&z0, &z1);
		float_swap(&w0, &w1);
	}

	//Create vectors point after we sort the vertices
	vec4_t point_a = { x0, y0, z0, w0 };
	vec4_t point_b = { x1, y1, z1, w1 };
	vec4_t point_c = { x2, y2, z2, w2 };


	//Render the upper part of the triangle
	////////////////////////////////////////////////////

	//The slop usually is how much y is increasing given the x is increasing one one
	//What we want is, given that y is increasing one by one, how much is our x incresing?
	float inv_slope1 = 0;
	float inv_slope2 = 0;

	//we don't want to divide by a negative or a zero and we want a float
	if (y1 - y0 != 0) inv_slope1 = (float)(x1 - x0) / abs(y1 - y0);
	if (y2 - y0 != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

	if (y1 - y0 != 0) {
		//go from y0 to y1
		for (int y = y0; y <= y1; y++)
		{
			//what the fuck is this doing? This somehow creates two x points each placed at the respective slopes of x0 to x1 (start) and x0 to x2 (end)
			int x_start = x1 + (y - y1) * inv_slope1;
			int x_end = x0 + (y - y0) * inv_slope2;

			if (x_end < x_start) {
				int_swap(&x_start, &x_end);
			}

			for (int x = x_start; x < x_end; x++)
			{

				//TODO: Draw our pixel with the color that comes from the texture
				//draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
					// Calculate the barycentric coordinates of our point 'p' inside the triangle
				draw_filled_triangle_pixel(point_a, point_b, point_c, x,y, color);

			}
		}
	}

	//Render the LOWER part of the triangle
	////////////////////////////////////////////////////

	//The slop usually is how much y is increasing given the x is increasing one one
	//What we want is, given that y is increasing one by one, how much is our x incresing?
	inv_slope1 = 0;
	inv_slope2 = 0;

	//we don't want to divide by a negative or a zero and we want a float

	if (y2 - y1 != 0) inv_slope1 = (float)(x2 - x1) / abs(y2 - y1);
	if (y2 - y0 != 0) inv_slope2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope1;
            int x_end = x0 + (y - y0) * inv_slope2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
				draw_filled_triangle_pixel(point_a, point_b, point_c, x, y, color);
            }
        }
    }

}

void fill_flat_top_triangle(int x1, int y1, int Mx, int My, int x2, int y2, uint32_t color) {

	//First find the two slopes for the two triangle sides, since our slope
	//is calculating for the y as the independent, i.e, we are looking we looking for 
	//How much the X is changing not the Y
	//if this division by delta y is 0 then we have a division by zero problem

	float inv_slope_1 = (float)(x2 - x1) / (y2 - y1);
	float inv_slope_2 = (float)(x2 - Mx) / (y2 - My);

	float x_start = x2;
	float x_end = x2;

	//Loop through the scanlines from top to bottom
	for (int i = y2; i >= My; i--)
	{
		draw_line(x_start, i, x_end, i, color);
		x_start -= inv_slope_1;
		x_end -= inv_slope_2;

	}

}

