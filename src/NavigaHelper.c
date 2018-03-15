#include <stdio.h>
#include <stdlib.h>

#include "map.h"
#include "NavigaHelper.h"


Route Navigation(Point *start, Point *end, Map *newmap) {
	int xs, ys, xe, ye, xi, yi, i;
	xs = start->x;
	ys = start->y;
	xe = end->x;
	ye = end->y;
	xi = xs;
	yi = ys;

	Point route[100];
	if (xi != xe &&yi != ye) {
		if (xi >= xe &&yi >= ye) {
			while (i < ys - ye) {
				yi--;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
			while (i < ((xs - xe) + (ys - ye))) {
				xi--;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
		}
		if (xi < xe &&yi > ye) {
			while (i < ys - ye) {
				yi--;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
			while (i < ((xe - xs) + (ys - ye))) {
				xi++;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
		}
		if (xi <= xe &&yi <= ye) {
			while (i < ye - ys) {
				yi++;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
			while (i < ((xe - xs) + (ye - ys))) {
				xi++;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
		}
		if (xi > xe &&yi < ye) {
			while (i < ye - ys) {
				yi++;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
			while (i < ((xs - xe) + (ye - ys))) {
				xi--;
				route[i].x = xi;
				route[i].y = yi;
				i++
			}
		}
		return route[100];
	}
	else {
#ifdef DEBUG
		printf("You are already there!\n");
#endif	
		return 0;
	}
	
	


}
