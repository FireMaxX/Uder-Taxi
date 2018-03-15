/* TestServerUI.c: Server-UI-Only Test Module */
/* Author: Yifan Xu(Max) */
/* Version: 1.2, 03/09/2017 */
/* V1.0: File Create */
/* V1.1: Fix Bug */
/* V1.2: Delete unrelated part */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <assert.h>
#include <math.h>
#include <gtk/gtk.h>
#include "Dboard.h"
#include "Response.h"
#include "constants.h"
#include "map.h"
#include "utils.h"
#include "Pathfind.h"
#include "ServerManagement.h"

Map *map;
Point start, end;
int Jsig = 0;				/* Variable for multiply clients recognition */
Taxi *taxi[MaxTaxi];
int TaxiAmount = 0;
char buffer[100];
GtkWidget *Window;
GtkLabel *GTKMoney = NULL;
GtkWidget *GTKMap = NULL;

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

int main()
{
	TaxiStand *TS;
	Array *arr;
	int m, i, j;

	Jsig = 0;				/* intialize ID recognition system */

	/* Load Map */
	i = LoadMapFromFile();
	assert(i);

	/* Initialize Taxi pointers */
	m = map->stands->size;
	arr = map->stands;
#ifdef DEBUG
	printf("TAXI: m = %d\n", m);
#endif
	for (i = 0; i < m; i++)
	{
		TS = (TaxiStand *)(arr->items + arr->itemSize * i);
#ifdef DEBUG
		printf("TAXI: TaxiNum in Stand #%d = %d\n", i, TS->numTaxis);
		printf("TAXI: StandLocation is (%d,%d)\n", TS->mark.x, TS->mark.y);
#endif
		for (j = 0; j < TS->numTaxis; j++) {
			taxi[TaxiAmount] = newTaxi(0, TS->mark);
			TaxiAmount++;
		}
	}
#ifdef DEBUG
	printf("TAXI: Total Taxi Amount = %d\n", TaxiAmount);
#endif

	strcpy(buffer, "1 Million :)");
	Window = CreateWindow(0, 0);
	while (!Jsig) {
		UpdateWindow();
	}
	
	deleteMap(map);
	return 0;
}