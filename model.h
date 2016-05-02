#ifndef MODEL_H
#define MODEL_H

#define MAX_VERT_INDEX (32768)
#define MAX_TRIANGLE_INDEX (16384)

typedef struct
{
  float m[16]; // matrix to convert verts to screen space
  float verts[MAX_VERT_INDEX];
  float verts_screen[MAX_VERT_INDEX];
  int   indexes[MAX_TRIANGLE_INDEX];
  int   vCount;
  int   tCount;
  int   length;
  unsigned char screen[1];
}
Model_t;

typedef struct
{
  float Px, Py, Pz, Qx, Qy, Qz, Rx, Ry, Rz;
  float x[3],y[3],dx[3],dy[3];
  float PQx, PQy, PQz, QRx, QRy, QRz, RPx, RPy, RPz;
  float C[3];
  float xMin, xMax, yMin, yMax;
  float normal[3];
  int   color;
}
Triangle_t;

// for a render target of 1024x1024, set length to 1024
Model_t *model_Create( int length );
void model_addVertex( Model_t *model, float x, float y, float z );
void model_addTriangle( Model_t *model, int p, int q, int r );
void model_viewBegin( Model_t *model );
void model_viewScale( Model_t *model, float sx, float sy, float sz );
void model_viewTranslate( Model_t *model, float tx, float ty, float tz );
void model_viewRotateX( Model_t *model, float radians );
void model_viewRotateY( Model_t *model, float radians );
void model_viewRotateZ( Model_t *model, float radians );
void model_viewEnd( Model_t *model );
void model_createCube( Model_t *model );
void model_screenDrawSquare( Model_t *model,
  int left, int top, int length, int color );
void model_drawTriangle( Model_t *model, Triangle_t *t, int left, int top,
  int length );
void model_viewRender( Model_t *model );
void model_viewSave( Model_t *model, char *filename );
void model_loadObj( Model_t *m, char *filename );

#endif /* MODEL_H */
