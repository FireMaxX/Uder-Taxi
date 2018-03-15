#ifndef PATHFIND_H
#define PATHFIND_H
#include "map.h"
typedef struct PEntry PosListEntry;
typedef struct list ILIST;

struct PEntry{	
	PosListEntry *Next;
	PosListEntry *Prev;
	Point *Pos;
	PosListEntry *Camefrom;
	int F;
	int G;
	int H;
};

struct list{
	int Length;		 /* Length of the list */
	PosListEntry *First; 			/* Pointer to the first entry, or NULL */
	PosListEntry *Last;
  PosListEntry *Current;
};

void PrintList(ILIST *list);

ILIST *CreatePosList(void);

int checkLandmark(Point pos, Map* newMap);

int AppendEntry(ILIST *list, PosListEntry* newEntry);

int PrependEntry(ILIST *list, PosListEntry* newEntry);

PosListEntry* MinFSearch(ILIST *list);

int Cal_H_value(Point *start, Point *end);

PosListEntry* PointSearch(Point* pos, ILIST* list);

int Create_AdjacentPos(PosListEntry* BestPos, Point *pos, Point *end, ILIST* open, ILIST* close);

int Find_AdjacentPoint(PosListEntry* BestPos, Map* map,Point *end, ILIST* open, ILIST* close);

ILIST* Pathfind(Point start, Point goal, Map *map);

#endif
