#ifndef _STL_READER_
#define _STL_READER_

#include <tgmath.h>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include <inttypes.h>
#include "geometric/algebra.h"
#include "object.h"
#include "geometric/polygon.h"

typedef struct stl_container stl_container;
struct stl_container {
  size_t length;
  object *triangles;
};

stl_container read_stl_file(const char *file_name, double scale, properties material, matrix transformation);

#endif
