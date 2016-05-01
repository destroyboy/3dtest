#include <stdio.h>
#include <time.h>
#include "model.h"
#include "matrix.h"

int main()
{
  Model_t *model = model_Create( 1024 );
  model_loadObj( model, "teddy.obj" );
  model_viewBegin( model );
  model_viewTranslate( model, model->length/2, model->length/2, 0.0 );
  model_viewScale( model, 20.0, 20.0, 20.0 );
  model_viewRotateZ( model, 3.141592654 ); // my coordinates are upsidedown
  model_viewEnd( model );
  clock_t start = clock();
  {
    int i;
    for ( i = 0; i < 20; i++)
      model_viewRender( model );
  }
  clock_t stop = clock();
  printf("test took %f seconds\n",((int)stop-(int)start)/1000000.0);
  model_viewSave( model, "teddy" );
  return 0;
}
