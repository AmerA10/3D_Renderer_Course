#pragma once
#include "camera.h"
static camera_t camera;

static float camera_move_speed = 5.f;
static float camera_rotate_speed = 1.0f;

void init_camera(vec3_t position, vec3_t direction) {
	camera.position = position;
	camera.direction = direction;
	camera.forward_velocity = vec3_new(0, 0, 0);
	camera.pitch = 0.0f;
	camera.yaw = 0.0f;
}


float get_camera_move_speed() {
	return camera_move_speed;
}

float get_camera_rotate_speed() {
	return camera_rotate_speed;
}

camera_t get_camera() {
	return camera;
}

float get_camera_yaw() {
	return camera.yaw;
}

float get_camera_pitch() {
	return camera.pitch;
}

vec3_t get_camera_velocity() {
	return camera.forward_velocity;
}

vec3_t get_camera_direction() {
	return camera.direction;
}

vec3_t get_camera_position() {
	return camera.position;
}
void update_camera_position_y(float val) {
	camera.position.y = val;
}


void update_camera_position(vec3_t v) {
	camera.position = v;
}

void update_camera_direction(vec3_t v) {
	camera.direction = v;
}

void update_camera_forward_velocity(vec3_t v) {
	camera.forward_velocity = v;
}

void update_camera_yaw(float val) {
	camera.yaw = val;
}

void update_camera_pitch(float val) {
	camera.pitch = val;
}