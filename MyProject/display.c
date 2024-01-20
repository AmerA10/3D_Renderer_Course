#include "display.h"
#include <stdio.h>


//These variables are no longer global variables and only can only be seen in this file
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static uint32_t* color_buffer = NULL;
static float* z_buffer = NULL;
static SDL_Texture* color_buffer_texture = NULL;
static int window_width = 1280;
static int window_height = 800;
static bool EnableBackFaceCulling = true;
static int render_method = 0;


bool initialize_window(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Set width and height of the SDL window with the max screen resolution
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(1, &display_mode);
	int fullscreen_width = display_mode.w;
	int fullscreen_height = display_mode.h;

	// Create a SDL Window
		window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		fullscreen_width,
		fullscreen_height,
		SDL_WINDOW_FULLSCREEN_DESKTOP
	);
	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// Create a SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	// Allocate the required memory in bytes to hold the color buffer
	color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
	z_buffer = (float*)malloc(sizeof(float) * window_width * window_height);

	// Creating a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGBA32,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);

	return true;
}

void clear_z_buffer() {

	for (int i = 0; i < window_height * window_width; i++) {
		z_buffer[i] = 1.0f;
	}

	//for (int y = 0; y < window_height; y++) 
	//{
		//for (int x = 0; x < window_width; x++) 
		//{
			//window_width * y + x gets the position as if its a 2D grid even though our color grid is a single array
			//z_buffer[(window_width * y) + x] = 1.0f;

		//}
	//}
}

void clear_color_buffer(uint32_t color) {

	for (int i = 0; i < window_width * window_height; i++) {
		color_buffer[i] = color;
	}

	//for (int y = 0; y < window_height; y++) {
		//for (int x = 0; x < window_width; x++) {
			//window_width * y + x gets the position as if its a 2D grid even though our color grid is a single array
			//color_buffer[(window_width * y) + x] = color;

		//}
	//}
}

void draw_grid(void)
{
	int gridMultiple = 30;
	//for (int y = 0; y < window_height; y++) {
	//	for (int x = 0; x < window_width; x++) {
	//		if (x % gridMultiple == 0 || y % gridMultiple == 0) {
	//			color_buffer[(window_width * y) + x] = 0xFF0f4C75;

	//		}
	//	//window_width * y + x gets the position as if its a 2D grid even though our color grid is a single array

	//	}
	//}
	for (int y = 0; y < window_height; y += gridMultiple) {
		for (int x = 0; x < window_width; x += gridMultiple / 2) {

			//back ground dot colors
			color_buffer[(window_width * y) + x] = 0xFFFFFFFF;

			//window_width * y + x gets the position as if its a 2D grid even though our color grid is a single array

		}
	}
}

void draw_pixel(int x, int y, uint32_t color) 
{
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {

		return;

	}
    
	color_buffer[(window_width * y) + x] = color;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= longest_side_length; i++) {
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}


void draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
	//x, y starting top left position
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			int current_x = start_x + i;
			int current_y = start_y + j;
			draw_pixel(current_x, current_y, color);
		}
	}
}
void draw_RightAngleTri(int start_x, int start_y, int width, int height, uint32_t color) {
	//x, y starting top left position
	int initWidth = 1;
	for (int y = start_y; y < start_y + height; y++) {
		for (int x = start_x; x < start_x + initWidth; x++) {
			color_buffer[(window_width * y) + x] = color;
		}
		initWidth++;
	}

}

void render_color_buffer() {

	//using lock and unlock wrong
	//SDL_UnlockTexture(color_buffer_texture);

	//texture pitch is the size of each row
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}


void destroy_window(void) {
	free(color_buffer);
	free(z_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool is_back_face_culling() {
	return EnableBackFaceCulling;
}

void set_back_face_culling(bool mode) {
	EnableBackFaceCulling = mode;
}

void set_render_option(int option) {
	render_method = option;
}

int get_window_width() {
	return window_width;
}

int get_window_height() {
	return window_width;
}

bool should_render_filled_triangles() {
	return (render_method == FilledTriangleAndWireframeLines || render_method == FilledTriangles);
}
bool should_render_textured_triangles() {
	return (render_method == Textured || render_method == Textured_Wire);
}

bool should_render_wire_traingles() {
	return (render_method == WireframeLine || render_method == FilledTriangleAndWireframeLines || render_method == Textured_Wire);
}

bool should_render_dots() {
	return render_method == WireframeDot;
}

float get_zbuffer_at(int x, int y) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return 1.0f;
	}

	return z_buffer[(window_width * y) + x];
}

void update_zbuffer_at(int x, int y, float value) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return;
	}

	z_buffer[(window_width * y) + x] = value;
}