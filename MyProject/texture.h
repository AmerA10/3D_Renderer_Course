#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

//each vertex of a face will have this coordinate
typedef struct {
	float u;
	float v;
}text2_t;


text2_t text2_clone(text2_t* t);

#endif // TEXTURE_H
