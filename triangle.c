#include <math.h>
#include "triangle.h"

void triangle_Normal( Triangle_t *t, float normal[3] )
{
  float Ux = t->dx[0];
  float Uy = t->dy[0];
  float Uz = t->dz[0];

  float Vx = -t->dx[2]; //t->Rx - t->Px;
  float Vy = -t->dy[2];//t->Ry - t->Py;
  float Vz = -t->dz[2];//t->Rz - t->Pz;

  normal[ 0 ] = Uy * Vz - Uz * Vy;
  normal[ 1 ] = Uz * Vx - Ux * Vz;
  normal[ 2 ] = Ux * Vy - Uy * Vx;
}

void triangle_Populate( Triangle_t *t, float Px, float Py, float Pz,
                                       float Qx, float Qy, float Qz,
                                       float Rx, float Ry, float Rz )
{
  t->x[0] = Px; t->x[1] = Qx; t->x[2] = Rx;
  t->y[0] = Py; t->y[1] = Qy; t->y[2] = Ry;

  t->dx[0] = Qx - Px; t->dy[0] = Qy - Py; t->dz[0] = Qz - Pz;
  t->dx[1] = Rx - Qx; t->dy[1] = Ry - Qy; t->dz[1] = Rz - Qz;
  t->dx[2] = Px - Rx; t->dy[2] = Py - Ry; t->dz[2] = Pz - Rz;

  t->C[0] = -t->dx[0] * t->y[0] + t->dy[0] * t->x[0];
  t->C[1] = -t->dx[1] * t->y[1] + t->dy[1] * t->x[1];
  t->C[2] = -t->dx[2] * t->y[2] + t->dy[2] * t->x[2];

  float xMin = 9999999;
  float xMax = -9999999;
  float yMin = 9999999;
  float yMax = -9999999;

  if ( Px < xMin )
    xMin = Px;
  if ( Qx < xMin )
    xMin = Qx;
  if ( Rx < xMin )
    xMin = Rx;

  if ( Py < yMin )
    yMin = Py;
  if ( Qy < yMin )
    yMin = Qy;
  if ( Ry < yMin )
    yMin = Ry;

  if ( Px > xMax )
    xMax = Px;
  if ( Qx > xMax )
    xMax = Qx;
  if ( Rx > xMax )
    xMax = Rx;

  if ( Py > yMax )
    yMax = Py;
  if ( Qy > yMax )
    yMax = Qy;
  if ( Ry > yMax )
    yMax = Ry;

  t->xMin = xMin;
  t->xMax = xMax;
  t->yMin = yMin;
  t->yMax = yMax;

  triangle_Normal( t, t->normal );

  // normalize the z component of the normal!
  float l = sqrt(t->normal[0]*t->normal[0]+
                 t->normal[1]*t->normal[1]+
                 t->normal[2]*t->normal[2]);

  // calculate a color based on the normal
  t->color = 64+(int)(191*t->normal[2]/l);
}

int triangle_boundingBoxIntersectsSquare( Triangle_t *t, int left, int top, int length )
{
  if ( t->xMin >= left + length )
    return 0;
  if ( t->xMax <= left )
    return 0;
  if ( t->yMin >= top + length )
    return 0;
  if ( t->yMax <= top  )
    return 0;
  return 1;
}

