/* TestGTKMain.c: Independent Test Mode Client, built to test UI only */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "map.h"
#include "GTKMain.h"
#include "constants.h"
#include "ClientMap.h"


Map *map;					/* Globle pointer to the whole Map structure */
Point taxi;					/* Structure variable to save taxi's location */
BigP position;				/* Return value of the first GTK Window(Request Window) */
char RealRe[SLEN];			/* Global Variable share with Request/GTK */
int FF;						/* Global Variable share with Confirmation/GTK */
char GTKbuffer[100];		/* Global Message buffer to communicate with Map Window */
int Jsig;
GtkWidget *Window;
GtkLabel *GTKMessage = NULL;
GtkWidget *GTKMap = NULL;
int GTKMaplength;
int GTKMapwidth;

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
	char fetch[25];
	int choice;
	int errorcode;
	do {
	printf("Client GTK Test Module Start, make your choice:\n");
	printf("1.Client Request Collect Window Demo\n");
	printf("2.Information Confirm Window Demo\n");
	printf("3.Notice Info Window Demo\n");
	printf("4.Alert Info Window Demo\n");
	printf("5.Test Map\n");
	printf("6.Type 6 to Exit\n");
	printf("------------------------------------------\n");
	fgets(fetch, sizeof(fetch), stdin);
	choice = atoi(fetch);
	switch (choice) {
		case 1: {
			GTK1(0, 0);
			break;
		}
		case 2: {
			GTK2(0, 0, "Test Order #90001: Taxi7 will pick you up around 10:15. Arrival time is around 11:42. Total Fee $8.75");
			break;
		}
		case 3: {
			GTK3(0, 0, "Test Notice Information shows here");
			break;
		}
		case 4: {
			GTK4(0, 0, "Test Alert", "Test Alert Information shows here");
			break;
		}
		case 5: {
			/* Load Map */
			errorcode = LoadMapFromFile();
			assert(errorcode);
			GTKMaplength = map->col * 25;
			GTKMapwidth = map->row * 25;

			taxi.x = 3;
			taxi.y = 8;
			Window = CreateWindow(0, 0);
			while (!Jsig) {
				UpdateWindow();
			}
			break;
		}
		default: {
			break;
		}
	}
	} while (choice != 6);
	return 0;
}
