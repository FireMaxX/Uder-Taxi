#include"ServerManagement.h"
#include "constants.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
/*importand function: calculate route time in Minute*/
int RouteTime(Route *route){
  int time;
  time = 60*route->Length/(int)SPEED; /*change the unit of time into munites*/
  return time;
 #ifdef DEBUG
    printf("Successfully calculate route time");
#endif
}

/*important function: initial taxi when create*/
Taxi* newTaxi(int type, Point location) {

    Taxi *newTaxi = malloc(sizeof(Taxi));	
#ifdef DEBUG
	if (newTaxi != NULL) {
		printf("Create a newTaxi success!\n");
	}
#endif
	newTaxi->position = malloc(sizeof(Point));
	newTaxi->position->x = location.x;
	newTaxi->position->y = location.y;
    newTaxi->status = 0;
	newTaxi->type = type;
	if (type == 0)
		newTaxi->available_seat = 4;
	else
		newTaxi->available_seat = 6;

	  return newTaxi;
}	
/*importand function: return fee in double */
double Fee(Point *start,Route *route,Map* newMap){
  int i=0;
  double fee = BaseFee;
  Point start_point;
  Point end_point;
  TaxiStand* stand;
  stand = newMap->stands->items; 
  start_point.x = start->x;
  start_point.y = start->y;
  end_point.x = route->Last->Pos->x;
  end_point.y = route->Last->Pos->y;

    while(i<newMap->stands->size){
    if ((start_point.x == stand[i].mark.x&&start_point.y == stand[i].mark.y)||(end_point.x ==stand[i].mark.x&&end_point.y ==stand[i].mark.y))
      fee = 0;
	i++;
}

  fee = fee + (double)(route->Length)*DisCharge/4.00;
#ifdef DEBUG
    printf("successfully calculate fee");
#endif
  return fee;
}
/*important function: find the nearest taxi*/
Taxi *Find_Taxi(Point* pickup_pos,int custom_num, Taxi *TaxiManagement[MAXTAXI],Map *newMap){
	int i=0;
	Taxi *taxi = malloc(sizeof(Taxi));
	Taxi **SortedTaxi =malloc(sizeof(Taxi *)*MAXTAXI);
	SortedTaxi = SortTaxi(pickup_pos,TaxiManagement,newMap);
	while (i<MAXTAXI){
		if(SortedTaxi[i]->status ==0 && custom_num<=SortedTaxi[i]->available_seat){
			taxi = SortedTaxi[i];
			break;
		}
		i++;
	}
	if (i == MAXTAXI)
		taxi = NULL;
 #ifdef DEBUG
    printf("successfully find taxi\n");
#endif
	return taxi;
}

/*important function: give the next step in the route, WARNING consider last point in the route*/
Point *NextStep(Route *route){
	Point *next_pos=malloc(sizeof(Point));
	if (route->Current==NULL){
		route->Current=route->First;
		next_pos=route->Current->Pos;
	}
	else if(route->Current==route->Last){
		next_pos=route->Last->Pos;
	}
	else{
		route->Current = route->Current->Next;
		next_pos = route->Current->Pos;
	}
#ifdef DEBUG
    printf("successfully move to next step\n");
#endif
	return next_pos;
}

Route *AppendRoute(Route *r1, Route *r2){
	r1->Length = r1->Length + r2->Length;
	r1->Last->Next = r2->First;
	r1->Last = r2->Last;
	free(r2);
  #ifdef DEBUG
    printf("successfully append route\n");
#endif
	return r1;
}

/*important function: find nearest stand*/
Point* FindStand(Point *pos, Map* map){   
	int x, y, n, m, f, count;
	Array *arr = map->stands;
	TaxiStand* TStand;
	Point* nearStand;
	count = 999999;
	m = map->stands->size;
	for (n = 0; n < m; n++)
	{
		TStand = (TaxiStand *)(arr->items + arr->itemSize * n);
		x = (TStand->mark.x >= pos->x)? TStand->mark.x - pos->x: pos->x - TStand->mark.x;
		y = (TStand->mark.x >= pos->y)? TStand->mark.y - pos->y: pos->y - TStand->mark.y;
		f = x + y;
		if (f<count){
			count=f;
			nearStand=&(TStand->mark);
		}
	}
	return nearStand;	
}

