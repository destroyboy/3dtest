#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "model.h"

void triangle_Normal( Triangle_t *t, float normal[3] );
void triangle_Populate( Triangle_t *t, Model_t *model, int triangle_index );
int triangle_boundingBoxIntersectsSquare( Triangle_t *t, int left, int top, int length );
int triangle_intersectsWithSquare( Triangle_t *t, int left, int top, int length );

#endif /*TRIANGLE_H*/
