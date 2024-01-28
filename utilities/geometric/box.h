#ifndef _BOUNDING_BOX_
#define _BOUNDING_BOX_

#include "algebra.h"

typedef struct box box;
struct box {
  double min_x;
  double min_y;
  double min_z;
  double max_x;
  double max_y;
  double max_z;
  vector center;
};

#endif
