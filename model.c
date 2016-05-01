#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "matrix.h"
#include "model.h"

// for a render target of 1024x1024, set length to 1024
Model_t *model_Create( int length )
{
  Model_t *m = ( Model_t *)malloc( sizeof( Model_t ) + length * length );
  assert( m != NULL );
  m->vCount = 0;
  m->tCount = 0;
  m->length = length;
  memset( m->screen, 0, length * length );
  return m;
}

void model_addVertex( Model_t *model, float x, float y, float z )
{
  model->verts[model->vCount*4+0] = x;
  model->verts[model->vCount*4+1] = y;
  model->verts[model->vCount*4+2] = z;
  model->verts[model->vCount*4+3] = 1.0;
  model->vCount++; // !!no bounds checking
  assert( model->vCount < MAX_VERT_INDEX );
}

void model_addTriangle( Model_t *model, int p, int q, int r )
{
  model->indexes[model->tCount*3+0] = p;
  model->indexes[model->tCount*3+1] = q;
  model->indexes[model->tCount*3+2] = r;
  model->tCount++; // !!no bounds checking
  assert( model->tCount < MAX_TRIANGLE_INDEX );
}

void model_viewBegin( Model_t *model )
{
  matrix_Identity( model->m );
}

void model_viewScale( Model_t *model, float sx, float sy, float sz )
{
  float s[16];
  float m[16];
  memcpy(m, model->m, sizeof( m ) ); // make a copy as matrixMultiply can't work inplace
  matrix_Scale( s, sx, sy, sz);
  matrix_Multiply( model->m, m, s );
}

void model_viewTranslate( Model_t *model, float tx, float ty, float tz )
{
  float t[16];
  float m[16];
  memcpy(m, model->m, sizeof( m ) ); // make a copy as matrixMultiply can't work inplace
  matrix_Translate( t, tx, ty, tz );
  matrix_Multiply( model->m, m, t );
}

void model_viewRotateX( Model_t *model, float radians )
{
  float r[16];
  float m[16];
  memcpy(m, model->m, sizeof( m ) ); // make a copy as matrixMultiply can't work inplace
  matrix_RotateX( r, radians );
  matrix_Multiply( model->m, m, r );
}

void model_viewRotateY( Model_t *model, float radians )
{
  float r[16];
  float m[16];
  memcpy(m, model->m, sizeof( m ) ); // make a copy as matrixMultiply can't work inplace
  matrix_RotateY( r, radians );
  matrix_Multiply( model->m, m, r );
}

void model_viewRotateZ( Model_t *model, float radians )
{
  float r[16];
  float m[16];
  memcpy(m, model->m, sizeof( m ) ); // make a copy as matrixMultiply can't work inplace
  matrix_RotateZ( r, radians );
  matrix_Multiply( model->m, m, r );
}

static Model_t *_m; // hack for compare function

static int compare_triangle_zs( const void *a, const void *b )
{

  int *t1 = (int *)a;
  int *t2 = (int *)b;

  //printf("t1 = [%d %d %d] t2 = %d %d %d\n", t1[0],t1[1],t1[2],t2[0],t2[1],t2[2]);

  float t1_Pz = _m->verts_screen[4*t1[0]+2];
  float t1_Qz = _m->verts_screen[4*t1[1]+2];
  float t1_Rz = _m->verts_screen[4*t1[2]+2];

  float t2_Pz = _m->verts_screen[4*t2[0]+2];
  float t2_Qz = _m->verts_screen[4*t2[1]+2];
  float t2_Rz = _m->verts_screen[4*t2[2]+2];

  float z1 = t1_Pz + t1_Qz + t1_Rz;
  float z2 = t2_Pz + t2_Qz + t2_Rz;
  //console.log(z1.toString()+" "+z2.toString())
  return z1 - z2;
}

// didn't implement a z-buffer so just order from back to front.
static void model_viewZOrder( Model_t *model )
{
  _m = model;
  qsort( model->indexes, model->tCount, sizeof(int)*3, compare_triangle_zs );
}

void model_viewEnd( Model_t *model )
{
  int i;
  for ( i = 0; i < model->vCount*4; i+=4 )
  {
    matrix_Transform( model->m, model->verts, model->verts_screen, i );
  }
  model_viewZOrder( model );
}

