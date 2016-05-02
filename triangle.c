#include <math.h>
#include "triangle.h"

void triangle_Normal( Triangle_t *t, float normal[3] )
{
  float Ux = t->Qx - t->Px;
  float Uy = t->Qy - t->Py;
  float Uz = t->Qz - t->Pz;

  float Vx = t->Rx - t->Px;
  float Vy = t->Ry - t->Py;
  float Vz = t->Rz - t->Pz;

  normal[ 0 ] = Uy * Vz - Uz * Vy;
  normal[ 1 ] = Uz * Vx - Ux * Vz;
  normal[ 2 ] = Ux * Vy - Uy * Vx;
}

void triangle_Populate( Triangle_t *t, Model_t *model, int triangle_index )
{
  t->Px = model->verts_screen[4*model->indexes[3*triangle_index+0]+0];
  t->Py = model->verts_screen[4*model->indexes[3*triangle_index+0]+1];
  t->Pz = model->verts_screen[4*model->indexes[3*triangle_index+0]+2];

  t->Qx = model->verts_screen[4*model->indexes[3*triangle_index+1]+0];
  t->Qy = model->verts_screen[4*model->indexes[3*triangle_index+1]+1];
  t->Qz = model->verts_screen[4*model->indexes[3*triangle_index+1]+2];

  t->Rx = model->verts_screen[4*model->indexes[3*triangle_index+2]+0];
  t->Ry = model->verts_screen[4*model->indexes[3*triangle_index+2]+1];
  t->Rz = model->verts_screen[4*model->indexes[3*triangle_index+2]+2];

  t->x[0] = t->Px;
  t->x[1] = t->Qx;
  t->x[2] = t->Rx;

  t->y[0] = t->Py;
  t->y[1] = t->Qy;
  t->y[2] = t->Ry;

  t->PQx = t->Qx - t->Px;
  t->PQy = t->Qy - t->Py;
  t->PQz = t->Qz - t->Pz;

  t->QRx = t->Rx - t->Qx;
  t->QRy = t->Ry - t->Qy;
  t->QRz = t->Rz - t->Qz;

  t->RPx = t->Px - t->Rx;
  t->RPy = t->Py - t->Ry;
  t->RPz = t->Pz - t->Rz;

  t->dx[0] = t->PQx;
  t->dx[1] = t->QRx;
  t->dx[2] = t->RPx;

  t->dy[0] = t->PQy;
  t->dy[1] = t->QRy;
  t->dy[2] = t->RPy;

  t->C[0] = -t->dx[0] * t->y[0] + t->dy[0] * t->x[0];
  t->C[1] = -t->dx[1] * t->y[1] + t->dy[1] * t->x[1];
  t->C[2] = -t->dx[2] * t->y[2] + t->dy[2] * t->x[2];

  float xMin = 9999999;
  float xMax = -9999999;
  float yMin = 9999999;
  float yMax = -9999999;

  if ( t->Px < xMin )
    xMin = t->Px;
  if ( t->Qx < xMin )
    xMin = t->Qx;
  if ( t->Rx < xMin )
    xMin = t->Rx;

  if ( t->Py < yMin )
    yMin = t->Py;
  if ( t->Qy < yMin )
    yMin = t->Qy;
  if ( t->Ry < yMin )
    yMin = t->Ry;

  if ( t->Px > xMax )
    xMax = t->Px;
  if ( t->Qx > xMax )
    xMax = t->Qx;
  if ( t->Rx > xMax )
    xMax = t->Rx;

  if ( t->Py > yMax )
    yMax = t->Py;
  if ( t->Qy > yMax )
    yMax = t->Qy;
  if ( t->Ry > yMax )
    yMax = t->Ry;

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
    return 8;
  }

  if ( ( dx3[0] > 0 && dy3[0] > 0 && det_tr[0] > 0 ) ||
       ( dx3[0] > 0 && dy3[0] <= 0 && det_tl[0] > 0 ) ||
       ( dx3[0] <= 0 && dy3[0] > 0 && det_br[0] > 0 )||
       ( dx3[0] <= 0 && dy3[0] <= 0 && det_bl[0] > 0 ) )
  {
    accept|=1;
  }

  if ( ( dx3[1] > 0 && dy3[1] > 0 && det_tr[1] > 0 ) ||
       ( dx3[1] > 0 && dy3[1] <= 0 && det_tl[1] > 0 )||
       ( dx3[1] <= 0 && dy3[1] > 0 && det_br[1] > 0 )||
       ( dx3[1] <= 0 && dy3[1] <= 0 && det_bl[1] > 0 ) )
  {
    accept|=2;
  }

  if ( ( dx3[2] > 0 && dy3[2] > 0 && det_tr[2] > 0 ) ||
       ( dx3[2] > 0 && dy3[2] <= 0 && det_tl[2] > 0 ) ||
       ( dx3[2] <= 0 && dy3[2] > 0 && det_br[2] > 0 ) ||
       ( dx3[2] <= 0 && dy3[2] <= 0 && det_bl[2] > 0 ) )
  {
    accept|=4;
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
        accept |= 8;

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
        accept |= 8;
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
        accept |= 8;
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
        accept |= 8;
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
