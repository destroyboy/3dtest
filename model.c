#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "matrix.h"
#include "model.h"
#include "triangle.h"

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
    char *b = fgets( buf, 256, f );
    if ( b == NULL )
      break;
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

void model_screenDrawSquare( Model_t *model, int left, int top, int length, int color )
{
  int y;
  for ( y = top; y < top + length; y++ )
    memset( &model->screen[ y * model->length + left ], color, length );
}

// rasterize a 16x16 block - this needs to be optimized probably to a bunch
// of special cases, by looking at accept can see how many edges need to be
// checked also maybe look at dx and dy and handle seperate cases

void model_drawTriangle16( Model_t *model, Triangle_t *t,
  int left, int top, int length, int accept )
{
  //model_screenDrawSquare(model,left,top,length,t->color);
  if ( !triangle_boundingBoxIntersectsSquare( t, left, top, length ) )
    return;

  accept |= triangle_intersectsWithSquare( t, left, top, length-1 );

  if ( accept & 8  )
  {
    return;
  }
  else if ( accept == 7 )
  {
    model_screenDrawSquare(model,left,top,length,t->color);
  }
  else
  {
    length /= 2;
    if ( length < 1 )
    {
      return;
    }

    model_drawTriangle16( model, t, left, top, length, accept );
    model_drawTriangle16( model, t, left+length,top,length, accept );
    model_drawTriangle16( model, t, left, top+length, length, accept );
    model_drawTriangle16( model, t, left+length,top+length, length, accept );
  }
}
// recursively draw triangle up to the point we get to a 16x16 block
// then call a specalized function
void model_drawTriangle( Model_t *model, Triangle_t *t,
  int left, int top, int length )
{
  if ( !triangle_boundingBoxIntersectsSquare( t, left, top, length ) )
    return;

  int accept = triangle_intersectsWithSquare( t, left, top, length-1 );

  if ( accept & 8  )
  {
    return;
  }
  else if ( accept == 7 )
  {
    model_screenDrawSquare(model,left,top,length,t->color);
  }
  else
  {
    length /= 2;
    if ( length < 1 )
    {
      return;
    }

    if ( length > 8 )
    {
      model_drawTriangle( model, t, left, top, length );
      model_drawTriangle( model, t, left+length,top,length );
      model_drawTriangle( model, t, left, top+length, length );
      model_drawTriangle( model, t, left+length,top+length, length );
    }
    else
    {
      model_drawTriangle16( model, t, left, top, length*2, accept );
    }
  }
}

// length is trhe length in pixels of side of render target
void model_viewRender( Model_t *model )
{
  int i;
  for (i = 0; i < model->tCount; i++)
  {
    Triangle_t t;
    triangle_Populate( &t,
      model->verts_screen[4*model->indexes[3*i+0]+0],
      model->verts_screen[4*model->indexes[3*i+0]+1],
      model->verts_screen[4*model->indexes[3*i+0]+2],

      model->verts_screen[4*model->indexes[3*i+1]+0],
      model->verts_screen[4*model->indexes[3*i+1]+1],
      model->verts_screen[4*model->indexes[3*i+1]+2],

      model->verts_screen[4*model->indexes[3*i+2]+0],
      model->verts_screen[4*model->indexes[3*i+2]+1],
      model->verts_screen[4*model->indexes[3*i+2]+2]
    );

    if (t.normal[2] < 0 )
      continue;

    model_drawTriangle(model,&t,0,0,model->length );
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
