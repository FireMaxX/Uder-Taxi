#ifndef SERVERMANAGEMENT_H
#define SERVERMANAGEMENT_H
#include "Pathfind.h"
#include "map.h"
#include "utils.h"
#include "constants.h"
#include <math.h>
#include<time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#define MAXTAXI 36
#define MAXCARPOOL 2
#define MAXORDER 36
#define SPEED 45

typedef struct list Route;
typedef struct taxistruct Taxi;
typedef struct timedef Time;
typedef struct Clientorder Order;
typedef struct DriveInfo DriverOrder;

struct taxistruct{
	int num;
	int confirm_num[MAXCARPOOL];
	Point *position; 
	int type; /*0(taxi):four seat ;1(van):six seat */
	int status;  /*0:not occupied 1: wait for carpool 2:full*/
	int available_seat;
};

struct timedef{
	int reserve_hour;
	int reserve_min;
};

struct Clientorder{
	int confirm_num;
	Point* pickup_pos;
	Point* target_pos;
	int custom_num;
	double fee;
	Time *pickup_time;
	Time *arrive_time;
	Taxi *taxi;
	int carpool;/*0 not carpool 1 carpool*/
};

struct DriveInfo{
	int confirm_num[MAXCARPOOL];
	Taxi *taxi;
	Route *taxiRoute;
};

/*importand function: calculate route time in Minute*/
int RouteTime(Route *route);

/*important function: initial taxi when create*/
Taxi *newTaxi(int type, Point location);

/*importand function: return fee in double */
double Fee(Point *start,Route *route,Map *newMap);

/*important function: find the nearest taxi*/
Taxi *Find_Taxi(Point *pickup_pos,int custom_num, Taxi *TaxiManagement[MAXTAXI],Map *newMap);

/*important function: give the next step in the route, WARNING consider last point in the route*/
Point *NextStep(Route *route);

Route *AppendRoute(Route *r1, Route *r2);

/*important function: find nearest stand*/
Point* FindStand(Point *position, Map*newMap);

/*reserved function, don't need to know, please refer to the find_taxi function above*/
Taxi **SortTaxi(Point *pickup_pos, Taxi *TaxiManagement[MAXTAXI],Map *newMap);

/*---------------------------------------------------------------------------------------*/
/*order part*/

void AppendReserveOrder(Order *list[],Order *newOrder);

void AppendDriverOrder(DriverOrder *list[],DriverOrder *newOrder);

void AppendOrder(Order *Orders[], Order *newOrder);

void RemoveReserveOrder(Order *list[],int index );

void RemoveDriverOrder(DriverOrder *list[],int confirm_num);

void RemoveOrder(Order *Orders[],int confirm_num);

/*input needed Navigation period and convert to local Time*/
Time *convertTime(int navitime);

/*process client request, find taxi, calculate fee & time and create route info to every taxi*/
DriverOrder *ProcessOrder(Order *order,Taxi *TaxiManagement[MAXTAXI], Map *map);

int ChangeDriverRoute(DriverOrder *order,Order *newOrder,Map *map);


/*time period for every taxi do nextstep()*/
int delay();

int request();

Order *CreateNewOrder(int confirm_num,int custom_num, Point *pickup_pos, Point *target_pos);

DriverOrder *CreateNewDriverOrder(int confirm_num);

#endif
