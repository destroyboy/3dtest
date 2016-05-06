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


static void dumpMask( uint64_t mask )
{
  int x, y;
  for ( y = 0; y < 8; y++ )
  {
    for ( x = 0; x < 8; x++ )
    {
      printf( "%d", ( mask & ( ((uint64_t)1) << ( x + 8 * y ) ) ) != 0 );
    }
    printf("\n");
  }
  printf("\n");
}

// assign trangles to 64 bins (8x8 grid of bins)
static void model_binAssign( Model_t *model )
{
  int bx, by;
  int bl = model->length/8;
  int i;
  for ( i = 0; i < model->tCount; i++ )
  {
    uint64_t mask = 0;
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
      goto assign_mask;

    for ( by = 0; by < 8; by++ )
    {
      for ( bx = 0; bx < 8; bx++ )
      {
        if ( triangle_boundingBoxIntersectsSquare( &t, bx*bl, by*bl, bl ) )
        {
          mask |= ((uint64_t)1) << ( bx + by * 8 );
        }
      }
    }
assign_mask:
    model->binmask[i] = mask;
  }
}

void model_viewEnd( Model_t *model )
{
  int i;
  for ( i = 0; i < model->vCount*4; i+=4 )
    matrix_Transform( model->m, model->verts, model->verts_screen, i );
  model_viewZOrder( model );
  model_binAssign( model );
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

// rasterize a 4x4 block - this needs to be optimized probably to a bunch
// of special cases, by looking at accept can see how many edges need to be
// checked also maybe look at dx and dy and handle seperate cases

void model_drawTriangle4x4( Model_t *model, Triangle_t *t,
  int left, int top, int length, int accept )
{
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

    int edge0 = 0;
    int edge1 = 1;

    switch ( accept )
    {
      case 0 :
      {
        int Sx, Sy;
        float dx[3] = { t->dx[0], t->dx[1], t->dx[2] };
        float dy[3] = { t->dy[0], t->dy[1], t->dy[2] };
        float C[3]  = { t->C[0],  t->C[1],  t->C[2]  };
        float det[3] = { dx[0]*top - dy[0]*left + C[0],
                         dx[1]*top - dy[1]*left + C[1],
                         dx[2]*top - dy[2]*left + C[2]};
        unsigned char *screen = &model->screen[top*model->length+left];
        int width = model->length;
        int color = t->color;
        int i = 0;

        for ( Sy = 4; Sy > 0; Sy-- )
        {
          for ( Sx = 4; Sx > 0; Sx-- )
          {
            if ( det[0] > 0 && det[1] > 0 && det[2] > 0 )
               screen[ i ] = color;
            det[0] -= dy[0];
            det[1] -= dy[1];
            det[2] -= dy[2];
            i++;
          }
          det[0] += dx[0] + 4*dy[0];
          det[1] += dx[1] + 4*dy[1];
          det[2] += dx[2] + 4*dy[2];
          i += width - 4;
        }
        break;
      }
      case 1 : //edge0 = 1; edge1 = 2;
        edge0++;
      case 2 : //edge0 = 0; edge1 = 2;
        edge1++;
      case 4 : //edge0 = 0; edge1 = 1;
      {
        float dx[2] = { t->dx[edge0], t->dx[edge1] };
        float dy[2] = { t->dy[edge0], t->dy[edge1] };
        float C[2]  = { t->C[edge0],  t->C[edge1]  };
        int color = t->color;
        unsigned char *screen = &model->screen[top*model->length+left];
        int width = model->length;
        float det[2] = { dx[0]*top - dy[0]*left + C[0],
                         dx[1]*top - dy[1]*left + C[1] };
        int Sy, i = 0;
        for ( Sy = 4; Sy > 0; Sy-- )
        {
          uint32_t pixel4 = *((uint32_t*)&screen[i]);

          if ( det[0] > 0 && det[1] > 0 )
          {
              pixel4 &= 0xffffff00;
              pixel4 |= color<<0;
          }

          det[0] -= dy[0];
          det[1] -= dy[1];

          if ( det[0] > 0 && det[1] > 0 )
          {
            pixel4 &= 0xffff00ff;
            pixel4 |= color<<8;
          }

          det[0] -= dy[0];
          det[1] -= dy[1];

          if ( det[0] > 0 && det[1] > 0 )
          {
            pixel4 &= 0xff00ffff;
            pixel4 |= color<<16;
          }

          det[0] -= dy[0];
          det[1] -= dy[1];

          if ( det[0] > 0 && det[1] > 0 )
          {
            pixel4 &= 0x00ffffff;
            pixel4 |= color<<24;
          }

          *((uint32_t*)&screen[i]) = pixel4;

          det[0] += dx[0] + 3*dy[0];
          det[1] += dx[1] + 3*dy[1];
          i+= width;
        }
        break;
      }
      case 1|2 : // edge 2
        edge0++;
        //fallthrough
      case 1|4 : // edge 1
        edge0++;
        //fallthrough
      case 2|4 : // edge 0
      {
        float dx = t->dx[edge0];
        float dy = t->dy[edge0];
        float C  = t->C[edge0];
        int color = t->color;
        unsigned char *screen = &model->screen[top*model->length+left];
        int width = model->length;
        float det = dx*top - dy*left + C;
        int Sy, i = 0;
        for ( Sy = 4; Sy > 0; Sy-- )
        {
          uint32_t pixel4 = *((uint32_t*)&screen[i]);

          if ( det > 0 )
          {
              pixel4 &= 0xffffff00;
              pixel4 |= color<<0;
          }

          det -= dy;

          if ( det > 0 )
          {
            pixel4 &= 0xffff00ff;
            pixel4 |= color<<8;
          }

          det -= dy;

          if ( det > 0 )
          {
            pixel4 &= 0xff00ffff;
            pixel4 |= color<<16;
          }

          det -= dy;

          if ( det > 0 )
          {
            pixel4 &= 0x00ffffff;
            pixel4 |= color<<24;
          }

          *((uint32_t*)&screen[i]) = pixel4;

          det += dx + 3*dy;
          i+= width;
        }
        break;
      }
    }
  }
}
// recursively draw triangle up to the point we get to a 4x4 block
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

    if ( length > 2 )
    {
      model_drawTriangle( model, t, left, top, length );
      model_drawTriangle( model, t, left+length,top,length );
      model_drawTriangle( model, t, left, top+length, length );
      model_drawTriangle( model, t, left+length,top+length, length );
    }
    else
    {
      model_drawTriangle4x4( model, t, left, top, length*2, accept );
    }
  }
}

void model_viewRender( Model_t *model )
{
  int i;
  int bl = model->length/8;
  for (i = 0; i < model->tCount; i++)
  {
    int bx, by;
    uint64_t mask = model->binmask[i];
    if ( mask == 0 )
      continue;

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

    for ( by = 0; by < 8; by++ )
    {
      for ( bx = 0; bx < 8; bx++ )
      {
        if ( ( mask & (((uint64_t)1)<<(bx+by*8)) ) != 0 )
        {
          model_drawTriangle(model,&t,bl*bx,bl*by,bl );
        }
      }
    }
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
