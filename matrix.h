#ifndef MATRIX_H
#define MATRIX_H

void matrix_Dump( float m[16] );
void matrix_Identity( float m[16] );
void matrix_Scale( float m[16], float x, float y, float z );
void matrix_Translate( float m[16], float x, float y, float z );
void matrix_RotateX( float m[16], float r );
void matrix_RotateY( float m[16], float r );
void matrix_RotateZ( float m[16], float r );
void matrix_Multiply( float result[16], float m1[16], float m2[16] );
void matrix_Transform( float m[16], float *v, float *r, int i );

#endif /* MATRIX_H */
