#include <stdio.h>
#include <time.h>
#include "model.h"
#include "matrix.h"

//#define CUBE
//#define USE_EXPLICIT_SIMD

int main()
{
  clock_t start, stop;
  Model_t *model = model_Create( 1024 );
#ifdef CUBE
  model_createCube( model );
#else
  model_loadObj( model, "teddy.obj" );
#endif
  model_viewBegin( model );
  model_viewTranslate( model, model->length/2, model->length/2, 0.0 );
#ifdef CUBE
  model_viewScale( model, model->length/2, model->length/2, model->length/2 );
#else
  model_viewScale( model, 20.0, 20.0, 20.0 );
  model_viewRotateZ( model, 3.141592654 );
#endif
   // my coordinates are upsidedown
  model_viewEnd( model );
  start = clock();
  {
    int i;
    for ( i = 0; i < 500; i++)
      model_screenDrawSquare( model, 0, 0, model->length, 0x00 );
  }
  stop = clock();
  printf("sanity test took %f seconds\n",((int)stop-(int)start)/1000000.0);

  start = clock();
  {
    int i;
    for ( i = 0; i < 500; i++)
      model_viewRender( model );
  }
  stop = clock();
  printf("test took %f seconds\n",((int)stop-(int)start)/1000000.0);
  model_viewSave( model, "teddy" );
  return 0;
}