void model_createCube( Model_t *model )
{
  float cube_data[] = {
      -1.0,-1.0,-1.0, // triangle 1 : begin
      -1.0,-1.0, 1.0,
      -1.0, 1.0, 1.0, // triangle 1 : end
      1.0, 1.0,-1.0, // triangle 2 : begin
      -1.0,-1.0,-1.0,
      -1.0, 1.0,-1.0, // triangle 2 : end
      1.0,-1.0, 1.0,
      -1.0,-1.0,-1.0,
      1.0,-1.0,-1.0,
      1.0, 1.0,-1.0,
      1.0,-1.0,-1.0,
      -1.0,-1.0,-1.0,
      -1.0,-1.0,-1.0,
      -1.0, 1.0, 1.0,
      -1.0, 1.0,-1.0,
      1.0,-1.0, 1.0,
      -1.0,-1.0, 1.0,
      -1.0,-1.0,-1.0,
      -1.0, 1.0, 1.0,
      -1.0,-1.0, 1.0,
      1.0,-1.0, 1.0,
      1.0, 1.0, 1.0,
      1.0,-1.0,-1.0,
      1.0, 1.0,-1.0,
      1.0,-1.0,-1.0,
      1.0, 1.0, 1.0,
      1.0,-1.0, 1.0,
      1.0, 1.0, 1.0,
      1.0, 1.0,-1.0,
      -1.0, 1.0,-1.0,
      1.0, 1.0, 1.0,
      -1.0, 1.0,-1.0,
      -1.0, 1.0, 1.0,
      1.0, 1.0, 1.0,
      -1.0, 1.0, 1.0,
      1.0,-1.0, 1.0 };

  model->tCount = 0;
  model->vCount = 0;

  int i;

  for ( i = 0; i < sizeof(cube_data)/(sizeof(float)*9); i++)
  {
    model_addVertex(model,cube_data[9*i+0], cube_data[9*i+1], cube_data[9*i+2]);
    model_addVertex(model,cube_data[9*i+3], cube_data[9*i+4], cube_data[9*i+5]);
    model_addVertex(model,cube_data[9*i+6], cube_data[9*i+7], cube_data[9*i+8]);
    model_addTriangle(model,i*3+0,i*3+1,i*3+2);
  }
}

void model_loadObj( Model_t *model, char *filename )
{
  FILE *f = fopen(filename, "rt");
  char buf[256];
  char command[8];
  float v[3];
  do
  {
    fgets( buf, 256, f );
    sscanf( buf, "%s %f %f %f", &command[0], &v[0], &v[1], &v[2] );
    //printf("%s %f %f %f\n", command, v[0], v[1], v[2] );
    switch( command[0] )
    {
      case 'v' :
        model_addVertex( model, v[0], v[1], v[2] );
        break;
      case 'f' :
        model_addTriangle( model, v[0]-1, v[1]-1, v[2]-1 );
        break;
      default :
        break;
    }
  }
  while(!feof(f));


}

int model_triangleIntersectsWithSquare( Model_t *model, int triangle_index,
  int left, int top, int length, int accept )
{
  //triangle_index = triangle_index % 100;
  float Px = model->verts_screen[4*model->indexes[3*triangle_index+0]+0];
  float Py = model->verts_screen[4*model->indexes[3*triangle_index+0]+1];

  float Qx = model->verts_screen[4*model->indexes[3*triangle_index+1]+0];
  float Qy = model->verts_screen[4*model->indexes[3*triangle_index+1]+1];

  float Rx = model->verts_screen[4*model->indexes[3*triangle_index+2]+0];
  float Ry = model->verts_screen[4*model->indexes[3*triangle_index+2]+1];

  float dx[3] = { Qx - Px, Rx - Qx, Px - Rx };
  float dy[3] = { Qy - Py, Ry - Qy, Py - Ry };
  float x[3] = { Px, Qx, Rx };
  float y[3] = { Py, Qy, Ry };

  float Sx, Sy, det;

  int i;

  for ( i = 0; i < 3; i++ )
  {
    if ( accept & (1 << i) )
      continue;

    if ( dx[i] > 0 && dy[i] > 0 ) // reject = bottom left
                                  // accept = top right
    {
      // test reject
      Sx = left;
      Sy = top+length;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);

      if ( det <= 0 )
      {
        return -1;
      }
      // test accept
      Sx = left+length;
      Sy = top;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det > 0 )
        accept|=(1<<i);
    }
    else if ( dx[i] > 0 && dy[i] <= 0 ) // reject = bottom right
                                    // accept = top left
    {
      // test reject
      Sx = left+length;
      Sy = top+length;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det <= 0 )
        return -1;
      // test accept
      Sx = left;
      Sy = top;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det > 0 )
        accept|=(1<<i);
    }
    else if ( dx[i] <= 0 && dy[i] > 0 ) // reject = top left
                                    // accept = bottom right
    {
      Sx = left;
      Sy = top;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det <= 0 )
        return -1;
      Sx = left+length;
      Sy = top+length;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det > 0 )
        accept|=(1<<i);
    }
    else //if ( dx[i] <= 0 && dy[i] <= 0 ) // reject = top right
                                     // accept = bottom left
    {
      Sx = left+length;
      Sy = top;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det <= 0 )
        return -1;
      Sx = left;
      Sy = top+length;
      det = dx[i]*(Sy-y[i])-dy[i]*(Sx-x[i]);
      if ( det > 0 )
        accept|=(1<<i);
    }
  }
  //console.log(accept);
  return accept;
}

