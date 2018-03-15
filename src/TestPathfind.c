#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <assert.h>
#include "constants.h"
#include "map.h"
#include "utils.h"
#include "Pathfind.h"
#include"ServerManagement.h"

Map *map;

/* Load MapInfo from *.map file */
int LoadMapFromFile() {
	char *fileName;
	int i;

	/* Create a Global Pointer to Map */
	map = newMap();
#ifdef DEBUG
	printf("starting loading map\n");
	printf("Loading Map from %s\n", MapPathI);
#endif
	/* Load Map First Try */
	fileName = MapPathI;
	i = loadMap(fileName, map);
	if (i != 0) {	/* Load Map Second Try */
#ifdef DEBUG
		printf("Failed! Error %d\n", i);
		printf("Loading Map from %s\n", MapPathII);
#endif
		fileName = MapPathII;
		i = loadMap(fileName, map);
	}
	if (i != 0) {	/* Load Map Third Try */
#ifdef DEBUG
		printf("Failed! Error %d\n", i);
		printf("Loading Map from %s\n", MapPathIII);
#endif
		fileName = MapPathIII;
		i = loadMap(fileName, map);
	}
	if (i != 0) {	/* Load Map Fail */
#ifdef DEBUG
		printf("Failed! Error %d\n", i);
#endif
		printf("LoadMap Fail. Abort!\n");
		return 0;
	}
#ifdef DEBUG
	printf("Loading Map Successfully!\n");
#endif
	return 1;
}

int main(void){
	printf("starting loading map\n");
	Point* j;
	int i;
	/* Load Map */
	i = LoadMapFromFile();
	assert(i);

	/*All informaion of the map will be print here*/
	printMap(map);
	Point start, end;
	/*Please modify the start and end Point Here*/
	start.x = 3;
	start.y = 20;
	end.x = 18;
	end.y = 6;
	ILIST *Route;		/*The Route List*/
	Route = Pathfind(start, end, map);
	PrintList(Route);

	j=NextStep(Route);
	printf("Next_Pos is(%d,%d)\n",j->x, j->y);

	j=NULL;

	j= FindStand(&end,map);
	printf("The Nearest stand is (%d,%d)\n",j->x, j->y);

	start.x = 18;
	start.y = 6;
	end.x = 20;
	end.y = 34;
	ILIST *Route2;		/*The Route List*/
	Route2 = Pathfind(start, end, map);
	PrintList(Route2);
	
	ILIST *Route3;
	Route3 = AppendRoute(Route, Route2);
	PrintList(Route3);

	j = FindStand(&end, map);

	printf("Test Pass. Success!\n");
	return 0;
}
