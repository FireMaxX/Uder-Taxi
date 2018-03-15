#include "Pathfind.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"
#include "utils.h"

ILIST *CreatePosList() {
	ILIST *l;
	l = malloc(sizeof(ILIST));
	if (!l) {
		perror("out of memory!");
		exit(10);
	}
	l->First = NULL;
	l->Last = NULL;
	l->Current=NULL;
	l->Length = 0;
	return l;
};


int checkLandmark(Point pos, Map* map){
	/*check = 0 not in landmark; 1 in landmark*/
	int n,m, tx, ty, bx, by;
	Array *arr = map->landmarks;
	Landmark* landmark;
	m = map->landmarks->size;

	if (pos.y > map -> col || pos.y<0 || pos.x<0 || pos.x > map->row){
		return 1;
	}

	for (n = 0; n < m; n++) 
	{
		landmark = (Landmark *)((char *)arr->items + arr->itemSize * n);
		tx = landmark->topLeft.x;
		ty = landmark->topLeft.y;
		bx = landmark->botRight.x;
		by = landmark->botRight.y;
		if (pos.x> tx && pos.x<bx && pos.y>ty && pos.y< by)
			return 1;
	}
	return 0;	
}

int AppendEntry(ILIST *list, PosListEntry* newEntry) {
	assert(list);
	assert(newEntry);
	if (list->Length == 0) {
		list->First = newEntry;
		list->Last = newEntry;
		newEntry->Prev = NULL;
		newEntry->Next = NULL;
	}
	else if (list->Length >= 1) {
		newEntry->Prev = list->Last;
		list->Last->Next = newEntry;
		newEntry->Next = NULL;
		list->Last = newEntry;
	}
	else { return 0; }
	list->Length++;
#ifdef DEBUG
	printf("newEntry Pos:(%d, %d)  F: %d \n", newEntry->Pos->x, newEntry->Pos->y, newEntry->F);
	printf("listLast Pos:(%d, %d)  F: %d \n", list->Last->Pos->x, list->Last->Pos->y, list->Last->F);
#endif
	return 1;
}

int PrependEntry(ILIST *list, PosListEntry* newEntry) {
	assert(list);
	assert(newEntry);
	if (list->First == NULL) {
		list->First = newEntry;
		list->Last = newEntry;
		newEntry->Prev = NULL;
		newEntry->Next = NULL;
	}
	else {
		list->First->Prev = newEntry;
		newEntry->Next = list->First;
		newEntry->Prev = NULL;
		list->First = newEntry;
	}
	list->Length++;
#ifdef DEBUG
	printf("Successful Prepend map point!\n");
#endif
	return 0;
};


/*void DeleteLastPos(ILIST *list){
	assert(list);
	assert(list -> Last);
	PosListEntry *e;
	e = list -> Last;
	list-> Last -> Prev -> Next = NULL;
	list -> Last = list -> Last -> Prev;
	free(e -> Pos);
	free(e -> Camefrom);
	free(e);
	e = NULL;
	list -> Length--;
	#ifdef DEBUG
	printf("Successful DELEETE last point! \n");
	#endif
};

void DeletePosList(ILIST *list){
	PosListEntry *e, *n;
	assert(list);
	e = list->First;
	while (e){
			n=e -> Next;
			assert(e);
			free(e -> Pos);
			free(e -> Camefrom);
			free(e);
			e=n;
	}
	free(list);
	#ifdef DEBUG
	printf("Successful DELEETE list! \n");
	n#endif
};
*/

PosListEntry* MinFSearch(ILIST *list) { /*return the min F Pos Entry from list  and delete the Entry from the list */
	assert(list);
	if (list->First == NULL) {
		return NULL;
	}
	PosListEntry *e, *k;
	int count;
	count = 0;
	e = list->First;
	if (list->Length == 1) {
		list->First = NULL;
		list->Last = NULL;
		list->Length--;
		return e;
	}
	else {
		k = list->First->Next;
		while(k!=NULL){
			if (e->F > k->F) {
				e = k;
				count++;
			}
			k = k->Next;
		}
	}

#ifdef DEBUG
	printf("MinF half  Pass\n");
#endif
	if (e->Next == NULL) {
		list->Last = e->Prev;                       /*have been modified*/
		e->Prev->Next = NULL;
		e->Next = NULL;
		e->Prev = NULL;
	}
	else if (e->Prev == NULL) {
		list->First = e->Next;
		e->Next->Prev = NULL;
		e->Next = NULL;
		e->Prev = NULL;
	}
	else {
		e->Prev->Next = e->Next;
		e->Next->Prev = e->Prev;
		e->Prev = NULL;
		e->Next = NULL;
	}
	list->Length--;
	k = NULL;

#ifdef DEBUG
	printf("The min F value point is x= %d  y= %d \n", e->Pos->x, e->Pos->y);
#endif
	return e;
}

int Cal_H_value(Point *start, Point *end){
	int x, y;
	x = (end->x >= start->x)? end->x - start->x: start->x - end->x;
	y = (end->y >= start->y)? end->y - start->y: start->y - end->y;
	return x+y;
}

PosListEntry* PointSearch(Point* pos, ILIST* list) { /*return k for find same point, return NULL for not find*/
	PosListEntry *k;
	k = list->First;
#ifdef DEBUG 
	printf("PointSearch\n list: ");
	PrintList(list);
#endif
	while (k != NULL) {
		if ((pos->x == k->Pos->x) && (pos->y == k->Pos->y)) {
#ifdef DEBUG
			printf("Same position find in the list\n");
#endif
			return k;
		}
		else {
			k = k->Next;
		}
	}
	k = NULL;
	return NULL;
}

