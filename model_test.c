#include <stdio.h>
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
  model_viewRender( model );
  model_viewSave( model, "test" );
  return 0;
}
