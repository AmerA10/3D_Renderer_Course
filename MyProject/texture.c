#include "texture.h"
#include "stdio.h"

text2_t text2_clone(text2_t* t) {
    text2_t result = {
        t->u,t->v
    };

    return result;
}


