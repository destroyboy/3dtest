#include <stdio.h>
#include <math.h>

void matrix_Dump( float m[16] )
{
  printf("[ %04.3f %04.3f %04.3f %04.3f ]\n", m[0], m[4], m[8],  m[12]);
  printf("[ %04.3f %04.3f %04.3f %04.3f ]\n", m[1], m[5], m[9],  m[13]);
  printf("[ %04.3f %04.3f %04.3f %04.3f ]\n", m[2], m[6], m[10], m[14]);
  printf("[ %04.3f %04.3f %04.3f %04.3f ]\n", m[3], m[7], m[11], m[15]);
  printf("\n");
}

void matrix_Identity( float m[16] )
{
  m[0]  = m[5]  = m[10] = m[15] = 1.0;
  m[1]  = m[2]  = m[3]  = m[4]  = 0.0;
  m[6]  = m[7]  = m[8]  = m[9]  = 0.0;
  m[11] = m[12] = m[13] = m[14] = 0.0;
}

void matrix_Scale( float *m, float x, float y, float z )
{
  m[1]  = m[2]  = m[3]  = m[4]  = 0.0;
	m[6]  = m[7]  = m[8]  = m[9]  = 0.0;
	m[11] = m[12] = m[13] = m[14] = 0.0;
	m[15]                         = 1.0;
	m[0]                          = x;
	m[5]                          = y;
	m[10]                         = z;
}

void matrix_Translate( float m[16], float x, float y, float z )
{
  m[0] = m[5] = m[10] = m[15] = 1.0;
  m[1] = m[2] = m[3]  = m[4]  = 0.0;
  m[6] = m[7] = m[8]  = m[9]  = 0.0;
  m[11]                       = 0.0;
  m[ 12 ]                     = x;
  m[ 13 ]                     = y;
  m[ 14 ]                     = z;
}


void matrix_RotateX( float m[16], float r )
{
  m[0] = 1.0; m[4] = 0.0;    m[8]  =  0.0;    m[12] = 0.0;
  m[1] = 0.0; m[5] = cos(r); m[9]  = -sin(r); m[13] = 0.0;
  m[2] = 0.0; m[6] = sin(r); m[10] =  cos(r); m[14] = 0.0;
  m[3] = 0.0; m[7] = 0.0;    m[11] =  0.0;    m[15] = 1.0;
}

void matrix_RotateY( float m[16], float r )
{
  m[0] = cos(r); m[4] = 0.0; m[8]  = -sin(r); m[12] = 0.0;
  m[1] = 0.0;    m[5] = 1.0; m[9]  =  0.0;    m[13] = 0.0;
  m[2] = sin(r); m[6] = 0.0; m[10] =  cos(r); m[14] = 0.0;
  m[3] = 0.0;    m[7] = 0.0; m[11] =  0.0;    m[15] = 1.0;
}

void matrix_RotateZ( float m[16], float r )
{
  m[0] =  cos(r); m[4] = -sin(r); m[8]  = 0.0; m[12]  = 0.0;
  m[1] =  sin(r); m[5] =  cos(r); m[9]  = 0.0; m[13]  = 0.0;
  m[2] =  0.0;    m[6] =  0.0;    m[10] = 1.0; m[14] = 0.0;
  m[3] =  0.0;    m[7] =  0.0;    m[11] = 0.0; m[15] = 1.0;
}

void matrix_Multiply( float result[16], float m1[16], float m2[16] )
{
  // First Column
  result[0] = m1[0] * m2[0] + m1[4] * m2[1] + m1[8] * m2[2] + m1[12] * m2[3];
  result[1] = m1[1] * m2[0] + m1[5] * m2[1] + m1[9] * m2[2] + m1[13] * m2[3];
  result[2] = m1[2] * m2[0] + m1[6] * m2[1] + m1[10] * m2[2] + m1[14] * m2[3];
  result[3] = m1[3] * m2[0] + m1[7] * m2[1] + m1[11] * m2[2] + m1[15] * m2[3];

  // Second Column
  result[4] = m1[0] * m2[4] + m1[4] * m2[5] + m1[8] * m2[6] + m1[12] * m2[7];
  result[5] = m1[1] * m2[4] + m1[5] * m2[5] + m1[9] * m2[6] + m1[13] * m2[7];
  result[6] = m1[2] * m2[4] + m1[6] * m2[5] + m1[10] * m2[6] + m1[14] * m2[7];
  result[7] = m1[3] * m2[4] + m1[7] * m2[5] + m1[11] * m2[6] + m1[15] * m2[7];

  // Third Column
  result[8] = m1[0] * m2[8] + m1[4] * m2[9] + m1[8] * m2[10] + m1[12] * m2[11];
  result[9] = m1[1] * m2[8] + m1[5] * m2[9] + m1[9] * m2[10] + m1[13] * m2[11];
  result[10] = m1[2] * m2[8] + m1[6] * m2[9] + m1[10] * m2[10] + m1[14] * m2[11];
  result[11] = m1[3] * m2[8] + m1[7] * m2[9] + m1[11] * m2[10] + m1[15] * m2[11];

  // Fourth Column
  result[12] = m1[0] * m2[12] + m1[4] * m2[13] + m1[8] * m2[14] + m1[12] * m2[15];
  result[13] = m1[1] * m2[12] + m1[5] * m2[13] + m1[9] * m2[14] + m1[13] * m2[15];
  result[14] = m1[2] * m2[12] + m1[6] * m2[13] + m1[10] * m2[14] + m1[14] * m2[15];
  result[15] = m1[3] * m2[12] + m1[7] * m2[13] + m1[11] * m2[14] + m1[15] * m2[15];
}

// r=m*v
// offset in float array (everything is a float array)
void matrix_Transform( float m[16], float *v, float *r, int i )
{
  r[i+0] = m[0]*v[i+0]  + m[4]*v[i+1]  + m[8]*v[i+2]  + m[12]*v[i+3];
  r[i+1] = m[1]*v[i+0]  + m[5]*v[i+1]  + m[9]*v[i+2]  + m[13]*v[i+3];
  r[i+2] = m[2]*v[i+0]  + m[6]*v[i+1]  + m[10]*v[i+2] + m[14]*v[i+3];
  r[i+3] = m[3]*v[i+0]  + m[7]*v[i+1]  + m[11]*v[i+2] + m[15]*v[i+3];
}