/*reserved function, don't need to know, please refer to the find_taxi function above*/
Taxi **SortTaxi(Point *pickup_pos, Taxi *TaxiManagement[MAXTAXI],Map *newMap){
	int i = 0,j,k;
	int dis1=0,dis2=0;
  Taxi **temp=malloc(sizeof(Taxi *)*MAXTAXI);
	Taxi **SortedTaxi = malloc(sizeof(Taxi *)*MAXTAXI);
	SortedTaxi[0]=TaxiManagement[0];
	temp[0] = TaxiManagement[0];
	for(j=0;j<MAXTAXI;j++){
		k=0;
		dis1 = Cal_H_value(pickup_pos,TaxiManagement[j]->position);
		while(k<j){
			dis2= Cal_H_value(pickup_pos,SortedTaxi[k]->position);		
			if(dis1<=dis2){
				for(i=k;i<j;i++)	
					temp[i-k]= SortedTaxi[i];			
				SortedTaxi[k] = TaxiManagement[j];			
				for (i=k+1;i<=j;i++)
					SortedTaxi[i] = temp[i-k-1];				
				break;
			}				
			k++;
		}
		if (k==j){
			SortedTaxi[j] = TaxiManagement[j];
		}
	}
  #ifdef DEBUG
    printf("successfully sorted taxi\n");
#endif
	return SortedTaxi;
}

/*-----------------------------------------------------------------------*/
Order *CreateNewOrder(int confirm_num,int custom_num, Point *pickup_pos, Point *target_pos){
  Order *order = malloc(sizeof(Order));
  order->pickup_pos = pickup_pos;
  order->target_pos = target_pos;
  order->pickup_time = NULL;
  order->arrive_time = NULL;
  order->taxi = NULL;
  order->custom_num = custom_num;
  order->confirm_num = confirm_num;
  return order;
}


DriverOrder *CreateNewDriverOrder(int confirm_num){
	DriverOrder *order =malloc(sizeof(DriverOrder));
	order->taxi = malloc(sizeof(Taxi));
	order->taxiRoute = CreatePosList();
	order->confirm_num[0] = confirm_num;
	return order;
}

void AppendReserveOrder(Order *list[],Order *newOrder){
	int i=0;
	while(list[i]->confirm_num!=0){
		i++;		
	}
	list[i]=newOrder;
  #ifdef DEBUG
    printf("successfully append reserve order \n");
#endif
}
void AppendDriverOrder(DriverOrder *list[],DriverOrder *newOrder){
	int i=0;
	while(list[i]->confirm_num[0]!=0){
		i++;		
	}
	list[i]=newOrder;
  #ifdef DEBUG
    printf("successfully append driverorder \n");
#endif
}

void AppendOrder(Order *list[],Order *newOrder){
	int i=0;
	while(list[i]->confirm_num!=0){
		i++;		
	}
	list[i]=newOrder;
  #ifdef DEBUG
    printf("successfully append order \n");
#endif
}


void RemoveReserveOrder(Order *list[],int index ){
	int i;
	Order *newList[MAXORDER];
	for(i=0;i<MAXORDER-1;i++){
		if(i<index)
			newList[i] = list[i];
		else if(i>index)
			newList[i] = list[i+1];
	}
	list = newList;
 #ifdef DEBUG
    printf("successfully remove reserve order \n");
#endif 
}
void RemoveDriverOrder(DriverOrder *list[],int confirm_num){
	int i,j;
	DriverOrder *newList[MAXTAXI];
	for(i=0;i<MAXTAXI-1;i++){
		if(list[i]->confirm_num[0]==confirm_num)
			break;
	}
	for(j=0;j<MAXTAXI-1;j++){
		if(j<i)
			newList[j] = list[j];
		else if(i>j)
			newList[j] = list[j+1];
	}
	list = newList;
 #ifdef DEBUG
    printf("successfully remove driver order \n");
#endif 
}

void RemoveOrder(Order *list[], int confirm_num){
	int i,j;
	Order *newList[OrderNum];
	for(i=0;i<OrderNum-1;i++){
		if(list[i]->confirm_num==confirm_num)
			break;
	}
	for(j=0;j<OrderNum-1;j++){
		if(j<i)
			newList[j] = list[j];
		else if(i>j)
			newList[j] = list[j+1];
	}
	list = newList;

}

/*input needed Navigation period and convert to local Time*/
Time *convertTime(int navitime){
  Time *localtm = malloc(sizeof(Time));
	time_t rawtime;
  struct tm* info;
	time(&rawtime);
	info = localtime(&rawtime);
  if (60 - info->tm_min>navitime){
    localtm->reserve_hour = info->tm_hour;
    localtm->reserve_min = info->tm_min + navitime; 
  }
  else{
    if (info->tm_hour==23)
      localtm->reserve_hour = 0;
    else
    localtm->reserve_hour = info->tm_hour+1;
    localtm->reserve_min = info->tm_min+navitime-60;
  }
#ifdef DEBUG
    printf("successfully convert to local time\n");
#endif
  return localtm;
}

