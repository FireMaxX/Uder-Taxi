#include <stdio.h>
#include <stdlib.h>

#include "map.h"

#ifndef NAVIGAHELPER_H
#define NAVIGAHELPER_H

Typedef struct {
	Point* routepoint;
	Int count;
}Route;


Route Navigation(Point *start, Point *end, Map *newmap);

#endif