void PrintList(ILIST *list) {
	PosListEntry *e;
	e = list->First;
	printf("List:  ");
	while (e != NULL) {
		printf("(%d,%d) ", e->Pos->x, e->Pos->y);
		e = e->Next;
	}
	printf("\n");
}

int Create_AdjacentPos(PosListEntry* BestPos, Point* pos, Point* end, ILIST* open, ILIST* close){
	int temp_G;
#ifdef DEBUG
	printf("IN Create_AD pos is (%d,%d)\n", pos->x, pos->y);
#endif
	temp_G = BestPos->G + 1; /*the distance between each point*/
	if (PointSearch(pos, close)== NULL){
			PosListEntry *PrevPosEntry = NULL;
			PrevPosEntry = PointSearch(pos, open);
			if (PrevPosEntry!=NULL){
			#ifdef DEBUG
				printf("PrevPosEntry(%d,%d)\n", PrevPosEntry->Pos->x, PrevPosEntry->Pos->y);
			#endif
				if (PrevPosEntry->G < temp_G){
					PrevPosEntry->Camefrom = BestPos;
					PrevPosEntry->G=temp_G;
					PrevPosEntry->F=temp_G+PrevPosEntry->H;
				}	
				return 0;		
			}
			else{
			#ifdef DEBUG
				printf("AppendEntry start!!----------------------------------------\n");
			#endif 
				PosListEntry* newEntry;
				newEntry = malloc(sizeof(PosListEntry));
				newEntry->Next=NULL;
				newEntry->Prev=NULL;
				newEntry->Camefrom=NULL;
				newEntry->Pos = pos;
				newEntry->H = Cal_H_value(pos, end);
				newEntry->G = temp_G;
				newEntry->F = newEntry->H + newEntry->G;
				newEntry->Camefrom = BestPos;
				AppendEntry(open, newEntry);
			}
	}
	return 0;
}


int Find_AdjacentPoint(PosListEntry* BestPos, Map* map,Point *end, ILIST* open, ILIST* close){
	Point* p1;
	Point* p2;
	Point* p3;
	Point* p4;
	p1=malloc(sizeof(Point));
	p2=malloc(sizeof(Point));
	p3=malloc(sizeof(Point));
	p4=malloc(sizeof(Point));
	p1->x = BestPos->Pos->x+1;
	p1->y = BestPos->Pos->y;
	p2->x = BestPos->Pos->x-1;
	p2->y = BestPos->Pos->y;
	p3->x = BestPos->Pos->x;
	p3->y = BestPos->Pos->y+1;
	p4->x = BestPos->Pos->x;
	p4->y = BestPos->Pos->y-1;
	if (checkLandmark(*p1, map)==0){          
		Create_AdjacentPos(BestPos, p1, end, open, close);
#ifdef DEBUG 
		printf("Open ");
		PrintList(open);
		printf("Close");
		PrintList(close);
#endif	
	}
	if (checkLandmark(*p2, map)==0){
		Create_AdjacentPos(BestPos, p2, end, open, close);
#ifdef DEBUG 
		printf("Open ");
		PrintList(open);
		printf("Close");
		PrintList(close);
#endif
	}
	if (checkLandmark(*p3, map)==0){
		Create_AdjacentPos(BestPos, p3, end, open, close);
#ifdef DEBUG 
		printf("Open ");
		PrintList(open);
		printf("Close");
		PrintList(close);
#endif
	}
	if (checkLandmark(*p4, map)==0){
		Create_AdjacentPos(BestPos, p4, end, open, close);
#ifdef DEBUG 
		printf("Open ");
		PrintList(open);
		printf("Close");
		PrintList(close);
#endif
	}
#ifdef DEBUG
	printf("Add Best point to Close List!!!-===================\n");
#endif
	AppendEntry(close, BestPos);
	return 0;
};

ILIST* Pathfind(Point start, Point goal, Map *map){
	ILIST *close;
	ILIST *open;
	PosListEntry* BestPos;
	close = CreatePosList();
	open = CreatePosList();
	/*innitial and put first entry in the open list */
	PosListEntry* newEntry;
	newEntry = malloc(sizeof(PosListEntry));
	newEntry->Next=NULL;
	newEntry->Prev=NULL;
	newEntry->Camefrom=NULL;
	newEntry->Pos = &start;
	newEntry->H = Cal_H_value(&start, &goal);
	newEntry->G = 0;
	newEntry->F = newEntry->H + newEntry->G;
	newEntry->Camefrom = NULL;
	AppendEntry(open, newEntry);
	while(open->First!=NULL){	
		BestPos=MinFSearch(open);
#ifdef DEBUG
		printf("-----------------------------------------------------BestPos :(%d, %d)  F: %d \n", BestPos->Pos->x, BestPos->Pos->y, BestPos->F);	
#endif

		if (BestPos->Pos->x == goal.x && BestPos->Pos->y == goal.y){
			PosListEntry* TempEntry;
			ILIST* Route;
			Route = CreatePosList();
			int count;
			count = 0;
			TempEntry = BestPos;
			while(TempEntry->Camefrom!=NULL){
#ifdef DEBUG
				printf("(%d ,%d)\n", TempEntry->Pos->x, TempEntry->Pos->y);
#endif
				PrependEntry(Route, TempEntry);
				TempEntry = TempEntry->Camefrom;
				count++;
			}
#ifdef DEBUG
			printf("The total step is %d\n", count);
			PrintList(Route);
#endif
			return Route;
		}
		Find_AdjacentPoint(BestPos, map, &goal, open, close);
#ifdef DEBUG 
		printf("Open ");
		PrintList(open);
		printf("Close");
		PrintList(close);
#endif
	}
	printf("Can't find the path\n");
	return NULL;
}