/*process client request, find taxi, calculate fee & time and create route info to every taxi*/
DriverOrder *ProcessOrder(Order *order,Taxi *TaxiManagement[MAXTAXI], Map *map){
	DriverOrder *driverinfo = malloc(sizeof(DriverOrder));
	/*DriverOrder *driverorder;*/
	Taxi *taxi = malloc(sizeof(Taxi));
	Route *pickuproute =CreatePosList();
  	Route *route = CreatePosList();
  	Route *finalroute=CreatePosList();
	Route *backroute=CreatePosList();
  Time *picktm = malloc(sizeof(Time));
  Time *arrivetm=malloc(sizeof(Time));
  int pickup_time;
  int arrive_time;
  double fee;
  Point taxistart;
  Point taxigoal;
  Point navigoal;
	Point *nearstand;
	
	driverinfo->confirm_num[0] = order->confirm_num;
	taxi = Find_Taxi(order->pickup_pos, order->custom_num,TaxiManagement,map);
	taxi->confirm_num[0] = order->confirm_num;
	taxistart.x = taxi->position->x;
	taxistart.y = taxi->position->y;
	taxigoal.x = order->pickup_pos->x;
	taxigoal.y = order->pickup_pos->y;
	navigoal.x = order->target_pos->x;
	navigoal.y = order->target_pos->y;
	
#ifdef DEBUG
	printf("%d %d %d %d %d %d\n",taxistart.x,taxistart.y,taxigoal.x,taxigoal.y,navigoal.x,navigoal.y);
	printf("order num%d\n",order->confirm_num);
	printf("taxi_confirm[0] = %d\n",taxi->confirm_num[0]);
#endif
	pickuproute = Pathfind(taxistart,taxigoal,map);
	route=Pathfind(taxigoal,navigoal,map);
	fee = Fee(&taxigoal,route,map);
    pickup_time = RouteTime(pickuproute);
	arrive_time = RouteTime(route);
	picktm = convertTime(pickup_time);
	arrivetm = convertTime(arrive_time);
	order->pickup_time = picktm;
	order->arrive_time = arrivetm;
	if (pickuproute->First!=pickuproute->Last){
		finalroute= AppendRoute(pickuproute,route);
	}else{
		finalroute=route;	
	}
	nearstand=FindStand(&navigoal,map);
	if (navigoal.x==nearstand->x && navigoal.y==nearstand->y){
		finalroute=finalroute;
	}else{
		backroute=Pathfind(navigoal,*nearstand,map);
		finalroute=AppendRoute(finalroute,backroute);
	}
#ifdef DEBUG
	PrintList(finalroute);
	printf("printed finalroute\n");
#endif
	if (order->carpool==1){
		taxi->status =1;
		taxi->available_seat = (4+2*taxi->type)-order->custom_num;
    order->fee = fee/2;
	}
	else{
		taxi->status =1;
		taxi->available_seat =0;
    order->fee = fee;
	}
	driverinfo->taxi = taxi;
	driverinfo->taxiRoute = finalroute;
#ifdef DEBUG
    printf("successfully Process Client order\n");
#endif
	return driverinfo;
}

int ChangeDriverRoute(DriverOrder *order,Order *newOrder,Map *map){
	Route *finalRoute = CreatePosList();
	Route *newRoute=CreatePosList();
	Route *oldRoute=CreatePosList();
	Point *taxipos = malloc(sizeof(Point));
	Point *target1= malloc(sizeof(Point));
	Point *target2= malloc(sizeof(Point));
	taxipos = order->taxi->position;
	order->confirm_num[1]=newOrder->confirm_num;
	target1 = order->taxiRoute->Last->Pos;
	assert(0);
	target2 = newOrder->target_pos;
	if(abs(taxipos->x-target1->x)+abs(taxipos->y-target1->y) <= abs(taxipos->x-target2->x)+abs(taxipos->y-target2->y)){
		newRoute =Pathfind(*target1,*target2,map);
		finalRoute = AppendRoute(order->taxiRoute,newRoute);
	}
	else{
		newRoute = Pathfind(*(order->taxi->position),*target2,map);
		oldRoute = Pathfind(*target2,*target1,map);
		finalRoute = AppendRoute(newRoute,oldRoute);
	}
	order->taxiRoute = finalRoute;
  #ifdef DEBUG
    printf("successfully change driver route\n");
#endif
	return 0;
}



/*time period for every taxi do nextstep()*/
int delay(){
/*
	time_t rawtime;
  struct tm* info;
	time(&rawtime);
	info = localtime(&rawtime);
	if (info->tm_sec %10 ==0)
		return 1;
	else 
		return 0;
	*/
	int delay;
	printf("input 1 to delay and move taxi or 0 to continue ");
	scanf("%d",&delay);
	return delay;	
  #ifdef DEBUG
    printf("successfully process delay()");
#endif

}
int request(){
	int request;
	printf("input 1 to request a taxi or 0 to exit");
	scanf("%d",&request);
	return request;
}

