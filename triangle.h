#ifndef TRIANGLE_H
#define TRIANGLE_H

typedef struct
{
  float x[3],y[3],dx[3],dy[3],dz[3],C[3];
  float xMin, xMax, yMin, yMax;
  float normal[3];
  int   color;
}
Triangle_t;

#define ACCEPT_NONE  (0)
#define ACCEPT_EDGE0 (1<<0)
#define ACCEPT_EDGE1 (1<<1)
#define ACCEPT_EDGE2 (1<<2)
#define REJECT_EDGEX (1<<3)

#define CHECK_EDGE0 (ACCEPT_EDGE1|ACCEPT_EDGE2)
#define CHECK_EDGE1 (ACCEPT_EDGE0|ACCEPT_EDGE2)
#define CHECK_EDGE2 (ACCEPT_EDGE0|ACCEPT_EDGE1)


void triangle_Normal( Triangle_t *t, float normal[3] );
void triangle_Populate( Triangle_t *t, float Px, float Py, float Pz,
  float Qx, float Qy, float Qz, float Rx, float Ry, float Rz);
int triangle_boundingBoxIntersectsSquare( Triangle_t *t, int left, int top, int length );
int triangle_intersectsWithSquare( Triangle_t *t, int left, int top, int length );
int triangle_containsPoint( Triangle_t *t, int Sx, int Sy );

#endif /*TRIANGLE_H*/
