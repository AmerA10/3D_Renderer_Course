#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"


typedef struct {

	vec3_t position;
	vec3_t direction;
	vec3_t forward_velocity;
	float yaw; //rotation around the y-axis
	float pitch; //rotation around the x-axis

}camera_t;

void init_camera(vec3_t position, vec3_t direction);



camera_t get_camera();

float get_camera_move_speed();
float get_camera_rotate_speed();

float get_camera_yaw();
float get_camera_pitch();

vec3_t get_camera_position();
vec3_t get_camera_direction();
vec3_t get_camera_velocity();

void update_camera_position(vec3_t v);
void update_camera_direction(vec3_t v);
void update_camera_forward_velocity(vec3_t v);
void update_camera_yaw(float val);
void update_camera_pitch(float val);

void update_camera_position_y(float val);




#endif // !CAMERA_H