void model_screenDrawRect( Model_t *model, int left,
  int top, int width, int height, int color )
{
  int x, y;
  for ( y = top; y < top + height; y++ )
    for ( x = left; x < left + width; x++ )
      model->screen[ y * model->length + x ] = color;
}

void model_drawTriangle( Model_t *model, int triangle_index, int left, int top,
                        int length, int color, int accept )
{
  int intersect = model_triangleIntersectsWithSquare( model, triangle_index,
                    left, top, length-1, accept );

  if ( intersect == -1 )
  {
    return;
  }
  else if ( intersect == 7 )
  {
    model_screenDrawRect(model,left,top,length,length,color);
  }
  else
  {
    length /= 2;
    if ( length < 1 )
    {
      return;
    }

    model_drawTriangle(
      model, triangle_index, left,       top,        length, color, accept);
    model_drawTriangle(
      model, triangle_index, left+length,top,        length, color, accept);
    model_drawTriangle(
      model, triangle_index, left,       top+length, length, color, accept);
    model_drawTriangle(
      model, triangle_index, left+length,top+length, length, color, accept);
  }
}

void model_triangleNormal( Model_t *model, float normal[3], int triangle_index )
{
  float Px = model->verts_screen[4*model->indexes[3*triangle_index+0]+0];
  float Py = model->verts_screen[4*model->indexes[3*triangle_index+0]+1];
  float Pz = model->verts_screen[4*model->indexes[3*triangle_index+0]+2];

  float Qx = model->verts_screen[4*model->indexes[3*triangle_index+1]+0];
  float Qy = model->verts_screen[4*model->indexes[3*triangle_index+1]+1];
  float Qz = model->verts_screen[4*model->indexes[3*triangle_index+1]+2];

  float Rx = model->verts_screen[4*model->indexes[3*triangle_index+2]+0];
  float Ry = model->verts_screen[4*model->indexes[3*triangle_index+2]+1];
  float Rz = model->verts_screen[4*model->indexes[3*triangle_index+2]+2];

  float Ux = Qx - Px;
  float Uy = Qy - Py;
  float Uz = Qz - Pz;

  float Vx = Rx - Px;
  float Vy = Ry - Py;
  float Vz = Rz - Pz;

  normal[ 0 ] = Uy * Vz - Uz * Vy;
  normal[ 1 ] = Uz * Vx - Ux * Vz;
  normal[ 2 ] = Ux * Vy - Uy * Vx;
}

// length is trhe length in pixels of side of render target
void model_viewRender( Model_t *model )
{
  int i;
  for (i = 0; i < model->tCount; i++)
  {
    float normal[3];
    model_triangleNormal( model, normal, i);

    if (normal[2] < 0 )
    {
      continue;
    }

      // normalize the z component of the normal!
    float l = sqrt(normal[0]*normal[0]+
                   normal[1]*normal[1]+
                   normal[2]*normal[2]);

    // calculate a color based on the normal
    int color = 64+(int)(192*normal[2]/l);
    model_drawTriangle(model,i,0,0,model->length, color, 0);
  }
}

void model_viewSave( Model_t *model, char *name )
{
  FILE *f;
  char filename[256];
  sprintf( filename, "%s%dx%d.raw", name, model->length, model->length );
  f = fopen(filename,"wb");
  assert( f != NULL);
  fwrite( &model->screen[0], 1, model->length*model->length, f );
  fclose(f);
}