#ifdef USE_EXPLICIT_SIMD
typedef float v4sf __attribute__ ((vector_size(16)));
int triangle_intersectsWithSquareSimd( Triangle_t *t, int left, int top, int length )
{
  v4sf dx3 = { t->dx[0], t->dx[1], t->dx[2], 0.0f };
  v4sf dy3 = { t->dy[0], t->dy[1], t->dy[2], 0.0f };
  v4sf C3  = { t->C[0],  t->C[1],   t->C[2], 0.0f };
  v4sf top3 = { top, top, top, 0.0f };
  v4sf left3 = { left,left,left,0.0f };
  v4sf length3 = { length, length, length, 0 };
  v4sf det_tl = dx3*top3           - dy3*left3 + C3;
  v4sf det_tr = dx3*top3           - dy3*(left3+length3) + C3;
  v4sf det_bl = dx3*(top3+length3) - dy3*left3 + C3;
  v4sf det_br = dx3*(top3+length3) - dy3*(left3+length3) + C3;

  int i, accept = 0;

  if ( ( dx3[0] >  0 && dy3[0] >  0 && det_bl[0] <= 0 ) ||
       ( dx3[1] >  0 && dy3[1] >  0 && det_bl[1] <= 0 ) ||
       ( dx3[2] >  0 && dy3[2] >  0 && det_bl[2] <= 0 ) ||
       ( dx3[0] >  0 && dy3[0] <= 0 && det_br[0] <= 0 ) ||
       ( dx3[1] >  0 && dy3[1] <= 0 && det_br[1] <= 0 ) ||
       ( dx3[2] >  0 && dy3[2] <= 0 && det_br[2] <= 0 ) ||
       ( dx3[0] <= 0 && dy3[0] >  0 && det_tl[0] <= 0 ) ||
       ( dx3[1] <= 0 && dy3[1] >  0 && det_tl[1] <= 0 ) ||
       ( dx3[2] <= 0 && dy3[2] >  0 && det_tl[2] <= 0 ) ||
       ( dx3[0] <= 0 && dy3[0] <= 0 && det_tr[0] <= 0 ) ||
       ( dx3[1] <= 0 && dy3[1] <= 0 && det_tr[1] <= 0 ) ||
       ( dx3[2] <= 0 && dy3[2] <= 0 && det_tr[2] <= 0 ))
  {
    return REJECT_EDGEX;
  }

  if ( ( dx3[0] > 0 && dy3[0] > 0 && det_tr[0] > 0 ) ||
       ( dx3[0] > 0 && dy3[0] <= 0 && det_tl[0] > 0 ) ||
       ( dx3[0] <= 0 && dy3[0] > 0 && det_br[0] > 0 )||
       ( dx3[0] <= 0 && dy3[0] <= 0 && det_bl[0] > 0 ) )
  {
    accept|=ACCEPT_EDGE0;
  }

  if ( ( dx3[1] > 0 && dy3[1] > 0 && det_tr[1] > 0 ) ||
       ( dx3[1] > 0 && dy3[1] <= 0 && det_tl[1] > 0 )||
       ( dx3[1] <= 0 && dy3[1] > 0 && det_br[1] > 0 )||
       ( dx3[1] <= 0 && dy3[1] <= 0 && det_bl[1] > 0 ) )
  {
    accept|=ACCEPT_EDGE1;
  }

  if ( ( dx3[2] > 0 && dy3[2] > 0 && det_tr[2] > 0 ) ||
       ( dx3[2] > 0 && dy3[2] <= 0 && det_tl[2] > 0 ) ||
       ( dx3[2] <= 0 && dy3[2] > 0 && det_br[2] > 0 ) ||
       ( dx3[2] <= 0 && dy3[2] <= 0 && det_bl[2] > 0 ) )
  {
    accept|=ACCEPT_EDGE2;
  }

  return accept;
}
#else
int triangle_intersectsWithSquare( Triangle_t *t, int left, int top, int length )
{
  float dx[3] = { t->dx[0], t->dx[1], t->dx[2] };
  float dy[3] = { t->dy[0], t->dy[1], t->dy[2] };
  float C[3] = { t->C[0], t->C[1], t->C[2] };

  float Sx, Sy, det;

  int i, accept = 0;

  for ( i = 0; i < 3; i++ )
  {
    if ( dx[i] > 0 && dy[i] > 0 ) // reject = bottom left
                                  // accept = top right
    {
      // test reject
      Sx = left;
      Sy = top+length;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det <= 0 )
        accept |= REJECT_EDGEX;

      Sx = left+length;
      Sy = top;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det > 0 )
        accept|=(1<<i);
    }
    else if ( dx[i] > 0 && dy[i] <= 0 ) // reject = bottom right
                                    // accept = top left
    {
      // test reject
      Sx = left+length;
      Sy = top+length;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det <= 0 )
        accept |= REJECT_EDGEX;
      // test accept
      Sx = left;
      Sy = top;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det > 0 )
        accept|=(1<<i);
    }
    else if ( dx[i] <= 0 && dy[i] > 0 ) // reject = top left
                                    // accept = bottom right
    {
      Sx = left;
      Sy = top;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det <= 0 )
        accept |= REJECT_EDGEX;
      Sx = left+length;
      Sy = top+length;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det > 0 )
        accept|=(1<<i);
    }
    else //if ( dx[i] <= 0 && dy[i] <= 0 ) // reject = top right
                                     // accept = bottom left
    {
      Sx = left+length;
      Sy = top;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det <= 0 )
        accept |= REJECT_EDGEX;
      Sx = left;
      Sy = top+length;
      det = dx[i]*Sy - dy[i]*Sx + C[i];
      if ( det > 0 )
        accept|=(1<<i);
    }
  }
  return accept;
}
#endif /*USE_EXPLICIT_SIMD*/

// follow convention of rectangle function
// return 8 for NO, 7 for YES
int triangle_containsPoint( Triangle_t *t, int Sx, int Sy )
{
  float dx[3] = { t->dx[0], t->dx[1], t->dx[2] };
  float dy[3] = { t->dy[0], t->dy[1], t->dy[2] };
  float C[3]  = { t->C[0],  t->C[1],  t->C[2]  };

  float det[3] = { dx[0]*Sy - dy[0]*Sx + C[0],
                   dx[1]*Sy - dy[1]*Sx + C[1],
                   dx[2]*Sy - dy[2]*Sx + C[2] };

  if ( det[0] <= 0 || det[1] <= 0 || det[2] <= 0 )
    return REJECT_EDGEX;

  return ACCEPT_EDGE0|ACCEPT_EDGE1|ACCEPT_EDGE2;
}
