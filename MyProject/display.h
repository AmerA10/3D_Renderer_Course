#pragma once
#ifndef  DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#define FPS 100

//1000ms = 1 sec / 30 frames per second, how much time each frame should get
#define FRAME_TARGET_TIME (1000 / FPS)



enum RenderOption {
	WireframeDot, WireframeLine, FilledTriangles, FilledTriangleAndWireframeLines, Textured, Textured_Wire
};

extern bool EnableBackFaceCulling;

bool initialize_window(void);

int get_window_width();
int get_window_height();

bool is_back_face_culling();
bool should_render_filled_triangles();
bool should_render_textured_triangles();
bool should_render_wire_traingles();
bool should_render_dots();

SDL_Window* get_sdl_window();
SDL_Renderer* get_sdl_renderer();

void draw_grid(void);
void draw_rect(int start_x, int start_y, int width, int height, uint32_t color);
void draw_pixel(int x, int y, uint32_t color);
void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void draw_RightAngleTri(int start_x, int start_y, int width, int height, uint32_t color);

void render_color_buffer();
void clear_color_buffer(uint32_t color);
void destroy_window(void);
void clear_z_buffer();


void set_render_option(int method);
void set_back_face_culling(bool mode);

float get_zbuffer_at(int x, int y);
void update_zbuffer_at(int x, int y, float value);

#endif // ! DISPLAY_H

